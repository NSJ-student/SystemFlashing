#ifndef JETSONTX2FLASHINGINFO_H
#define JETSONTX2FLASHINGINFO_H

#include <QQuickView>
#include <QObject>
#include <QDebug>
#include <QtXml>
#include <QList>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>

class Ip
{
public:
    Ip(QString _name, QString _address)
    {
        name = _name;
        address = _address;
        src_file = "interface_192";
        dst_path = "rootfs/etc/network/interfaces";
    }

    QString name;
    QString address;
    QString src_file;
    QString dst_path;
};

class DisplayOut
{
public:
    DisplayOut(QString _name, QString _base_path)
    {
        name = _name;
        if(_base_path.isEmpty())
        {
            base_path = ".";
        }
        else
        {
            base_path = _base_path;
        }
        app_prefix = "DISP_2CH_CTRL";
        app_dir = "work_obj/DSI";
        rsc_dir = "work_obj";
        dst_path = "rootfs/home/mik21/Desktop";
    }

    QString name;
    QString dts;
    QString base_path;
    QString app_prefix;
    QString major_prefix;
    QString app_dir;
    QString rsc_dir;
    QString dst_path;
    QList<Ip*> ip_list;
};

class Project
{
public:
    Project(QString _name, DisplayOut * p_disp = Q_NULLPTR)
    {
        name = _name;
        display_out = p_disp;
    }

    QString name;
    DisplayOut * display_out;
};

class UserStatus
{
public:
    UserStatus()
    {
    }

    QString project;
    QString display_out;
    QString ip;
    QString remote_upgrade;
    QString disp_ctrl;

    Project * m_project;
    DisplayOut * m_display_out;
    Ip * m_ip;

    void clear()
    {
        project = "";
        display_out = "";
        ip = "";
        remote_upgrade = "";
        disp_ctrl = "";

        m_project = Q_NULLPTR;
        m_display_out = Q_NULLPTR;
        m_ip = Q_NULLPTR;
    }
};

class JetsonTx2FlashingInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList projectList READ projectList NOTIFY projectListChanged)
    Q_PROPERTY(QStringList displayList READ displayList NOTIFY displayListChanged)
    Q_PROPERTY(QStringList ipList READ ipList NOTIFY ipListChanged)
    Q_PROPERTY(QStringList upgradeAppList READ upgradeAppList NOTIFY upgradeAppListChanged)
    Q_PROPERTY(QStringList dispAppList READ dispAppList NOTIFY dispAppListChanged)

public:
    JetsonTx2FlashingInfo(QObject *parent = Q_NULLPTR);
    ~JetsonTx2FlashingInfo();

    void setWindow(QQuickWindow * window);

    const QStringList projectList();
    const QStringList displayList();
    const QStringList ipList();
    const QStringList upgradeAppList();
    const QStringList dispAppList();

signals:
    void projectListChanged();
    void displayListChanged();
    void ipListChanged();
    void upgradeAppListChanged();
    void dispAppListChanged();
    void currentDispOut(QVariant data);

    void currentIdxProject(QVariant data);
    void currentIdxDispOut(QVariant data);
    void currentIdxIp(QVariant data);
    void currentIdxDispCtrl(QVariant data);
    void currentIdxRemoteUpgrade(QVariant data);

    void lastProject(QVariant data);
    void lastUpdatedTime(QVariant data);
    void lastDispOut(QVariant data);
    void lastIp(QVariant data);
    void lastDispCtrl(QVariant data);
    void lastRemoteUpgrade(QVariant data);

    void executeCommand(const QString &command);
    void detectUsbName(const QString &name);
    void init();

    void activateControls();
    void deactivateControls();

public slots:
    void windowCreated();

    void loadSettingInfo(const QString &path);
    void saveLastFlashInfo();

    void onProjectChanged(int project);
    void onDisplayChanged(int display_out);
    void onIpChanged(int ip);
    void onRemoteupgradeChanged(int remote_upgrade);
    void onDispctrlChanged(int dispctrl);

    void flashing();
    void flashingWithoutMakingImage();
    void flashing_dtb();

private:
    QQuickWindow* mQmlView;

    QString m_flashingAppPath;

    QList<Project*> listProject;
    QList<DisplayOut*> listDispOut;
    QList<Ip*> listIp;

    UserStatus currentStatus;
    UserStatus lastUpdatedStatus;

    QStringList m_projectList;
    QStringList m_displayList;
    QStringList m_ipList;
    QStringList m_upgradeAppList;
    QStringList m_dispAppList;

    QString m_prefixSudo;

    bool getLastProject();
    void updateUpgradeAppList(bool last_project_exists);
    void updateDispAppList(bool last_project_exists);
    void clearSettingInfo();

    template <typename T>
    T * findObjByName(const QList<T *> &list, const QString &name)
    {
        for(int index=0; index<list.count(); index++)
        {
            if(list.at(index)->name == name)
            {
                return list.at(index);
            }
        }

        return Q_NULLPTR;
    }

    template <typename T>
    int findIndexByName(const QList<T *> &list, const QString &name)
    {
        for(int index=0; index<list.count(); index++)
        {
            if(list.at(index)->name == name)
            {
                return index;
            }
        }

        return -1;
    }
};

#endif // JETSONTX2FLASHINGINFO_H
