#include <QUrl>
#include <QCanBus>

#include "gui.h"
#include "logger.h"

void Gui::EnumerateCanDevices()
{
    QString err;
    QStringList deviceNames;
    for (auto device : QCanBus::instance()->availableDevices("socketcan",&err))
    {
        deviceNames.append(device.name());
    }

    setCanDevices(deviceNames);
}

Gui::Gui(QObject *parent) : QObject(parent), m_updater(this)
{
    m_progress = 0.0;
    m_running = false;

    EnumerateCanDevices();

    connect(&m_updater, SIGNAL(UpdateProgress(float)),
            this, SLOT(ProgressUpdate(float)));
    connect(&m_updater, SIGNAL(UpdateBusy(bool)),
            this, SLOT(BusyUpdate(bool)));
}

bool Gui::dryRun() const
{
    return m_dryRun;
}

bool Gui::reboot() const
{
    return m_reboot;
}

bool Gui::fullErase() const
{
    return m_fullErase;
}

QString Gui::filename() const
{
    return m_filename;
}

QString Gui::target() const
{
    return m_target;
}

float Gui::progress() const
{
    return m_progress;
}

bool Gui::running() const
{
    return m_running;
}

bool Gui::CheckSettings(uint32_t canId)
{
    if (reboot() == true && canId == 0)
    {
        gLogger.WriteError("Must Specify Target for Reboot");
        return false;
    }

    if (m_filename == "")
    {
        gLogger.WriteError("Must Specify Firmware File to Update");
        return false;
    }

    return true;
}

uint32_t Gui::GetCanId()
{
    bool ok;
    uint32_t canId;
    QString target = m_target;

    canId = target.toInt(&ok, 10);
    if (ok == true)
    {
        return canId;
    }

    canId = target.toInt(&ok, 16);
    if (ok == true)
    {
        return canId;
    }

    return 0;
}

void Gui::beginUpdate()
{
    uint32_t canId;

    gLogger.ClearLog();

    QUrl url(m_filename);
    m_filename = url.toLocalFile();

    canId = GetCanId();
    if (CheckSettings(canId) == false)
    {
        return;
    }

    UpdateSettings settings = {canId, m_device, m_filename, reboot(),
                               dryRun(), fullErase()};

    m_updater.BeginUpdate(settings);
}

QString Gui::device() const
{
    return m_device;
}

QStringList Gui::canDevices() const
{
    return m_canDevices;
}

void Gui::setDryRun(bool dryRun)
{
    if (m_dryRun == dryRun)
        return;

    m_dryRun = dryRun;
    emit dryRunChanged(m_dryRun);
}

void Gui::setReboot(bool reboot)
{
    if (m_reboot == reboot)
        return;

    m_reboot = reboot;
    emit rebootChanged(m_reboot);
}

void Gui::setFullErase(bool fullErase)
{
    if (m_fullErase == fullErase)
        return;

    m_fullErase = fullErase;
    emit fullEraseChanged(m_fullErase);
}

void Gui::setFilename(QString filename)
{
    if (m_filename == filename)
        return;

    m_filename = filename;
    emit filenameChanged(m_filename);
}

void Gui::setTarget(QString target)
{
    if (m_target == target)
        return;

    m_target = target;
    emit targetChanged(m_target);
}

void Gui::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(m_running);
}

void Gui::ProgressUpdate(float progress)
{
    setProgress(progress);
}

void Gui::BusyUpdate(bool busy)
{
    setRunning(busy);
}

void Gui::setDevice(QString device)
{
    if (m_device == device)
        return;

    m_device = device;
    emit deviceChanged(m_device);
}

void Gui::setCanDevices(QStringList canDevices)
{
    if (m_canDevices == canDevices)
        return;

    m_canDevices = canDevices;
    emit canDevicesChanged(m_canDevices);
}

void Gui::setProgress(float progress)
{
    if (qFuzzyCompare(m_progress, progress))
        return;

    m_progress = progress;
    emit progressChanged(m_progress);
}
