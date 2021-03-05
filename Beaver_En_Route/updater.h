#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QStateMachine>
#include <QFinalState>

#include "firmware.h"
#include "BeaverBoot/bootping.h"
#include "BeaverBoot/bootwrite.h"
#include "BeaverBoot/bootreboot.h"
#include "sectioniterator.h"

typedef struct {
    uint32_t targetId;
    QString device;
    QString filename;
    bool reboot;
    bool dryRun;
    bool fullErase;
} UpdateSettings;

class Updater : public QObject
{
    Q_OBJECT

    QStateMachine m_stateMachine;

    QState m_initialState;
    QState m_rebootState;
    QState m_updateState;
    QState m_nextSectorState;

    QFinalState m_rebootBusyState;
    QFinalState m_transErrState;
    QFinalState m_finalState;

    BootPing m_beforeRebootPing;
    BootPing m_beforeUpdatePing;
    BootWrite m_writeSector;

    UpdateSettings m_currentSettings;

    Firmware m_currentFirmware;

    SectionIterator m_curSection;

    int CheckBootloaderAck();
    int WriteSections();
    void SetupStates();
    void SetupTransitions();
    SectionIterator NextNonEmptySection(SectionIterator curSection);
public:
    explicit Updater(QObject *parent = nullptr);
    void BeginUpdate(UpdateSettings settings);


signals:
    void StartReboot();
    void RebootDone();
    void StartUpdate();
    void WriteSection();
    void AnotherSection();
    void UpdateDone();
    void FinishProcess();
    void TransmissionError();

    void UpdateBusy(bool busy);
    void UpdateProgress(float progress);

private slots:
    void initialFunction();
    void rebootFunction();
    void updateFunction();
    void nextSectorFunction();
    void finalFunction();
    void transErrorFunction();
    void rebootBusyFunction();

};

#endif // UPDATER_H
