/***************************************************************************
                          cfg.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cfg.h"

#include <QFile>
#include "log.h"
#include "misc.h"

namespace SIM
{

Config::Config(const QString& filename) : m_filename(filename),
	m_roothub(PropertyHub::create()), m_changed(false)
{
}

Config::~Config()
{
}

PropertyHubPtr Config::rootHub()
{
    return m_roothub;
}

QByteArray Config::serialize()
{
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""));

    QDomElement roothubelement = doc.createElement("roothub");
    if(m_roothub->serialize(roothubelement))
        doc.appendChild(roothubelement);

    return doc.toByteArray();
}

bool Config::deserialize(const QByteArray& arr)
{
    QDomDocument doc;
    if(!doc.setContent(arr))
        return false;

    QDomElement propertyhub = doc.firstChildElement("roothub");
    if (propertyhub.isNull())
        return false;

    if(!m_roothub->deserialize(propertyhub))
        return false;

    return true;
}

bool Config::mergeOldConfig(const QString& filename)
{
    QFile f(filename);
    if( !f.open(QIODevice::ReadOnly) )
        return false;

    log(L_DEBUG, "Merging old config: %s", qPrintable(filename));
    QString config = QString(f.readAll());
    QRegExp re("\\[([^\\]]+)\\]\n([^\\[]+)");

    int pos = 0;
    while((pos = re.indexIn(config, pos)) != -1)
    {
        pos += re.matchedLength();
        QString ns = re.cap(1);
        QStringList lines = re.cap(2).split('\n');
        for(QStringList::iterator it = lines.begin(); it != lines.end(); ++it)
        {
            QStringList line = it->split('=');
            if(line.size() != 2)
                continue;
            PropertyHubPtr hub = rootHub()->propertyHub(ns);
            if(hub.isNull())
            {
                hub = PropertyHub::create(ns);
                rootHub()->addPropertyHub(hub);
            }

            // Merge if only there's no setting in a new config:
            if(!hub->value(line[0]).isValid())
            {
                if(line[1].startsWith('"') && line[1].endsWith('"'))
                    hub->setValue(line[0], line[1].mid(1, line[1].length() - 2));
                else
                    hub->setValue(line[0], line[1]);
            }
        }
    }
    f.close();
    return true;
}

bool Config::writeToFile()
{
    QFile configFile(m_filename);
    if (!configFile.open(QIODevice::WriteOnly))
    {
        log(L_WARN, "Cannot open file: %s", qPrintable(m_filename));
        return false;
    }
    if (configFile.write(serialize()) < 0)
    {
        log(L_ERROR, "Unable to write file: %s", qPrintable(m_filename));
        return false;
    }
    configFile.close();
    return true;
}

bool Config::readFromFile()
{
    QFile configFile(m_filename);
    if (!configFile.open(QIODevice::ReadOnly))
    {
        log(L_WARN, "Cannot open file: %s", qPrintable(m_filename));
        return false;
    }
    if (!deserialize(configFile.readAll()))
    {
        log(L_WARN, "Unable to deserialize: %s", qPrintable(m_filename));
        return false;
    }
    return true;
}

EXPORT QString getToken(QString &from, char c, bool bUnEscape)
{
    QString res;
    int i;
    for (i = 0; i < from.length(); i++){
        if (from[i] == c)
            break;
        if (from[i] == '\\'){
            i++;
            if (i >= from.length())
                break;
            if (!bUnEscape)
                res += '\\';
        }
        res += from[i];
    }
    if (i < from.length()){
        from = from.mid(i + 1);
    }else{
        from.clear();
    }
    return res;
}

EXPORT QByteArray getToken(QByteArray &from, char c, bool bUnEscape)
{
    QByteArray res;
    int i;
    for (i = 0; i < from.length(); i++){
        if (from[i] == c)
            break;
        if (from[i] == '\\'){
            i++;
            if (i >= from.length())
                break;
            if (!bUnEscape)
                res += '\\';
        }
        res += from[i];
    }
    if (i < from.length()){
        from = from.mid(i + 1);
    }else{
        from.clear();
    }
    return res;
}


}   // namespace SIM
