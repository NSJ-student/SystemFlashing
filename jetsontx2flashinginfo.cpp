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
}

void JetsonTx2FlashingInfo::button_test(QString str)
{
    qDebug() << str;
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
    QDomNodeList IpList = top_element.elementsByTagName("DISPLAY_OUT");
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

        if(last_project_exists)
        {
            if(name == currentStatus.ip)
            {
                currentStatus.m_ip = p_ip;
            }
        }
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

        if(last_project_exists)
        {
            if(name == currentStatus.display_out)
            {
                currentStatus.m_display_out = p_disp;
            }
        }
    }

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

        if(last_project_exists)
        {
            if(name == currentStatus.project)
            {
                currentStatus.m_project = p_project;
            }
        }
    }

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

void JetsonTx2FlashingInfo::projectChanged(const QString &project)
{
    Project * search = findObjByName(listProject, project);
    if(search != Q_NULLPTR)
    {
        currentStatus.project = project;
        currentStatus.m_project = search;

        if(currentStatus.display_out != search->display_out->name)
        {
            currentStatus.display_out = search->display_out->name;
            currentStatus.ip = search->display_out->ip_list.at(0)->name;
        }
    }
}

void JetsonTx2FlashingInfo::displayChanged(const QString &display_out)
{
    DisplayOut * search = findObjByName(listDispOut, display_out);
    if(search != Q_NULLPTR)
    {
        currentStatus.display_out = display_out;
        currentStatus.m_display_out = search;
    }
}

void JetsonTx2FlashingInfo::ipChanged(const QString &ip)
{
    Ip * search = findObjByName(listIp, ip);
    if(search != Q_NULLPTR)
    {
        currentStatus.ip = ip;
        currentStatus.m_ip = search;
    }
}

void JetsonTx2FlashingInfo::remoteupgradeChanged(const QString &remote_upgrade)
{
    currentStatus.remote_upgrade = remote_upgrade;
}

void JetsonTx2FlashingInfo::dispctrlChanged(const QString &dispctrl)
{
    currentStatus.disp_ctrl = dispctrl;
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
    currentStatus.clear();
}
