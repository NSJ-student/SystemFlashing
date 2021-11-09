#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "jetsontx2flashinginfo.h"
#include "terminalprocess.h"
#include "usbdetector.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // create object
    JetsonTx2FlashingInfo tx2_info;
    TerminalProcess terminal;
#if defined(Q_OS_LINUX)
    UsbDetector usb_detector;
#endif

    // set context property
    QQmlContext *ownContext = engine.rootContext();
    ownContext->setContextProperty("jetson_obj", &tx2_info);
    ownContext->setContextProperty("terminal_obj", &terminal);
    ownContext->setContextProperty("model_project", QVariant::fromValue(tx2_info.projectList()));
    ownContext->setContextProperty("model_display", QVariant::fromValue(tx2_info.displayList()));
    ownContext->setContextProperty("model_ip", QVariant::fromValue(tx2_info.ipList()));
    ownContext->setContextProperty("model_upgradeApp", QVariant::fromValue(tx2_info.upgradeAppList()));
    ownContext->setContextProperty("model_dispApp", QVariant::fromValue(tx2_info.dispAppList()));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    //qrc:/main.qml를 등록한 엔진의 object값을 가져옴
    QObject *root = engine.rootObjects().first();
    //qrc:/main.qml를 등록한 엔진의 object값을 window타입으로 변경해준다.
    QQuickWindow * p_window = qobject_cast<QQuickWindow *>(root);
    p_window->setIcon(QIcon(":/icon/firmware.ico"));

    QObject::connect(&tx2_info, SIGNAL(executeCommand(QString)), &terminal, SLOT(executeCommand(QString)));
    QObject::connect(&terminal, SIGNAL(saveLastFlashInfo()), &tx2_info, SLOT(saveLastFlashInfo()));
#if defined(Q_OS_LINUX)
    QObject::connect(&tx2_info, SIGNAL(detectUsbName(QString)), &usb_detector, SLOT(detectdUsbName(QString)));
    // UsbDetector
    usb_detector.setWindow(p_window);
#endif
    // JetsonTx2FlashingInfo
    tx2_info.setWindow(p_window);
    // TerminalProcess
    terminal.setWindow(p_window);

    return app.exec();
}
