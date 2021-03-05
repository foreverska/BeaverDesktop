#ifndef BOOTWRITE_H
#define BOOTWRITE_H

#include <QObject>
#include <QStateMachine>
#include <QFinalState>
#include <QTimer>
#include <QCanBus>

#include "section.h"

class BootWrite : public QObject
{
    Q_OBJECT

    Section m_curSection;
    uint32_t m_curByte;

    QStateMachine *m_pStateMachine;
    QCanBusDevice *m_pDevice;

    QState m_sendInit;
    QState m_initAck;
    QState m_sendData;
    QState m_dataAck;
    QState *m_pTarget;

    QFinalState m_recvNack;
    QFinalState m_recvIssue;

    QTimer m_timeout;

    void SetupStates();
    void SetupTransitions();

private slots:
    void SendInitFunc();
    void InitAckFunc();
    void SendDataFunc();
    void DataAckFunc();

    void RecvNackFunc();
    void RecvIssueFunc();

public:
    explicit BootWrite(QObject *parent,
                       QStateMachine *pStateMachine,
                       QState *pTarget);

    void SetSection(Section newSection);
    QState *GetEntrypoint();
    void SetInterface(QString interface);
    void SetupCanFilter();
signals:
    void RecvTimeout();
    void AckRecv();
    void NackRecv();
    void MoreData();
    void DoneData();

};

#endif // BOOTWRITE_H
