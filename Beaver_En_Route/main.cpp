#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "gui.h"
#include "logger.h"

QSharedPointer<Gui> pGuiBackend;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    app.setOrganizationName("Big Honkus");
    app.setOrganizationDomain("bighonkus.com");

    QQmlApplicationEngine engine;
    pGuiBackend = QSharedPointer<Gui>(new Gui(&engine));
    Logger *pLogger = &gLogger;
    engine.rootContext()->setContextProperty("guiBackend", pGuiBackend.data());
    engine.rootContext()->setContextProperty("logger", pLogger);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
