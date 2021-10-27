#include "usbdetector.h"

UsbDetector::UsbDetector(QObject *parent) : QObject(parent)
{
    m_recvWork = new UsbWatcher();
    m_recvWork->moveToThread(&m_recvThread);

    QObject::connect(&m_recvThread, SIGNAL(started()), m_recvWork, SLOT(body()));
    QObject::connect(m_recvWork, SIGNAL(connected()), this, SIGNAL(connected()));
    QObject::connect(m_recvWork, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

UsbDetector::~UsbDetector()
{
    m_recvWork->stop();

    if(m_recvThread.isRunning())
    {
        m_recvThread.quit();
        m_recvThread.wait(100);
        if(m_recvThread.isRunning())
        {
            m_recvThread.terminate();
        }
    }

    delete m_recvWork;
}

void UsbDetector::setWindow(QQuickWindow *window)
{
    mMainView = window;

    if(!m_recvThread.isRunning())
    {
        m_recvThread.start();
    }

    QObject::connect(this, SIGNAL(disconnected()), mMainView, SLOT(qmlDisconnected()));
    QObject::connect(this, SIGNAL(connected()), mMainView, SLOT(qmlConnected()));
}
