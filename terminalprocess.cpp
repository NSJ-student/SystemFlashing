#include "terminalprocess.h"

#if defined (Q_OS_LINUX)

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<TerminalProcess>("TerminalProcess", 1, 0,
                                     "TerminalProcess");
//    m_process.setReadChannel(QProcess::StandardOutput);
    m_process.setProcessChannelMode(QProcess::MergedChannels);

#if (USE_THREAD==1)
    m_recvWork = new TerminalWork(&m_process);
    m_recvWork->moveToThread(&m_recvThread);

    QObject::connect(&m_recvThread, SIGNAL(started()), m_recvWork, SLOT(body()));
    QObject::connect(m_recvWork, SIGNAL(recv(QVariant)), this, SIGNAL(recv(QVariant)),
                     Qt::QueuedConnection);
    QObject::connect(m_recvWork, SIGNAL(finished()), this, SLOT(finished()));
#else
    m_codec = QTextCodec::codecForName("eucKR");
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(readyStdErr()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyStdOut()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
#endif
}

TerminalProcess::~TerminalProcess()
{
#if (USE_THREAD==1)
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
#endif
}

void TerminalProcess::setWindow(QQuickWindow *window)
{
    mMainView = window;

    QObject::connect(mMainView, SIGNAL(keyPressed(QString)), this, SLOT(inputKey(QString)));
    QObject::connect(this, SIGNAL(recv(QVariant)), mMainView, SLOT(qmlProcessRecv(QVariant)),
                     Qt::DirectConnection);
    QObject::connect(this, SIGNAL(remove()), mMainView, SLOT(qmlRemoveChar()),
                     Qt::DirectConnection);

#if (USE_THREAD==1)
    if(!m_recvThread.isRunning())
    {
        m_recvThread.start();
    }
#endif
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
    qDebug() << key;
    //if(m_process.isWritable())
    {
        if(key == "\r")
        {
            m_command.append("\n");
            emit recv(QVariant("\rprompt >> "));

            m_process.setProgram(m_command.toUtf8());
            m_process.start(QIODevice::ReadWrite);
            m_command.clear();
        }
        else if(key == "\b")
        {
            if(m_command.length() > 0)
            {
                m_command.remove(m_command.length()-1, 1);
                emit remove();
            }
        }
        else
        {
            m_command.append(key);
            emit recv(QVariant(key));
        }
    }
}

#if (USE_THREAD==1)

void TerminalProcess::finished()
{
    qDebug() << "recv thread end: " << m_process.state();
}

#else

void TerminalProcess::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "finished: " << exitStatus;
}

void TerminalProcess::readyStdOut()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdOut";
}

void TerminalProcess::readyStdErr()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdErr";
}

#endif

#else

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<TerminalProcess>("TerminalProcess", 1, 0,
                                     "TerminalProcess");
//    m_process.setReadChannel(QProcess::StandardOutput);
    m_process.setProgram("cmd");
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.start(QIODevice::ReadWrite);

#if (USE_THREAD==1)
    m_recvWork = new TerminalWork(&m_process);
    m_recvWork->moveToThread(&m_recvThread);

    QObject::connect(&m_recvThread, SIGNAL(started()), m_recvWork, SLOT(body()));
    QObject::connect(m_recvWork, SIGNAL(recv(QVariant)), this, SIGNAL(recv(QVariant)),
                     Qt::QueuedConnection);
    QObject::connect(m_recvWork, SIGNAL(finished()), this, SLOT(finished()));
#else
    m_codec = QTextCodec::codecForName("eucKR");
    QObject::connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(readyStdErr()));
    QObject::connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyStdOut()));
    QObject::connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
#endif
}

TerminalProcess::~TerminalProcess()
{
#if (USE_THREAD==1)
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
#endif
}

void TerminalProcess::setWindow(QQuickWindow *window)
{
    mMainView = window;

    QObject::connect(mMainView, SIGNAL(keyPressed(QString)), this, SLOT(inputKey(QString)));
    QObject::connect(this, SIGNAL(recv(QVariant)), mMainView, SLOT(qmlProcessRecv(QVariant)),
                     Qt::DirectConnection);
    QObject::connect(this, SIGNAL(remove()), mMainView, SLOT(qmlRemoveChar()),
                     Qt::DirectConnection);

#if (USE_THREAD==1)
    if(!m_recvThread.isRunning())
    {
        m_recvThread.start();
    }
#endif
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
    qDebug() << key;
    if(m_process.isWritable())
    {
        if(key == "\r")
        {
            m_command.append("\n");
            m_process.write(m_command.toUtf8());
            m_command.clear();
        }
        else if(key == "\b")
        {
            if(m_command.length() > 0)
            {
                m_command.remove(m_command.length()-1, 1);
            }
        }
        else
        {
            m_command.append(key);
        }
    }
}

#if (USE_THREAD==1)

void TerminalProcess::finished()
{
    qDebug() << "recv thread end: " << m_process.state();
}

#else

void TerminalProcess::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "finished: " << exitStatus;
}

void TerminalProcess::readyStdOut()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdOut";
}

void TerminalProcess::readyStdErr()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdErr";
}

#endif

#endif
