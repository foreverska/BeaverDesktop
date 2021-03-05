#include <QCanBus>
#include <QCanBusFrame>

#include "logger.h"
#include "bootping.h"
#include "updater.h"

#define BEAVERBOOT_PING 0x1F020000
#define BEAVERBOOT_ACK  0x1F020100
#define ALLMATCH        0xFFFFFFFF

#define BEAVERBOOT_ACKVAL   0x00
#define BEAVERBOOT_NACKVAL  0x01

void BootPing::SetupStates()
{
    connect(&m_sendPing, SIGNAL(entered()), this, SLOT(SendPingFunc()));
    connect(&m_processAck, SIGNAL(entered()), this, SLOT(ProcessAckFunc()));
    connect(&m_pingTimeout, SIGNAL(entered()), this, SLOT(PingTimeoutFunc()));
    connect(&m_pingNack, SIGNAL(entered()), this, SLOT(PingNackFunc()));

    m_pStateMachine->addState(&m_sendPing);
    m_pStateMachine->addState(&m_processAck);
    m_pStateMachine->addState(&m_pingTimeout);
    m_pStateMachine->addState(&m_pingNack);
}

void BootPing::SetupTransitions()
{
    if (m_pTimeoutDest == nullptr)
    {
        m_sendPing.addTransition(this, SIGNAL(Timedout()), &m_pingTimeout);
    }
    else
    {
        m_sendPing.addTransition(this, SIGNAL(Timedout()), m_pTimeoutDest);
    }

    m_processAck.addTransition(this, SIGNAL(BeaverPingNack()), &m_pingNack);
    m_processAck.addTransition(this, SIGNAL(BeaverPingAck()), m_pAckDest);
}

void BootPing::SetupCanFilter()
{
    QCanBusDevice::Filter filter;
    QList<QCanBusDevice::Filter> filterList;

    filter.frameId = BEAVERBOOT_ACK;
    filter.frameIdMask = 0x0;
    filter.format = QCanBusDevice::Filter::MatchExtendedFormat;
    filter.type = QCanBusFrame::InvalidFrame;
    filterList.append(filter);

    m_pDevice->setConfigurationParameter(QCanBusDevice::RawFilterKey,
                                         QVariant::fromValue(filterList));
}

BootPing::BootPing(QObject *parent,
                   QStateMachine *pStateMachine,
                   QState *pAckDest,
                   QState *pTimeoutDest) : QObject(parent)
{

    m_pStateMachine = pStateMachine;
    m_pAckDest = pAckDest;
    m_pTimeoutDest = pTimeoutDest;

    SetupStates();
    SetupTransitions();
}

QState *BootPing::GetEntrypoint()
{
    return &m_sendPing;
}

void BootPing::SetInterface(QString interface)
{
    m_pDevice = QCanBus::instance()->createDevice("socketcan", interface);
    m_pDevice->setConfigurationParameter(QCanBusDevice::BitRateKey,
                                         QVariant());
    SetupCanFilter();
    m_sendPing.addTransition(m_pDevice, SIGNAL(framesReceived()), &m_processAck);
}

void BootPing::SendPingFunc()
{
    QCanBusFrame frame;
    QByteArray empty;

    frame.setFrameId(BEAVERBOOT_PING);
    frame.setFrameType(QCanBusFrame::UnknownFrame);
    frame.setPayload(empty);

    gLogger.WriteInfo("Bootloader ping sent");

    m_pDevice->connectDevice();
    m_pDevice->writeFrame(frame);
}

void BootPing::ProcessAckFunc()
{
    QVector<QCanBusFrame> frames;

    frames = m_pDevice->readAllFrames();

    for (auto frame : frames)
    {
        if (frame.frameId() == BEAVERBOOT_ACK &&
            frame.payload().at(0) == BEAVERBOOT_ACKVAL)
        {
            gLogger.WriteInfo("Bootloader ACK recieved");
            emit BeaverPingAck();
            m_pDevice->disconnectDevice();
            return;
        }
    }

    emit BeaverPingNack();
    m_pDevice->disconnectDevice();
}

void BootPing::PingTimeoutFunc()
{
    gLogger.WriteError("Timeout waiting on Ping from bootloader");
}

void BootPing::PingNackFunc()
{
    gLogger.WriteError("Bootloader NACKed ping request");
}

void BootPing::TimeoutFunc()
{
    emit Timedout();
}
