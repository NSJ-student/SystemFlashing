#include "jetsontx2flashinginfo.h"

JetsonTx2FlashingInfo::JetsonTx2FlashingInfo(QObject *parent) :
    QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<JetsonTx2FlashingInfo>("JetsonTx2FlashingInfo", 1, 0,
                                           "JetsonTx2FlashingInfo");
}

JetsonTx2FlashingInfo::~JetsonTx2FlashingInfo()
{

}

void JetsonTx2FlashingInfo::setWindow(QQuickWindow *window)
{
    mMainView = window;
}

void JetsonTx2FlashingInfo::button_test(QString str)
{
    qDebug() << str;
}
