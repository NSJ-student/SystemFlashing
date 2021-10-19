#ifndef TERMINALPROCESS_H
#define TERMINALPROCESS_H

#include <QQuickView>
#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QThread>

class TerminalWork : public QObject
{
    Q_OBJECT

public:
    TerminalWork(QProcess * p_process, QObject *parent = Q_NULLPTR) :
        QObject(parent)
    {
        m_recvProcess = p_process;
        cancel = false;
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
                    emit recv(QVariant(QString(read)));

//                    qDebug() << QString(read);
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
    void finished();

private:
    QQuickWindow* mMainView;

    QProcess m_process;
    QThread m_recvThread;
    TerminalWork * m_recvWork;

    QString m_command;
};

#endif // TERMINALPROCESS_H
