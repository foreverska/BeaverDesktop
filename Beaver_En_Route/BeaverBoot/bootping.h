#ifndef BOOTPING_H
#define BOOTPING_H

#include <QObject>
#include <QSharedPointer>
#include <QCanBusDevice>
#include <QState>
#include <QFinalState>
#include <QTimer>

class BootPing : public QObject
{
    Q_OBJECT

    QStateMachine *m_pStateMachine;
    QCanBusDevice *m_pDevice;

    QState m_sendPing;
    QState m_processAck;
    QState *m_pAckDest;
    QState *m_pTimeoutDest;

    QFinalState m_pingTimeout;
    QFinalState m_pingNack;

    QTimer m_timeout;
    int timeouts;

    void SetupStates();
    void SetupTransitions();
    void SetupCanFilter();
public:
    explicit BootPing(QObject *parent = nullptr,
                      QStateMachine *pStateMachine = nullptr,
                      QState *pAckDest = nullptr,
                      QState *pTimeoutDest = nullptr);
    QState *GetEntrypoint();
    void SetInterface(QString interface);

private slots:
    void SendPingFunc();
    void ProcessAckFunc();
    void PingTimeoutFunc();
    void PingNackFunc();
    void TimeoutFunc();

signals:
    void Timedout();
    void Continue();
    void BeaverPingAck();
    void BeaverPingNack();

};

#endif // BOOTPING_H
