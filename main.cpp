#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "jetsontx2flashinginfo.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    JetsonTx2FlashingInfo * tx2_info = new JetsonTx2FlashingInfo();
    QQmlApplicationEngine engine;
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

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
