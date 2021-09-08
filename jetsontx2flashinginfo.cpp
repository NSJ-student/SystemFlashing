#include "jetsontx2flashinginfo.h"

JetsonTx2FlashingInfo::JetsonTx2FlashingInfo(QObject *parent) :
    QObject(parent)
{
    //class를 qml에서 사용하기 위해서 등록해주는 부분
    qmlRegisterType<JetsonTx2FlashingInfo>("JetsonTx2FlashingInfo", 1, 0,
                                           "JetsonTx2FlashingInfo");
}

JetsonTx2FlashingInfo::~JetsonTx2FlashingInfo()
{
    clearSettingInfo();
}

void JetsonTx2FlashingInfo::setWindow(QQuickWindow *window)
{
    mMainView = window;
    connect(mMainView, SIGNAL(projectChanged(int)), this, SLOT(projectChanged(int)));
    connect(mMainView, SIGNAL(displayChanged(int)), this, SLOT(displayChanged(int)));
    connect(mMainView, SIGNAL(ipChanged(int)), this, SLOT(ipChanged(int)));
    connect(mMainView, SIGNAL(remoteupgradeChanged(int)), this, SLOT(remoteupgradeChanged(int)));
    connect(mMainView, SIGNAL(dispctrlChanged(int)), this, SLOT(dispctrlChanged(int)));
}

void JetsonTx2FlashingInfo::button_test(QString str)
{
    qDebug() << str;
    m_projectList << "test" << "test2";
    emit projectListChanged();
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
    loadSettingInfo("init.xml");
}

void JetsonTx2FlashingInfo::loadSettingInfo(const QString &path)
{
    QFile settings(path);
    if(!settings.exists())
    {
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

        if((last_project_exists)&&(name == currentStatus.display_out))
        {
            currentStatus.m_display_out = p_disp;
            foreach(Ip * ip_item, p_disp->ip_list)
            {
                m_ipList.append(ip_item->name);
                if((last_project_exists) && (ip_item->name == currentStatus.ip))
                {
                    currentStatus.m_ip = ip_item;
                }
                else if(currentStatus.m_ip == Q_NULLPTR)
                {
                    currentStatus.ip = ip_item->name;
                    currentStatus.m_ip = ip_item;
                }
            }
        }
        else if(index == 0)
        {
            currentStatus.display_out = name;
            currentStatus.m_display_out = p_disp;
            foreach(Ip * ip_item, p_disp->ip_list)
            {
                m_ipList.append(ip_item->name);
                if((last_project_exists) && (ip_item->name == currentStatus.ip))
                {
                    currentStatus.m_ip = ip_item;
                }
                else if(currentStatus.m_ip == Q_NULLPTR)
                {
                    currentStatus.ip = ip_item->name;
                    currentStatus.m_ip = ip_item;
                }
            }
        }
    }
    emit displayListChanged();
    emit ipListChanged();

    // project
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

        if((last_project_exists)&&(name == currentStatus.project))
        {
            currentStatus.m_project = p_project;
        }
        else if(index == 0)
        {
            currentStatus.project = name;
            currentStatus.m_project = p_project;
        }
    }
    emit projectListChanged();

    updateUpgradeAppList();
    updateDispAppList();

    return;
}

void JetsonTx2FlashingInfo::saveSettingInfo()
{
    QDomDocument doc;
    QDomElement xml = doc.createElement("LAST_LOG");
    xml.setAttribute("DATE", QDateTime::currentDateTime().date().toString("yyyy/MM/dd"));
    xml.setAttribute("TIME", QDateTime::currentDateTime().date().toString("HH:mm:ss"));
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

void JetsonTx2FlashingInfo::projectChanged(int project)
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
        currentStatus.project = project;
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
        emit ipListChanged();
    }
}

void JetsonTx2FlashingInfo::displayChanged(int display_out)
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
        currentStatus.display_out = display_out;
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

        updateUpgradeAppList();
        updateDispAppList();
    }
}

void JetsonTx2FlashingInfo::ipChanged(int ip)
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
        currentStatus.ip = ip;
        currentStatus.m_ip = search;
    }
}

void JetsonTx2FlashingInfo::remoteupgradeChanged(int remote_upgrade)
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

void JetsonTx2FlashingInfo::dispctrlChanged(int dispctrl)
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

bool JetsonTx2FlashingInfo::getLastProject()
{
    QFile settings("last_log.xml");
    if(!settings.exists())
    {
        return false;
    }

    QDomDocument doc;
    doc.setContent(&settings);
    settings.close();

    QDomElement top_element = doc.firstChildElement("LAST_LOG");
    if(top_element.isNull())
    {
        return false;
    }

    QString date = top_element.attribute("DATE");
    QString time = top_element.attribute("TIME");

    qDebug() << "last update: " << date << time;

    QDomElement project = top_element.firstChildElement("PROJECT");
    if(!project.isNull())
    {
        return false;
    }
    currentStatus.project = project.attribute("NAME");

    QDomElement disp_out = top_element.firstChildElement("DISPLAY_OUT");
    if(!disp_out.isNull())
    {
        return false;
    }
    currentStatus.display_out = disp_out.attribute("NAME");

    QDomElement ip = top_element.firstChildElement("IP");
    if(!ip.isNull())
    {
        return false;
    }
    currentStatus.ip = ip.attribute("NAME");

    QDomElement remote_upgrade = top_element.firstChildElement("REMOTE_UPGRADE");
    if(!remote_upgrade.isNull())
    {
        return false;
    }
    currentStatus.remote_upgrade = remote_upgrade.attribute("NAME");

    QDomElement disp_ctrl = top_element.firstChildElement("DISP_CTRL");
    if(!disp_ctrl.isNull())
    {
        return false;
    }
    currentStatus.disp_ctrl = disp_ctrl.attribute("NAME");

    return true;
}

void JetsonTx2FlashingInfo::updateUpgradeAppList()
{
    DisplayOut * p_disp = currentStatus.m_display_out;
    if(p_disp == Q_NULLPTR)
    {
        return;
    }

    QString dir_path = p_disp->base_path + "/" + p_disp->rsc_dir;
    QDir dir(dir_path);
    if(!dir.exists())
    {
        return;
    }

    QStringList filters;
    filters << "RemoteUpgrade*";
    QStringList results = dir.entryList(filters, QDir::Files | QDir::NoDot | QDir::NoDotDot);

    m_upgradeAppList.clear();
    foreach(QString file, results)
    {
        m_upgradeAppList.append(file.remove(0, 14));
    }
    if(results.count() > 0)
    {
        currentStatus.remote_upgrade = m_upgradeAppList.at(0);
    }
    emit upgradeAppListChanged();
}

void JetsonTx2FlashingInfo::updateDispAppList()
{
    DisplayOut * p_disp = currentStatus.m_display_out;
    if(p_disp == Q_NULLPTR)
    {
        return;
    }

    QString dir_path = p_disp->base_path + "/" + p_disp->app_dir;
    QDir dir(dir_path);
    if(!dir.exists())
    {
        return;
    }

    QStringList filters;
    filters << p_disp->app_prefix + "*";
    qDebug() << p_disp->app_prefix;
    QStringList results = dir.entryList(filters, QDir::Files | QDir::NoDot | QDir::NoDotDot);

    m_dispAppList.clear();
    foreach(QString file, results)
    {
        m_dispAppList.append(file.remove(0, p_disp->app_prefix.length()-1));
    }
    if(results.count() > 0)
    {
        currentStatus.disp_ctrl = m_dispAppList.at(0);
    }
    emit dispAppListChanged();
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
