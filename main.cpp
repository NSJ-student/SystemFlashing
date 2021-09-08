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

    JetsonTx2FlashingInfo * tx2_info = new JetsonTx2FlashingInfo();
    TerminalProcess * terminal = new TerminalProcess();
    QQmlApplicationEngine engine;

    QQmlContext *ownContext = engine.rootContext();
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

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
