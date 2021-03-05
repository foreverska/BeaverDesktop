#include <QDateTime>

#include "logger.h"

#define DATETIME_FORMAT "dd.MM.yyyy hh:mm:ss.zzz"

Logger::Logger(QObject *parent) : QObject(parent)
{

}

QString Logger::log() const
{
    return m_log;
}

void Logger::setLog(QString log)
{
    if (m_log == log)
        return;

    m_log = log;
    emit logChanged(m_log);
}

void Logger::WriteError(QString error)
{
    QString newError = "[ERROR] " +
            QDateTime::currentDateTime().toString(DATETIME_FORMAT) +
            " : ";
    newError += error + "\n";
    setLog(newError + log());
}

void Logger::WriteWarning(QString warning)
{
    QString newWarning = "[WARNING] " +
            QDateTime::currentDateTime().toString(DATETIME_FORMAT) +
            " : ";
    newWarning += warning + "\n";
    setLog( newWarning + log());
}

void Logger::WriteInfo(QString info)
{
    QString newInfo = "[INFO] " +
            QDateTime::currentDateTime().toString(DATETIME_FORMAT) +
            " : ";
    newInfo += info + "\n";
    setLog(newInfo + log());
}

void Logger::ClearLog()
{
    setLog("");
}

Logger gLogger;
