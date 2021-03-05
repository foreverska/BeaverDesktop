#include <QCanBusFrame>
#include <QDataStream>

#include "logger.h"
#include "bootwrite.h"

#define BEAVERBOOT_INIT 0x1F020040
#define BEAVERBOOT_DATA 0x1F020080
#define BEAVERBOOT_ACK  0x1F020100
#define ALLMATCH        0xFFFFFFFF

#define BEAVERBOOT_ACKVAL   0x00
#define BEAVERBOOT_NACKVAL  0x01

void BootWrite::SetupStates()
{
    connect(&m_sendInit, SIGNAL(entered()), this, SLOT(SendInitFunc()));
    connect(&m_initAck, SIGNAL(entered()), this, SLOT(InitAckFunc()));
    connect(&m_sendData, SIGNAL(entered()), this, SLOT(SendDataFunc()));
    connect(&m_dataAck, SIGNAL(entered()), this, SLOT(DataAckFunc()));
    connect(&m_recvNack, SIGNAL(entered()), this, SLOT(RecvNackFunc()));
    connect(&m_recvIssue, SIGNAL(entered()), this, SLOT(RecvIssueFunc()));

    m_pStateMachine->addState(&m_sendInit);
    m_pStateMachine->addState(&m_initAck);
    m_pStateMachine->addState(&m_sendData);
    m_pStateMachine->addState(&m_dataAck);
    m_pStateMachine->addState(&m_recvNack);
    m_pStateMachine->addState(&m_recvIssue);
}

void BootWrite::SetupTransitions()
{
    m_sendInit.addTransition(&m_timeout, SIGNAL(timeout()), &m_recvIssue);

    m_initAck.addTransition(this, SIGNAL(AckRecv()), &m_sendData);
    m_initAck.addTransition(this, SIGNAL(NackRecv()), &m_recvNack);

    m_sendData.addTransition(&m_timeout, SIGNAL(timeout()), &m_recvIssue);

    m_dataAck.addTransition(this, SIGNAL(MoreData()), &m_sendData);
    m_dataAck.addTransition(this, SIGNAL(DoneData()), m_pTarget);
}

BootWrite::BootWrite(QObject *parent,
                     QStateMachine *pStateMachine,
                     QState *pTarget) : QObject(parent)
{
    m_pDevice = nullptr;
    m_pStateMachine = pStateMachine;
    m_pTarget = pTarget;

    SetupStates();
    SetupTransitions();
}


void BootWrite::SetSection(Section newSection)
{
    m_curSection = newSection;
    m_curByte = 0;
}

QState *BootWrite::GetEntrypoint()
{
    return &m_sendInit;
}

void BootWrite::SetupCanFilter()
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

void BootWrite::SetInterface(QString interface)
{
    m_pDevice = QCanBus::instance()->createDevice("socketcan", interface);
    m_pDevice->setConfigurationParameter(QCanBusDevice::BitRateKey,
                                         QVariant());
    SetupCanFilter();
    m_sendInit.addTransition(m_pDevice, SIGNAL(framesReceived()), &m_initAck);
    m_sendData.addTransition(m_pDevice, SIGNAL(framesReceived()), &m_dataAck);
}


void BootWrite::SendInitFunc()
{
    QCanBusFrame frame;
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream.setByteOrder(stream.LittleEndian);

    gLogger.WriteInfo("Starting transfer for: " + m_curSection.name());

    stream << (quint32) m_curSection.addr() << (quint32) m_curSection.size();

    frame.setFrameId(BEAVERBOOT_INIT);
    frame.setPayload(payload);

    m_pDevice->connectDevice();
    m_pDevice->clear();
    m_pDevice->writeFrame(frame);
}

void BootWrite::InitAckFunc()
{
    QVector<QCanBusFrame> frames;

    frames = m_pDevice->readAllFrames();

    for (auto frame : frames)
    {
        if (frame.frameId() == BEAVERBOOT_ACK)
        {
            uint8_t val = frame.payload()[0];
            if (val == BEAVERBOOT_ACKVAL)
            {
                emit AckRecv();
                return;
            }
        }
    }

    emit NackRecv();
}

void BootWrite::SendDataFunc()
{
    QCanBusFrame frame;
    QByteArray payload;

    for (int i = 0; m_curByte < m_curSection.size() && i < 8; m_curByte++, i++)
    {
        payload.append(((quint8*) m_curSection.pData())[m_curByte]);
    }

    frame.setFrameId(BEAVERBOOT_DATA);
    frame.setPayload(payload);

    m_pDevice->writeFrame(frame);
}

void BootWrite::DataAckFunc()
{
    QVector<QCanBusFrame> frames;

    frames = m_pDevice->readAllFrames();

    for (auto frame : frames)
    {
        if (frame.frameId() == BEAVERBOOT_ACK)
        {
            uint8_t val = frame.payload()[0];
            if (val == BEAVERBOOT_ACKVAL)
            {
                if (m_curByte >= m_curSection.size())
                {
                    m_pDevice->disconnectDevice();
                    emit DoneData();
                    return;
                }
                else
                {
                    emit MoreData();
                    return;
                }
            }
        }
    }

    emit NackRecv();
}

void BootWrite::RecvNackFunc()
{
    gLogger.WriteError("ECU refused write command, check firmware file.");
}

void BootWrite::RecvIssueFunc()
{
    gLogger.WriteError("ECU became unresponsive during writing operation.");
}
