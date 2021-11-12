#include "jetsontx2flashinginfo.h"

JetsonTx2FlashingInfo::JetsonTx2FlashingInfo(QObject *parent) :
    QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<JetsonTx2FlashingInfo>("JetsonTx2FlashingInfo", 1, 0,
                                           "JetsonTx2FlashingInfo");
    m_flashingAppPath = QDir::currentPath();
    qDebug() << m_flashingAppPath;
}

JetsonTx2FlashingInfo::~JetsonTx2FlashingInfo()
{
    mQmlView = Q_NULLPTR;
    clearSettingInfo();
}

void JetsonTx2FlashingInfo::setWindow(QQuickWindow *window)
{
    mQmlView = window;

    QObject::connect(mQmlView, SIGNAL(projectChanged(int)), this, SLOT(onProjectChanged(int)));
    QObject::connect(mQmlView, SIGNAL(displayChanged(int)), this, SLOT(onDisplayChanged(int)));
    QObject::connect(mQmlView, SIGNAL(ipChanged(int)), this, SLOT(onIpChanged(int)));
    QObject::connect(mQmlView, SIGNAL(remoteupgradeChanged(int)), this, SLOT(onRemoteupgradeChanged(int)));
    QObject::connect(mQmlView, SIGNAL(dispctrlChanged(int)), this, SLOT(onDispctrlChanged(int)));

    QObject::connect(mQmlView, SIGNAL(flashImage()), this, SLOT(flashing()));
    QObject::connect(mQmlView, SIGNAL(flashImageNoMake()), this, SLOT(flashingWithoutMakingImage()));
    QObject::connect(mQmlView, SIGNAL(flashDtb()), this, SLOT(flashing_dtb()));

    QObject::connect(mQmlView, SIGNAL(loadConfig(QString)), this, SLOT(loadSettingInfo(QString)));

    QObject::connect(this, SIGNAL(currentDispOut(QVariant)), mQmlView, SLOT(qmlDisplayOut(QVariant)));

    QObject::connect(this, SIGNAL(lastProject(QVariant)), mQmlView, SLOT(qmlProject(QVariant)));
    QObject::connect(this, SIGNAL(lastUpdatedTime(QVariant)), mQmlView, SLOT(qmlLastUpdate(QVariant)));
    QObject::connect(this, SIGNAL(lastDispOut(QVariant)), mQmlView, SLOT(qmlDispOut(QVariant)));
    QObject::connect(this, SIGNAL(lastIp(QVariant)), mQmlView, SLOT(qmlIp(QVariant)));
    QObject::connect(this, SIGNAL(lastDispCtrl(QVariant)), mQmlView, SLOT(qmlDispCtrl(QVariant)));
    QObject::connect(this, SIGNAL(lastRemoteUpgrade(QVariant)), mQmlView, SLOT(qmlRemoteUpgrade(QVariant)));

    QObject::connect(this, SIGNAL(currentIdxProject(QVariant)), mQmlView, SLOT(qmlCurrentProject(QVariant)));
    QObject::connect(this, SIGNAL(currentIdxDispOut(QVariant)), mQmlView, SLOT(qmlCurrentDispOut(QVariant)));
    QObject::connect(this, SIGNAL(currentIdxIp(QVariant)), mQmlView, SLOT(qmlCurrentIp(QVariant)));
    QObject::connect(this, SIGNAL(currentIdxDispCtrl(QVariant)), mQmlView, SLOT(qmlCurrentDispCtrl(QVariant)));
    QObject::connect(this, SIGNAL(currentIdxRemoteUpgrade(QVariant)), mQmlView, SLOT(qmlCurrentRemoteUpgrade(QVariant)));

    QObject::connect(this, SIGNAL(init()), mQmlView, SLOT(qmlInit()));
    QObject::connect(this, SIGNAL(activateControls()), mQmlView, SLOT(qmlActivateControlBox()));
    QObject::connect(this, SIGNAL(deactivateControls()), mQmlView, SLOT(qmlDeactivateControlBox()));
}

const QStringList JetsonTx2FlashingInfo::projectList()
{
    return m_projectList;
}

const QStringList JetsonTx2FlashingInfo::displayList()
{
    return m_displayList;
}

const QStringList JetsonTx2FlashingInfo::ipList()
{
    return m_ipList;
}

const QStringList JetsonTx2FlashingInfo::upgradeAppList()
{
    return m_upgradeAppList;
}

const QStringList JetsonTx2FlashingInfo::dispAppList()
{
    return m_dispAppList;
}

void JetsonTx2FlashingInfo::windowCreated()
{
}

void JetsonTx2FlashingInfo::loadSettingInfo(const QString &path)
{
    QString _path = path;
#if defined (Q_OS_LINUX)
    QString result = _path.replace("file://","");
#else
    QString result = _path.replace("file:///","");
#endif
    QFile settings(result);
    if(!settings.exists())
    {
        qDebug() << "file not exist: " << result;
        return;
    }

    QDomDocument doc;
    doc.setContent(&settings);
    settings.close();

    QDomElement top_element = doc.firstChildElement("PROJECT_DB");
    if(top_element.isNull())
    {
        return;
    }

    // clear previout status
    clearSettingInfo();

    // get last project
    bool last_project_exists = getLastProject();

    QDomNodeList UsbList = top_element.elementsByTagName("USB");
    if(UsbList.count() > 0)
    {
        QDomElement element = UsbList.at(0).toElement();
        QString name = element.attribute("NAME");

        emit detectUsbName(name);
    }
    else
    {
        emit detectUsbName("");
    }

    QDomNodeList SudoList = top_element.elementsByTagName("SUDO");
    if(SudoList.count() > 0)
    {
        QDomElement element = SudoList.at(0).toElement();
        QString name = element.attribute("NAME");

        m_prefixSudo = "echo " + name + " | sudo -S ";
    }
    else
    {
        m_prefixSudo = "";
    }

    // IP
    QDomNodeList IpList = top_element.elementsByTagName("IP_LIST");
    for(int index=0; index<IpList.count(); index++)
    {
        QDomElement element = IpList.at(index).toElement();

        QString name = element.attribute("NAME");
        QString address = element.attribute("ADDRESS");
        Ip * p_ip = new Ip(name, address);
        listIp.append(p_ip);

        QString src_file = element.attribute("SRC_FILE");
        if(!src_file.isNull()) p_ip->src_file = src_file;

        QString dst_path = element.attribute("DST_PATH");
        if(!dst_path.isNull()) p_ip->dst_path = dst_path;
    }

    // display out
    int idx_dispout = -1;
    int idx_ip = -1;
    QDomNodeList DispList = top_element.elementsByTagName("DISPLAY_OUT");
    for(int index=0; index<DispList.count(); index++)
    {
        QDomElement element = DispList.at(index).toElement();

        QString name = element.attribute("NAME");
        QString base_path = element.attribute("BASE_PATH");

        DisplayOut * p_disp = new DisplayOut(name, base_path);
        listDispOut.append(p_disp);
        m_displayList.append(name);

        QString dts = element.attribute("DTS");
        if(!dts.isNull()) p_disp->dts = dts;

        QString app_prefix = element.attribute("APP_PREFIX");
        if(!app_prefix.isNull()) p_disp->app_prefix = app_prefix;

        QString major_prefix = element.attribute("MAJOR_PREFIX");
        if(!major_prefix.isNull()) p_disp->major_prefix = major_prefix;
        else                       p_disp->major_prefix = Q_NULLPTR;

        QString app_dir = element.attribute("APP_DIR");
        if(!app_dir.isNull()) p_disp->app_dir = app_dir;

        QString rsc_dir = element.attribute("RSC_DIR");
        if(!rsc_dir.isNull()) p_disp->rsc_dir = rsc_dir;

        QString dst_path = element.attribute("DST_PATH");
        if(!dst_path.isNull()) p_disp->dst_path = dst_path;

        QDomNodeList IPs = element.elementsByTagName("IP");
        for(int ip=0; ip<IPs.count(); ip++)
        {
            QDomElement ip_eletment = IPs.at(ip).toElement();

            QString ip_name = ip_eletment.attribute("NAME");
            if(ip_name.isNull())
            {
                continue;
            }

            Ip * search = findObjByName(listIp, ip_name);
            if(search != Q_NULLPTR)
            {
                p_disp->ip_list.append(search);
            }
        }

        if(last_project_exists)
        {
            if(name == lastUpdatedStatus.display_out)
            {
                idx_dispout = index;
                currentStatus.display_out = name;
                currentStatus.m_display_out = p_disp;
                for(int temp_idx=0; temp_idx<p_disp->ip_list.count(); temp_idx++)
                {
                    Ip * ip_item = p_disp->ip_list.at(temp_idx);
                    m_ipList.append(ip_item->name);
                    if((last_project_exists) && (ip_item->name == lastUpdatedStatus.ip))
                    {
                        idx_ip = temp_idx;
                        currentStatus.ip = ip_item->name;
                        currentStatus.m_ip = ip_item;
                    }
                }
                if((idx_ip < 0) && (currentStatus.m_display_out->ip_list.count()>0))
                {
                    idx_ip = 0;
                    currentStatus.ip = m_ipList.at(0);
                    currentStatus.m_ip = currentStatus.m_display_out->ip_list.at(0);
                }
            }
        }
    }
    if((idx_dispout < 0) && (DispList.count()>0))
    {
        idx_dispout = 0;
        currentStatus.display_out = m_displayList.at(0);
        currentStatus.m_display_out = listDispOut.at(0);
        for(int temp_idx=0; temp_idx<currentStatus.m_display_out->ip_list.count(); temp_idx++)
        {
            Ip * ip_item = currentStatus.m_display_out->ip_list.at(temp_idx);
            m_ipList.append(ip_item->name);
            if((last_project_exists) && (ip_item->name == lastUpdatedStatus.ip))
            {
                idx_ip = temp_idx;
                currentStatus.ip = ip_item->name;
                currentStatus.m_ip = ip_item;
            }
        }
        if((idx_ip < 0) && (currentStatus.m_display_out->ip_list.count()>0))
        {
            idx_ip = 0;
            currentStatus.ip = m_ipList.at(0);
            currentStatus.m_ip = currentStatus.m_display_out->ip_list.at(0);
        }
    }
    emit displayListChanged();
    emit ipListChanged();

    // project
    int idx_project = -1;
    QDomNodeList NodeList = top_element.elementsByTagName("PROJECT_lIST");
    if(NodeList.count() == 0)
    {
        return;
    }
    QDomElement project_list = NodeList.at(0).toElement();
    QDomNodeList projects = project_list.elementsByTagName("PROJECT");
    for(int index=0; index<projects.count(); index++)
    {
        QDomElement element = projects.at(index).toElement();

        QString name = element.attribute("NAME");
        QString display_out = element.attribute("DISPLAY_OUT");
        DisplayOut * p_disp = findObjByName(listDispOut, display_out);

        Project * p_project = new Project(name, p_disp);
        listProject.append(p_project);
        m_projectList.append(name);

        if(last_project_exists)
        {
            if(name == lastUpdatedStatus.project)
            {
                idx_project = index;
                currentStatus.project = name;
                currentStatus.m_project = p_project;
            }
        }
    }
    if((idx_project < 0) && (projects.count()>0))
    {
        idx_project = 0;
        currentStatus.project = m_projectList.at(0);
        currentStatus.m_project = listProject.at(0);
    }
    emit projectListChanged();
    if(idx_project >= 0)
    {
        emit currentIdxProject(idx_project);
    }

    updateUpgradeAppList(last_project_exists);
    updateDispAppList(last_project_exists);

    if(idx_dispout >= 0)
    {
        emit currentIdxDispOut(idx_dispout);
    }
    if(idx_ip >= 0)
    {
        emit currentIdxIp(idx_ip);
    }

#if defined (Q_OS_WIN)
    emit init();
#endif
    return;
}

void JetsonTx2FlashingInfo::saveLastFlashInfo()
{
    emit lastProject(currentStatus.project);
    emit lastDispOut(currentStatus.display_out);
    emit lastIp(currentStatus.ip);
    emit lastDispCtrl(currentStatus.disp_ctrl);
    emit lastRemoteUpgrade(currentStatus.remote_upgrade);

    QString date = QDateTime::currentDateTime().date().toString("yyyy/MM/dd");
    QString time = QDateTime::currentDateTime().time().toString("HH:mm:ss");

    emit lastUpdatedTime(QVariant("last update: " + date + " " + time));
    emit activateControls();

    QDomDocument doc;
    QDomElement xml = doc.createElement("LAST_LOG");
    xml.setAttribute("DATE", date);
    xml.setAttribute("TIME", time);
    doc.appendChild(xml);

    QDomElement child_element;
    child_element = doc.createElement("PROJECT");
    child_element.setAttribute("NAME", currentStatus.project);
    xml.appendChild(child_element);

    child_element = doc.createElement("DISPLAY_OUT");
    child_element.setAttribute("NAME", currentStatus.display_out);
    xml.appendChild(child_element);

    child_element = doc.createElement("IP");
    child_element.setAttribute("NAME", currentStatus.ip);
    xml.appendChild(child_element);

    child_element = doc.createElement("REMOTE_UPGRADE");
    child_element.setAttribute("NAME", currentStatus.remote_upgrade);
    xml.appendChild(child_element);

    child_element = doc.createElement("DISP_CTRL");
    child_element.setAttribute("NAME", currentStatus.disp_ctrl);
    xml.appendChild(child_element);

    QFile file("last_log.xml");
    bool ret = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(ret)
    {
        QTextStream stream(&file);
        stream << doc.toString();
        file.close();
        doc.clear();
    }
}

void JetsonTx2FlashingInfo::onProjectChanged(int project)
{
    if(project < 0)
    {
        return;
    }
    if(m_projectList.count() <= project)
    {
        return;
    }
    Project * search = findObjByName(listProject, m_projectList.at(project));
    if(search != Q_NULLPTR)
    {
        currentStatus.project = m_projectList.at(project);
        currentStatus.m_project = search;

        if(currentStatus.display_out != search->display_out->name)
        {
            currentStatus.display_out = search->display_out->name;
            currentStatus.ip = search->display_out->ip_list.at(0)->name;
        }

        m_ipList.clear();
        foreach(Ip * ip_item, search->display_out->ip_list)
        {
            m_ipList.append(ip_item->name);
            if(m_ipList.count() == 1)
            {
                currentStatus.ip = ip_item->name;
                currentStatus.m_ip = ip_item;
            }
        }

        int index = m_displayList.indexOf(currentStatus.display_out);
        if(index >= 0)
        {
            emit currentDispOut(QVariant(index));
        }
    }
}

void JetsonTx2FlashingInfo::onDisplayChanged(int display_out)
{
    if(display_out < 0)
    {
        return;
    }
    if(m_displayList.count() <= display_out)
    {
        return;
    }
    DisplayOut * search = findObjByName(listDispOut, m_displayList.at(display_out));
    if(search != Q_NULLPTR)
    {
        currentStatus.display_out = m_displayList.at(display_out);
        currentStatus.m_display_out = search;
        m_ipList.clear();
        foreach(Ip * ip_item, search->ip_list)
        {
            m_ipList.append(ip_item->name);
            if(m_ipList.count() == 1)
            {
                currentStatus.ip = ip_item->name;
                currentStatus.m_ip = ip_item;
            }
        }
        emit ipListChanged();

        updateUpgradeAppList(true);
        updateDispAppList(true);
    }
}

void JetsonTx2FlashingInfo::onIpChanged(int ip)
{
    if(ip < 0)
    {
        return;
    }
    if(m_ipList.count() <= ip)
    {
        return;
    }
    Ip * search = findObjByName(listIp, m_ipList.at(ip));
    if(search != Q_NULLPTR)
    {
        currentStatus.ip = m_ipList.at(ip);
        currentStatus.m_ip = search;
    }
}

void JetsonTx2FlashingInfo::onRemoteupgradeChanged(int remote_upgrade)
{
    if(remote_upgrade < 0)
    {
        return;
    }
    if(m_upgradeAppList.count() <= remote_upgrade)
    {
        return;
    }
    currentStatus.remote_upgrade = m_upgradeAppList.at(remote_upgrade);
}

void JetsonTx2FlashingInfo::onDispctrlChanged(int dispctrl)
{
    if(dispctrl < 0)
    {
        return;
    }
    if(m_dispAppList.count() <= dispctrl)
    {
        return;
    }
    currentStatus.disp_ctrl = m_dispAppList.at(dispctrl);
}

void JetsonTx2FlashingInfo::flashing()
{
    if(currentStatus.m_display_out)
    {
        if(!currentStatus.m_display_out->dts.isEmpty())
        {
            QString dts_src_dir = m_flashingAppPath + "/"+ currentStatus.m_display_out->rsc_dir + "/";
            QString dts_dst_dir = m_flashingAppPath + "/"+ currentStatus.m_display_out->rsc_dir + "/";
            QString dts_cmd = currentStatus.m_display_out->base_path + "/kernel/dtc -I dts -O dtb -o "
                     + dts_dst_dir + "temp.dtb "
                     + dts_src_dir + currentStatus.m_display_out->dts;
            emit executeCommand(m_prefixSudo + dts_cmd);

            QString dtb_dst_dir = currentStatus.m_display_out->base_path + "/kernel/dtb/";
            QString dtb_cmd = "cp " + dts_dst_dir + "temp.dtb"
                     + " " + dtb_dst_dir + "tegra186-quill-p3310-1000-c03-00-base.dtb";
            emit executeCommand(m_prefixSudo + dtb_cmd);
        }

        if(!currentStatus.disp_ctrl.isEmpty())
        {
            QString disp_src_dir = m_flashingAppPath + "/" + currentStatus.m_display_out->app_dir + "/";
            QString disp_dst_dir = currentStatus.m_display_out->dst_path + "/";
            QString disp_cmd = "cp " + disp_src_dir + currentStatus.disp_ctrl
                     + " " + disp_dst_dir + "DISP_CTRL";
            emit executeCommand(m_prefixSudo + disp_cmd);
        }

        if(!currentStatus.remote_upgrade.isEmpty())
        {
            QString upg_src_dir = m_flashingAppPath + "/" + currentStatus.m_display_out->rsc_dir + "/";
            QString upg_dst_dir = currentStatus.m_display_out->dst_path + "/";
            QString upg_cmd = "cp " + upg_src_dir + currentStatus.remote_upgrade
                     + " " + upg_dst_dir + "RemoteUpgrade";
            emit executeCommand(m_prefixSudo + upg_cmd);
        }

        if(currentStatus.m_ip && !currentStatus.m_ip->src_file.isEmpty())
        {
            QString ip_src_dir = m_flashingAppPath + "/" + currentStatus.m_display_out->rsc_dir + "/";
            QString ip_dst_dir = currentStatus.m_display_out->base_path + "/" + currentStatus.m_ip->dst_path + "/";
            QString ip_cmd = "cp " + ip_src_dir + currentStatus.m_ip->src_file
                     + " " + ip_dst_dir + currentStatus.m_ip->src_file;
            emit executeCommand(m_prefixSudo + ip_cmd);
        }

        emit executeCommand("cd " + currentStatus.m_display_out->base_path);
        emit executeCommand(m_prefixSudo + "./flash.sh jetson-tx2 mmcblk0p1");
        emit deactivateControls();
#if defined (Q_OS_WIN)
        saveLastFlashInfo();
#endif
    }
}

void JetsonTx2FlashingInfo::flashingWithoutMakingImage()
{
    emit executeCommand("cd " + currentStatus.m_display_out->base_path);
    emit executeCommand(m_prefixSudo + "./flash.sh -r jetson-tx2 mmcblk0p1");
    emit deactivateControls();
#if defined (Q_OS_WIN)
    saveLastFlashInfo();
#endif
}

void JetsonTx2FlashingInfo::flashing_dtb()
{
    if(currentStatus.m_display_out && !currentStatus.m_display_out->dts.isEmpty())
    {
        QString dts_src_dir = m_flashingAppPath + "/"+ currentStatus.m_display_out->rsc_dir + "/";
        QString dts_dst_dir = m_flashingAppPath + "/"+ currentStatus.m_display_out->rsc_dir + "/";
        QString dts_cmd = currentStatus.m_display_out->base_path + "/kernel/dtc -I dts -O dtb -o "
                 + dts_dst_dir + "temp.dtb "
                 + dts_src_dir + currentStatus.m_display_out->dts;
        emit executeCommand(m_prefixSudo + dts_cmd);

        QString dtb_dst_dir = currentStatus.m_display_out->base_path + "/kernel/dtb/";
        QString dtb_cmd = "cp " + dts_dst_dir + "temp.dtb"
                 + " " + dtb_dst_dir + "tegra186-quill-p3310-1000-c03-00-base.dtb";
        emit executeCommand(m_prefixSudo + dtb_cmd);

        emit executeCommand("cd " + currentStatus.m_display_out->base_path);
        emit executeCommand(m_prefixSudo + "./flash.sh -r -k kernel-dtb jetson-tx2 mmcblk0p1");
        emit deactivateControls();
    }
}

bool JetsonTx2FlashingInfo::getLastProject()
{
    QFile settings("last_log.xml");
    if(!settings.exists())
    {
        qDebug() << "last_log.xml not exist";
        return false;
    }

    QDomDocument doc;
    doc.setContent(&settings);
    settings.close();
    bool result = true;

    QDomElement top_element = doc.firstChildElement("LAST_LOG");
    if(top_element.isNull())
    {
        qDebug() << "last_log.xml (LAST_LOG) not exist";
        result = false;
    }

    QString date = top_element.attribute("DATE");
    QString time = top_element.attribute("TIME");

    emit lastUpdatedTime(QVariant("last update: " + date + " " + time));

    QDomNodeList project = top_element.elementsByTagName("PROJECT");
    if(project.count() == 0)
    {
        qDebug() << "last_log.xml (PROJECT) not exist";
        result = false;
    }
    else
    {
        lastUpdatedStatus.project = project.at(0).toElement().attribute("NAME");
        emit lastProject(lastUpdatedStatus.project);
    }

    QDomNodeList disp_out = top_element.elementsByTagName("DISPLAY_OUT");
    if(disp_out.count() == 0)
    {
        qDebug() << "last_log.xml (DISPLAY_OUT) not exist";
        result = false;
    }
    else
    {
        lastUpdatedStatus.display_out = disp_out.at(0).toElement().attribute("NAME");
        emit lastDispOut(lastUpdatedStatus.display_out);
    }

    QDomNodeList ip = top_element.elementsByTagName("IP");
    if(ip.count() == 0)
    {
        qDebug() << "last_log.xml (IP) not exist";
        result = false;
    }
    else
    {
        lastUpdatedStatus.ip = ip.at(0).toElement().attribute("NAME");
        emit lastIp(lastUpdatedStatus.ip);
    }

    QDomNodeList remote_upgrade = top_element.elementsByTagName("REMOTE_UPGRADE");
    if(remote_upgrade.count() == 0)
    {
        qDebug() << "last_log.xml (REMOTE_UPGRADE) not exist";
        result = false;
    }
    else
    {
        lastUpdatedStatus.remote_upgrade = remote_upgrade.at(0).toElement().attribute("NAME");
        emit lastRemoteUpgrade(lastUpdatedStatus.remote_upgrade);
    }

    QDomNodeList disp_ctrl = top_element.elementsByTagName("DISP_CTRL");
    if(disp_ctrl.count() == 0)
    {
        qDebug() << "last_log.xml (DISP_CTRL) not exist";
        result = false;
    }
    else
    {
        lastUpdatedStatus.disp_ctrl = disp_ctrl.at(0).toElement().attribute("NAME");
        emit lastDispCtrl(lastUpdatedStatus.disp_ctrl);
    }

    return result;
}

void JetsonTx2FlashingInfo::updateUpgradeAppList(bool last_project_exists)
{
    DisplayOut * p_disp = currentStatus.m_display_out;
    if(p_disp == Q_NULLPTR)
    {
        emit upgradeAppListChanged();
        return;
    }

//    QString dir_path = p_disp->base_path + "/" + p_disp->rsc_dir;
    QString dir_path = m_flashingAppPath + "/" + p_disp->rsc_dir;
    QDir dir(dir_path);
    if(!dir.exists())
    {
        qDebug() << dir_path << " not exist";
        emit upgradeAppListChanged();
        return;
    }

    QStringList filters;
    filters << "RemoteUpgrade*";
    QStringList results = dir.entryList(filters, QDir::Files | QDir::NoDot | QDir::NoDotDot);

    m_upgradeAppList.clear();
    int idx_upgrade = -1;
    for(int index=0; index<results.count(); index++)
    {
        QString file = results.at(index);
        m_upgradeAppList.append(file);
        if((last_project_exists) && (file == currentStatus.remote_upgrade))
        {
            idx_upgrade = index;
            currentStatus.remote_upgrade = file;
        }
    }
    if((idx_upgrade < 0) && (m_upgradeAppList.count() > 0))
    {
        idx_upgrade = 0;
        currentStatus.remote_upgrade = m_upgradeAppList.at(0);
    }
    emit upgradeAppListChanged();

    if(idx_upgrade >= 0)
    {
        emit currentIdxRemoteUpgrade(idx_upgrade);
    }
}

void JetsonTx2FlashingInfo::updateDispAppList(bool last_project_exists)
{
    DisplayOut * p_disp = currentStatus.m_display_out;
    if(p_disp == Q_NULLPTR)
    {
        emit dispAppListChanged();
        return;
    }

//    QString dir_path = p_disp->base_path + "/" + p_disp->app_dir;
    QString dir_path = m_flashingAppPath + "/" + p_disp->app_dir;
    QDir dir(dir_path);
    if(!dir.exists())
    {
        qDebug() << dir_path << " not exist";
        emit dispAppListChanged();
        return;
    }

    QStringList filters;
    filters << p_disp->app_prefix + "*";
    QStringList results = dir.entryList(filters, QDir::Files | QDir::NoDot | QDir::NoDotDot);

    m_dispAppList.clear();
    int idx_dispctrl = -1;
    for(int index=0; index<results.count(); index++)
    {
        QString file = results.at(index);
        m_dispAppList.append(file);
        if((last_project_exists) && (file == currentStatus.disp_ctrl))
        {
            idx_dispctrl = index;
            currentStatus.disp_ctrl = file;
        }
    }
    if((idx_dispctrl < 0) && (m_dispAppList.count() > 0))
    {
        idx_dispctrl = 0;
        currentStatus.disp_ctrl = m_dispAppList.at(0);
    }
    emit dispAppListChanged();

    if(idx_dispctrl >= 0)
    {
        emit currentIdxDispCtrl(idx_dispctrl);
    }
}

void JetsonTx2FlashingInfo::clearSettingInfo()
{
    while(listProject.count() > 0)
    {
        delete listProject.takeAt(0);
    }
    while(listDispOut.count() > 0)
    {
        delete listDispOut.takeAt(0);
    }
    while(listIp.count() > 0)
    {
        delete listIp.takeAt(0);
    }

    m_projectList.clear();
    m_displayList.clear();
    m_ipList.clear();
    m_upgradeAppList.clear();
    m_dispAppList.clear();

    currentStatus.clear();
}
