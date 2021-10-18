#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include "jetsontx2flashinginfo.h"
#include "terminalprocess.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // create object
    JetsonTx2FlashingInfo * tx2_info = new JetsonTx2FlashingInfo();
    TerminalProcess * terminal = new TerminalProcess();

    // set context property
    QQmlContext *ownContext = engine.rootContext();
    ownContext->setContextProperty("jetson_obj", tx2_info);
    ownContext->setContextProperty("terminal_obj", terminal);
    ownContext->setContextProperty("model_project", QVariant::fromValue(tx2_info->projectList()));
    ownContext->setContextProperty("model_display", QVariant::fromValue(tx2_info->displayList()));
    ownContext->setContextProperty("model_ip", QVariant::fromValue(tx2_info->ipList()));
    ownContext->setContextProperty("model_upgradeApp", QVariant::fromValue(tx2_info->upgradeAppList()));
    ownContext->setContextProperty("model_dispApp", QVariant::fromValue(tx2_info->dispAppList()));

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    //qrc:/main.qml를 등록한 엔진의 object값을 가져옴
    QObject *root = engine.rootObjects()[0];
    //qrc:/main.qml를 등록한 엔진의 object값을 window타입으로 변경해준다.
    tx2_info->setWindow(qobject_cast<QQuickWindow *>(root));
    terminal->setWindow(qobject_cast<QQuickWindow *>(root));

    QQuickWindow * p_window = qobject_cast<QQuickWindow *>(root);
    // TerminalProcess
    QObject::connect(p_window, SIGNAL(keyPressed(QString)), terminal, SLOT(inputKey(QString)));
    QObject::connect(p_window, SIGNAL(run()), terminal, SLOT(start()));
    QObject::connect(terminal, SIGNAL(recv(QVariant)), p_window, SLOT(qmlProcessRecv(QVariant)));
    // JetsonTx2FlashingInfo
    QObject::connect(p_window, SIGNAL(projectChanged(int)), tx2_info, SLOT(projectChanged(int)));
    QObject::connect(p_window, SIGNAL(displayChanged(int)), tx2_info, SLOT(displayChanged(int)));
    QObject::connect(p_window, SIGNAL(ipChanged(int)), tx2_info, SLOT(ipChanged(int)));
    QObject::connect(p_window, SIGNAL(remoteupgradeChanged(int)), tx2_info, SLOT(remoteupgradeChanged(int)));
    QObject::connect(p_window, SIGNAL(dispctrlChanged(int)), tx2_info, SLOT(dispctrlChanged(int)));
    QObject::connect(tx2_info, SIGNAL(setDisplayOut(QVariant)), p_window, SLOT(onQmlDisplayOut(QVariant)));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
