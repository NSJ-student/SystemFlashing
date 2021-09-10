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
    }

signals:
    void recv(const QString &data);
    void finished();

public slots:
    void body()
    {
        try
        {
            while(m_recvProcess->isReadable())
            {
                QByteArray read = m_recvProcess->readAll();
                emit recv(QString(read));

                QThread::msleep(1);
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
};

class TerminalProcess : public QObject
{
    Q_OBJECT
public:
    TerminalProcess(QObject *parent = Q_NULLPTR);
    ~TerminalProcess();

    void setWindow(QQuickWindow * window);

signals:
    void recv(const QString &data);

public slots:
    void write(const QString &command);
    void inputKey(const QString &key);
    void finished();

private:
    QQuickWindow* mMainView;

    QProcess m_process;
    QThread m_recvThread;
    TerminalWork * m_recvWork;
};

#endif // TERMINALPROCESS_H
