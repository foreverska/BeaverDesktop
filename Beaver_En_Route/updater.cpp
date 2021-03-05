#include <unistd.h>

#include <QThread>

#include "gui.h"
#include "updater.h"
#include "firmware.h"
#include "logger.h"

#define ENDOFREBOOT (0.20)
#define ENDOFUPDATE (0.90)

#define BOOTLOADERACK   0
#define BOOTLOADERNACK   -1
#define UPDATEROK       0
#define UPDATERNOK      -1

#define BOTTOMADDR  0x00001000
#define TOPADDR     0x00040000

void Updater::SetupStates()
{
    connect(&m_initialState, SIGNAL(entered()), this, SLOT(initialFunction()));
    connect(&m_rebootState, SIGNAL(entered()), this, SLOT(rebootFunction()));
    connect(&m_updateState, SIGNAL(entered()), this, SLOT(updateFunction()));
    connect(&m_nextSectorState, SIGNAL(entered()), this, SLOT(nextSectorFunction()));
    connect(&m_transErrState, SIGNAL(entered()), this, SLOT(transErrorFunction()));
    connect(&m_rebootBusyState, SIGNAL(entered()), this, SLOT(rebootBusyFunction()));
    connect(&m_finalState, SIGNAL(entered()), this, SLOT(finalFunction()));

    m_stateMachine.addState(&m_initialState);
    m_stateMachine.addState(&m_rebootState);
    m_stateMachine.addState(&m_updateState);
    m_stateMachine.addState(&m_nextSectorState);
    m_stateMachine.addState(&m_finalState);
    m_stateMachine.addState(&m_transErrState);
    m_stateMachine.addState(&m_rebootBusyState);
    m_stateMachine.setInitialState(&m_initialState);
}

void Updater::SetupTransitions()
{
    m_initialState.addTransition(this, SIGNAL(StartReboot()),
                                 m_beforeRebootPing.GetEntrypoint());
    m_initialState.addTransition(this, SIGNAL(StartUpdate()),
                                 m_beforeUpdatePing.GetEntrypoint());

    m_rebootState.addTransition(this, SIGNAL(RebootDone()),
                                m_beforeUpdatePing.GetEntrypoint());
    m_rebootState.addTransition(this, SIGNAL(TransmissionError()), &m_transErrState);

    m_updateState.addTransition(this, SIGNAL(WriteSection()),
                                m_writeSector.GetEntrypoint());
    m_updateState.addTransition(this, SIGNAL(TransmissionError()), &m_transErrState);

    m_nextSectorState.addTransition(this, SIGNAL(AnotherSection()),
                                    m_writeSector.GetEntrypoint());
    m_nextSectorState.addTransition(this, SIGNAL(UpdateDone()), &m_finalState);
}

Updater::Updater(QObject *parent) : QObject(parent),
    m_beforeRebootPing(this, &m_stateMachine,
                       (QState*) &m_rebootBusyState, &m_rebootState),
    m_beforeUpdatePing(this, &m_stateMachine, &m_updateState),
    m_writeSector(this, &m_stateMachine, &m_nextSectorState),
    m_curSection(nullptr)
{
    m_stateMachine.setObjectName("Updater Machine");
    m_stateMachine.setChildMode(QState::ExclusiveStates);

    SetupStates();
    SetupTransitions();
}

void Updater::BeginUpdate(UpdateSettings settings)
{
    int retval;
    emit UpdateProgress(0);
    gLogger.WriteInfo("Preparing for update");
    m_beforeUpdatePing.SetInterface(settings.device);
    m_beforeRebootPing.SetInterface(settings.device);
    m_writeSector.SetInterface(settings.device);

    if (m_stateMachine.isRunning() == true)
    {
        gLogger.WriteWarning("Update In Progress");
        return;
    }

    retval = m_currentFirmware.load(settings.filename);
    if (retval == FIRMWARE_INCORRECT)
    {
        gLogger.WriteError("Firmware is incorrect format.");
        return;
    }
    else if (retval == FIRMWARE_NONEXIST)
    {
        gLogger.WriteError("Could not open firmware; make sure it exists.");
        return;
    }
    else if (retval == FIRMWARE_NOK)
    {
        gLogger.WriteError("Unknown error loading Firmware.");
        return;
    }

    m_currentSettings = settings;
    m_stateMachine.start();
}

void Updater::initialFunction()
{
    gLogger.WriteInfo("Init update process");
    emit UpdateBusy(true);

    if (m_currentSettings.reboot == true)
    {
        emit UpdateProgress(.10);
        emit StartReboot();
        return;
    }

    emit UpdateProgress(.20);
    emit StartUpdate();
    return;
}

void Updater::rebootFunction()
{
    gLogger.WriteInfo("Attempting reboot of 0x" +
                      QString::number(m_currentSettings.targetId,16));



    emit UpdateProgress(.20);
    gLogger.WriteInfo("Reboot succeeded");
    emit RebootDone();
    return;
}

SectionIterator Updater::NextNonEmptySection(SectionIterator curSection)
{
    do {
        ++curSection;
    } while (((*curSection).size() == 0 ||
            (*curSection).addr() < BOTTOMADDR ||
             (*curSection).addr() > TOPADDR) &&
            curSection != m_curSection.end());

    return curSection;
}

void Updater::updateFunction()
{
    gLogger.WriteInfo("Found ECU in programming mode: Starting update");

    m_curSection = m_currentFirmware.sections();
    m_curSection = NextNonEmptySection(m_curSection);
    m_writeSector.SetSection(*m_curSection);

    emit WriteSection();
    return;
}

void Updater::nextSectorFunction()
{
    SectionIterator nextSection = NextNonEmptySection(m_curSection);
    if (nextSection != m_curSection.end())
    {
        m_curSection = nextSection;
        m_writeSector.SetSection(*m_curSection);
        emit AnotherSection();
        return;
    }

    emit UpdateDone();
    return;
}

void Updater::finalFunction()
{
    gLogger.WriteInfo("Rebooting device to normal operation and finishing.");

    SendBootReboot(m_currentSettings.device);

    emit UpdateProgress(1.0);
    emit UpdateBusy(false);
}

void Updater::transErrorFunction()
{
    gLogger.WriteError("Unrecoverable transmission error");

    emit UpdateBusy(false);
}

void Updater::rebootBusyFunction()
{
    gLogger.WriteError("Found ECU in programming mode. Rebooting another \
would cause a clash.  Exiting.");

    emit UpdateBusy(false);
}
