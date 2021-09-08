#include "terminalprocess.h"

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    m_process.setProgram("sh");
    m_process.start();

    m_recvWork = new TerminalWork(&m_process);
    m_recvWork->moveToThread(&m_recvThread);

    QObject::connect(&m_recvThread, SIGNAL(started()), m_recvWork, SLOT(body()));
    QObject::connect(m_recvWork, SIGNAL(recv(QString)), this, SIGNAL(recv(QString)));
    QObject::connect(m_recvWork, SIGNAL(finished()), this, SLOT(finished()));
}

TerminalProcess::~TerminalProcess()
{
    m_process.close();

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

void TerminalProcess::setWindow(QQuickWindow *window)
{

}

void TerminalProcess::write(const QString &command)
{
    if(m_process.isWritable())
    {
        m_process.write(command.toUtf8().data(), command.length());
    }
}

void TerminalProcess::finished()
{
    qDebug() << "recv thread end: " << m_process.state();
}
