#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged)
    QString m_log;

public:
    explicit Logger(QObject *parent = nullptr);
    QString log() const;
    void WriteError(QString error);
    void WriteWarning(QString warning);
    void WriteInfo(QString info);
    void ClearLog();
public slots:
    void setLog(QString log);

signals:

    void logChanged(QString log);
};

extern Logger gLogger;

#endif // LOGGER_H
