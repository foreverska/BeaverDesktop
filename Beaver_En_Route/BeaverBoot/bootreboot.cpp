#include <QString>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QVariant>

#define BEAVERBOOT_REBOOT 0x1F0200C0

void SendBootReboot(QString interface)
{
    QCanBusDevice *pDevice;
    QCanBusFrame frame;
    QByteArray payload;

    pDevice = QCanBus::instance()->createDevice("socketcan", interface);
    pDevice->setConfigurationParameter(QCanBusDevice::BitRateKey,
                                         QVariant());

    pDevice->connectDevice();
    frame.setFrameId(BEAVERBOOT_REBOOT);
    frame.setPayload(payload);
    pDevice->writeFrame(frame);
    pDevice->waitForFramesWritten(1000);
    pDevice->disconnectDevice();
}
