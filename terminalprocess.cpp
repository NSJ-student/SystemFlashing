#include "terminalprocess.h"
#if defined (Q_OS_LINUX)
#include <signal.h>
#endif

//#if defined (Q_OS_LINUX)
#if (USE_BASH==0)

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    print_prompt = false;
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
    qDebug() << QLocale::system().uiLanguages();
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
//    qDebug() << key;
    if(key == "\r")
    {
        emit recv(QVariant("\r"));

        QStringList args = m_command.split(" ");
        if(args.count() > 0)
        {
            QString command = args.at(0);
            args.removeAt(0);

            m_process.setProgram(command.toUtf8());
            if(args.count() > 0)
            {
                m_process.setArguments(args);
            }
            m_process.start(QIODevice::ReadWrite);
            if(!m_process.waitForStarted(1000))
            {
                emit recv(QVariant("prompt >> "));
            }
        }
        else
        {
            emit recv(QVariant("prompt >> "));
        }

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

#if (USE_THREAD==1)

void TerminalProcess::finished()
{
    qDebug() << "recv thread end: " << m_process.state();
}

#else

void TerminalProcess::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "finished: " << exitStatus;
    emit recv(QVariant("prompt >> "));
}

void TerminalProcess::readyStdOut()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdOut" << string;
}

void TerminalProcess::readyStdErr()
{
    QByteArray read = m_process.readAll();
    QString string = m_codec->toUnicode(read);
    emit recv(QVariant(string));

    qDebug() << "readyStdErr" << string;
}

#endif

void TerminalProcess::executeCommand(const QString &command)
{
    qDebug() << command;
}

#else

TerminalProcess::TerminalProcess(QObject *parent) : QObject(parent)
{
    print_prompt = false;
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<TerminalProcess>("TerminalProcess", 1, 0,
                                     "TerminalProcess");
//    m_process.setReadChannel(QProcess::StandardOutput);
#if defined (Q_OS_LINUX)
    m_process.setProgram("bash");
#else
    m_process.setProgram("cmd");
#endif
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
    m_codec = QTextCodec::codecForName("EUC-KR");
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
    m_process.close();
    m_process.kill();
    m_process.waitForFinished(1000);
}

void TerminalProcess::setWindow(QQuickWindow *window)
{
    mMainView = window;

    QObject::connect(mMainView, SIGNAL(usePrompt(bool)), this, SLOT(use_prompt(bool)));
    QObject::connect(mMainView, SIGNAL(keyPressed(QString)), this, SLOT(inputKey(QString)));
    QObject::connect(this, SIGNAL(recv(QVariant)), mMainView, SLOT(qmlProcessRecv(QVariant)),
                     Qt::DirectConnection);
    QObject::connect(this, SIGNAL(remove()), mMainView, SLOT(qmlRemoveChar()),
                     Qt::DirectConnection);
    QObject::connect(this, SIGNAL(removeProgressLine()), mMainView, SLOT(qmlRemoveProgressLine()),
                     Qt::DirectConnection);

    QObject::connect(this, SIGNAL(activateControls()), mMainView, SLOT(qmlActivateControlBox()));
    QObject::connect(this, SIGNAL(deactivateControls()), mMainView, SLOT(qmlDeactivateControlBox()));
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
    if(m_process.isWritable())
    {
        if(key == "\r")
        {
            if(print_prompt && m_command.isEmpty())
            {
                emit recv(QVariant("\rprompt >> "));
            }
            else
            {
                m_command.append("\n");
                m_process.write(m_command.toUtf8());
                m_command.clear();
                emit recv(QVariant(key));
            }
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
#if defined (Q_OS_LINUX)
    if(exitStatus == QProcess::CrashExit)
    {
        QString errorMsg = "";

        if(exitCode == SIGINT)
        {
            errorMsg = "[CTRL] + [c] (SIGINT)\n";
        }
        else if(exitCode == SIGKILL)
        {
            errorMsg = "Process killed (SIGKILL)\n";
        }
        else if(exitCode == SIGSEGV)
        {
            errorMsg = "Segmentation fault (SIGSEGV)\n";
        }
        else if(exitCode == SIGTERM)
        {
            errorMsg = "Process killed (SIGTERM)\n";
        }
        else if(exitCode == SIGTSTP)
        {
            errorMsg = "[CTRL] + [z] (SIGTSTP)\n";
        }

        qDebug() << errorMsg;
    }
#endif
}

void TerminalProcess::readyStdOut()
{
    while(m_process.bytesAvailable())
    {
        QByteArray read = m_process.readLine();
        QString string = QString::fromLocal8Bit(read);//m_codec->toUnicode(read);
        if(!string.contains("\r") && !string.contains("\n"))
        {
            m_outString += string;
            continue;
        }
        else
        {
            if((string.right(1) != "\r") && (string.right(1) != "\n") && (string.right(2) != "\r\n"))
            {
                m_outString += string;
                continue;
            }
            if(!m_outString.isEmpty())
            {
                m_outString += string;
                string = m_outString;
                m_outString.clear();
            }
        }

//        qDebug() << string;
        if(!string.contains("100%\r") && string.contains("%\r"))
        {
            int percent_count = string.count("%\r");
            int line_count = string.count("\r");
            if(percent_count == line_count)
            {
                emit removeProgressLine();
                if(percent_count > 1)
                {
                    string = string.left(69);
                }
            }
        }

        emit recv(QVariant(string));

        if(string.contains("Writing partition APP with system.img") ||
                string.contains("Sending blob"))
        {
            emit recv(QVariant("[          ] [                                                ] 000%\r"));
        }
        if(string.contains("flashed successfully"))
        {
            emit saveLastFlashInfo();
        }
        if(string.contains("Failed") || string.contains("failed") || string.contains("Error"))
        {
            emit activateControls();
        }
    }

    if(print_prompt)
    {
        emit recv(QVariant("prompt >> "));
    }
}

void TerminalProcess::readyStdErr()
{
    while(m_process.bytesAvailable())
    {
        QByteArray read = m_process.readLine();
        QString string = QString::fromLocal8Bit(read);//m_codec->toUnicode(read);
        if(!string.contains("\r") && !string.contains("\n"))
        {
            m_errString += string;
            continue;
        }
        else
        {
            if((string.right(1) != "\r") && (string.right(1) != "\n") && (string.right(2) != "\r\n"))
            {
                m_outString += string;
                continue;
            }
            if(!m_errString.isEmpty())
            {
                m_errString += string;
                string = m_errString;
                m_errString.clear();
            }
        }

//        qDebug() << string;
        if(!string.contains("100%\r") && string.contains("%\r"))
        {
            int percent_count = string.count("%\r");
            int line_count = string.count("\r");
            if(percent_count == line_count)
            {
                emit removeProgressLine();
                if(percent_count > 1)
                {
                    string = string.left(69);
                }
            }
        }

        emit recv(QVariant(string));

        if(string.contains("Writing partition APP with system.img") ||
                string.contains("Sending blob"))
        {
            emit recv(QVariant("[          ] [                                                ] 000%\r"));
        }
        if(string.contains("flashed successfully"))
        {
            emit saveLastFlashInfo();
        }
        if(string.contains("Failed") || string.contains("failed") || string.contains("Error"))
        {
            emit activateControls();
        }
    }

    if(print_prompt)
    {
        emit recv(QVariant("prompt >> "));
    }
}

#endif

void TerminalProcess::executeCommand(const QString &command)
{
    qDebug() << command;
#if defined (Q_OS_LINUX)
    QString cmd = command + "\n";
    m_process.write(cmd.toUtf8());
#endif
}

void TerminalProcess::use_prompt(bool check)
{
    print_prompt = check;
}

#endif
