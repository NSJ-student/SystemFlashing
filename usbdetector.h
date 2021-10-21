#ifndef USBDETECTOR_H
#define USBDETECTOR_H

#include <QQuickView>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QProcess>

class UsbWatcher : public QObject
{
    Q_OBJECT

public:
    UsbWatcher(QObject *parent = Q_NULLPTR) :
        QObject(parent)
    {
        cancel = false;
    }

    void stop()
    {
        cancel = true;
    }

signals:
    void connected();
    void disconnected();
    void finished();

public slots:
    void body()
    {
        QProcess lsusb;
        lsusb.setProgram("lsusb");
        lsusb.setProcessChannelMode(QProcess::MergedChannels);
        qDebug() << "usb checking...";

        cancel = false;
        try
        {
            while(1)
            {
                if(cancel)
                {
                    break;
                }

                qDebug() << "start";
                lsusb.start();
                if(lsusb.waitForReadyRead(1000))
                {
                    QByteArray read = lsusb.readAll();
                    qDebug() << QString(read);
                }
                qDebug() << "kill";
                lsusb.kill();
                lsusb.waitForFinished(1000);
                QThread::msleep(1000);
            }
        }
        catch (...)
        {
            qDebug() << "thread error";
        }

        emit finished();
    }

private:
    bool cancel;
};

class UsbDetector : public QObject
{
    Q_OBJECT

public:
    UsbDetector(QObject *parent = Q_NULLPTR);
    ~UsbDetector();

    void setWindow(QQuickWindow * window);

signals:
    void connected();
    void disconnected();

private:
    QQuickWindow* mMainView;

    QThread m_recvThread;
    UsbWatcher * m_recvWork;

};

#endif // USBDETECTOR_H
