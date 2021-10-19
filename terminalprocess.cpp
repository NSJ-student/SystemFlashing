#include "terminalprocess.h"

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<TerminalProcess>("TerminalProcess", 1, 0,
                                     "TerminalProcess");
//    m_process.setReadChannel(QProcess::StandardOutput);
#if defined(Q_OS_LINUX)
    m_process.setProgram("bash");
#else
    m_process.setProgram("cmd");
#endif
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.start();

    m_recvWork = new TerminalWork(&m_process);
    m_recvWork->moveToThread(&m_recvThread);

    QObject::connect(&m_recvThread, SIGNAL(started()), m_recvWork, SLOT(body()));
    QObject::connect(m_recvWork, SIGNAL(recv(QVariant)), this, SIGNAL(recv(QVariant)),
                     Qt::QueuedConnection);
    QObject::connect(m_recvWork, SIGNAL(finished()), this, SLOT(finished()));
}

TerminalProcess::~TerminalProcess()
{
    m_process.close();
    m_process.kill();
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

void TerminalProcess::setWindow(QQuickWindow *window)
{
    mMainView = window;

    QObject::connect(mMainView, SIGNAL(keyPressed(QString)), this, SLOT(inputKey(QString)));
    QObject::connect(this, SIGNAL(recv(QVariant)), mMainView, SLOT(qmlProcessRecv(QVariant)),
                     Qt::DirectConnection);
    QObject::connect(this, SIGNAL(remove()), mMainView, SLOT(qmlRemoveChar()),
                     Qt::DirectConnection);

    if(!m_recvThread.isRunning())
    {
        m_recvThread.start();
    }
}

void TerminalProcess::start()
{
    qDebug() << "d start";
}

void TerminalProcess::write(const QString &command)
{
    if(m_process.isWritable())
    {
        m_process.write(command.toUtf8().data(), command.length());
    }
}

void TerminalProcess::inputKey(const QString &key)
{
//    qDebug() << key;
    if(m_process.isWritable())
    {
        if(key == "\r")
        {
            m_command.append("\n");
            m_process.write(m_command.toUtf8());
#if defined(Q_OS_LINUX)
            emit recv(QVariant("\rprompt >> "));
#endif
            m_command.clear();
        }
        else if(key == "\b")
        {
            if(m_command.length() > 0)
            {
                m_command.remove(m_command.length()-1, 1);
#if defined(Q_OS_LINUX)
                emit remove();
#endif
            }
        }
        else
        {
            m_command.append(key);
//            m_process.write(key.toUtf8().data());
#if defined(Q_OS_LINUX)
            emit recv(QVariant(key));
#endif
        }
    }
}

void TerminalProcess::finished()
{
    qDebug() << "recv thread end: " << m_process.state();
}
