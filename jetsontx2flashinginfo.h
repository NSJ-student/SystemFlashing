#ifndef JETSONTX2FLASHINGINFO_H
#define JETSONTX2FLASHINGINFO_H

#include <QQuickView>
#include <QObject>
#include <QDebug>

class JetsonTx2FlashingInfo : public QObject
{
    Q_OBJECT
public:
    JetsonTx2FlashingInfo(QObject *parent = Q_NULLPTR);
    ~JetsonTx2FlashingInfo();

    void setWindow(QQuickWindow * window);
    Q_INVOKABLE void button_test(QString str);

signals:

private:
    QQuickWindow* mMainView;

};

#endif // JETSONTX2FLASHINGINFO_H
