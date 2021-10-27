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
        m_cancel = false;
    }

    void stop()
    {
        m_cancel = true;
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

        m_cancel = false;
        m_connected = false;
        try
        {
            while(1)
            {
                if(m_cancel)
                {
                    break;
                }

                lsusb.start();
                if(lsusb.waitForReadyRead(1000))
                {
                    QByteArray read = lsusb.readAll();
                    QString strRead = QString(read);
                    if(strRead.contains("Cygnal"))
                    {
                        if(!m_connected)
                        {
                            emit connected();
                            m_connected = true;
                        }
                    }
                    else
                    {
                        if(m_connected)
                        {
                            emit disconnected();
                            m_connected = false;
                        }
                    }
                }
                lsusb.kill();
                if(!lsusb.waitForFinished(1000))
                {
                    qDebug() << "lsusb not finished";
                }
                QThread::msleep(500);
            }
        }
        catch (...)
        {
            qDebug() << "thread error";
        }

        emit finished();
    }

private:
    bool m_cancel;
    bool m_connected;
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
