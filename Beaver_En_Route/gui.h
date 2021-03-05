#ifndef GUI_H
#define GUI_H

#include <QObject>

#include "updater.h"

class Gui : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool dryRun READ dryRun WRITE setDryRun NOTIFY dryRunChanged)
    Q_PROPERTY(bool reboot READ reboot WRITE setReboot NOTIFY rebootChanged)
    Q_PROPERTY(bool fullErase READ fullErase WRITE setFullErase NOTIFY fullEraseChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QStringList canDevices READ canDevices WRITE setCanDevices NOTIFY canDevicesChanged)
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)

    bool m_dryRun;
    bool m_reboot;
    bool m_fullErase;
    QString m_filename;
    QString m_target;
    float m_progress;
    bool m_running;
    QString m_statusText;

    Updater m_updater;

    void CheckSettings();
    bool CheckSettings(uint32_t canId);
    uint32_t GetCanId();
    QStringList m_canDevices;

    void EnumerateCanDevices();
    QString m_device;

public:
    explicit Gui(QObject *parent = nullptr); 
    bool dryRun() const;
    bool reboot() const;
    bool fullErase() const;
    QString filename() const;
    QString target() const;
    float progress() const;
    bool running() const;
    QStringList canDevices() const;
    QString device() const;

    Q_INVOKABLE void beginUpdate();

public slots:
    void setDryRun(bool dryRun);
    void setReboot(bool reboot);
    void setFullErase(bool fullErase);
    void setFilename(QString filename);
    void setTarget(QString target);
    void setProgress(float progress);
    void setRunning(bool running);
    void setCanDevices(QStringList canDevices);
    void setDevice(QString device);

    void ProgressUpdate(float progress);
    void BusyUpdate(bool busy);

signals:

    void dryRunChanged(bool dryRun);
    void rebootChanged(bool reboot);
    void fullEraseChanged(bool fullErase);
    void filenameChanged(QString filename);
    void targetChanged(QString target);
    void progressChanged(float progress);
    void runningChanged(bool running);
    void canDevicesChanged(QStringList canDevices);
    void deviceChanged(QString device);
};

#endif // GUI_H
