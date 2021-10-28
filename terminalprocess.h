#ifndef TERMINALPROCESS_H
#define TERMINALPROCESS_H

#include <QQuickView>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <QTextCodec>

#define USE_THREAD  0

class TerminalWork : public QObject
{
    Q_OBJECT

public:
    TerminalWork(QProcess * p_process, QObject *parent = Q_NULLPTR) :
        QObject(parent)
    {
        qDebug() << QLocale::system().name();
        m_codec = QTextCodec::codecForName("eucKR");
        m_recvProcess = p_process;
        cancel = false;
    }
    ~TerminalWork()
    {
    }

    void stop()
    {
        cancel = true;
    }

signals:
    void recv(QVariant data);
    void finished();

public slots:
    void body()
    {
        qDebug() << "cmd rx waiting...";
        cancel = false;
        try
        {
            while(1)
            {
                if(cancel)
                {
                    break;
                }
                if(m_recvProcess->bytesAvailable() > 0)
                {
                    QByteArray read = m_recvProcess->readAll();
                    QString string = m_codec->toUnicode(read);
                    emit recv(QVariant(string));

                    qDebug() << string;
                }
                QThread::msleep(10);
            }
        }
        catch (...)
        {
            qDebug() << "process recv error";
        }

        emit finished();
    }

private:
    QProcess * m_recvProcess;
    QTextCodec *m_codec;
    bool cancel;
};

class TerminalProcess : public QObject
{
    Q_OBJECT
public:
    TerminalProcess(QObject *parent = Q_NULLPTR);
    ~TerminalProcess();

    void setWindow(QQuickWindow * window);

signals:
    void recv(QVariant data);
    void remove();

public slots:
    void start();
    void write(const QString &command);
    void inputKey(const QString &key);
#if (USE_THREAD==1)
    void finished();
#else
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyStdOut();
    void readyStdErr();
#endif
    void executeCommand(const QString &command);

private:
    QQuickWindow* mMainView;

    QProcess m_process;
#if (USE_THREAD==1)
    QThread m_recvThread;
    TerminalWork * m_recvWork;
#else
    QTextCodec *m_codec;
#endif
    QString m_command;
};

#endif // TERMINALPROCESS_H
