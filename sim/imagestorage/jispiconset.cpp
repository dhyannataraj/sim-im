#include "jispiconset.h"
#include "log.h"
#include "misc.h"

#include <QtXml>

namespace SIM {

JispIconSet::JispIconSet()
{
}

bool JispIconSet::load(const QString& filename)
{
    //printf("JispIconSet::load(%s)\n", qPrintable(filename));
    m_id = filename;
    m_uz.setName(filename);
    QByteArray arr;
    if(m_uz.open() &&
       (m_uz.readFile("icondef.xml", &arr) ||
        m_uz.readFile(QFileInfo(m_uz.name()).baseName() + "/icondef.xml", &arr)))
        return parse(arr);
    else
        return false;
}

bool JispIconSet::parse(const QByteArray& arr)
{
    QDomDocument doc;
    doc.setContent(arr);
    QDomElement icondef = doc.firstChildElement("icondef");
    QDomElement meta = icondef.firstChildElement("meta");
    m_name = meta.firstChildElement("name").text();
    QDomElement icon = icondef.firstChildElement("icon");
    while(!icon.isNull()) {
        QString name = icon.attribute("name");
        QDomElement object = icon.firstChildElement("object");
        QString pictfile = object.text();

        if(name.isEmpty())
            name = pictfile.toLower().section('.', 0, -2); // Remove trailing extension

        QDomNodeList texts = icon.elementsByTagName("text");
        QString txtSmile = texts.at(0).toElement().text().trimmed();
        if(txtSmile.isEmpty())
        {
            m_images.insert(name, pictfile);
        }
        else
        {
            name = name.prepend(m_id);
            m_images.insert(name, pictfile);
            if(!isTextIconAdded(txtSmile))
                m_smileKeys << txtSmile;

            for(int i = 0; i < texts.count(); i++) {
                m_smiles.insert(texts.at(i).toElement().text().trimmed(), name);
            }
        }
        icon = icon.nextSiblingElement("icon");
    }
    return true;
}

bool JispIconSet::isTextIconAdded(const QString& iconId)
{
    for (int i=0;i<textSmiles().count();++i)
        if (textSmiles().at(i)==iconId)
            return true;
    return false;
}

QString JispIconSet::id() const
{
    return m_id;
}

QString JispIconSet::name() const
{
    return m_name;
}

bool JispIconSet::hasIcon(const QString& iconId)
{
    //printf("JispIconSet::hasIcon(%s)\n", qPrintable(iconId));
    return m_images.contains(iconId);
}

bool JispIconSet::hasSmile(const QString& txtSmile)
{
    //printf("JispIconSet::hasIcon(%s)\n", qPrintable(iconId));
    return m_smiles.contains(txtSmile);
}

QStringList JispIconSet::textSmiles()
{
    //log(L_DEBUG, QStringList(m_smiles.keys()).join(""));
    //log(L_DEBUG, m_smileKeys.join(" "));
    //return m_smiles.keys();
    return m_smileKeys;

}

QIcon JispIconSet::icon(const QString& iconId)
{
    return QIcon(pixmap(iconId));
}

QImage JispIconSet::image(const QString& iconId)
{
    QByteArray arr;
    if (!m_uz.readFile(m_images.value(iconId), &arr) && !m_uz.readFile(QFileInfo(m_uz.name()).baseName() + '/' + m_images.value(iconId), &arr))
    {
        printf("no pixmap: %s/%s\n", qPrintable(iconId), qPrintable(m_images.value(iconId)));
        return QImage();
    }
    QImage img;
    img.loadFromData(arr);
    return img;
}

QPixmap JispIconSet::pixmap(const QString& iconId)
{
    //printf("JispIconSet::pixmap()\n");
    QByteArray arr;
    if (!m_uz.readFile(m_images.value(iconId), &arr) && !m_uz.readFile(QFileInfo(m_uz.name()).baseName() + '/' + m_images.value(iconId), &arr))
    {
        printf("no pixmap: %s/%s\n", qPrintable(iconId), qPrintable(m_images.value(iconId)));
        return QPixmap();
    }
    QPixmap p;
    p.loadFromData(arr);
    return p;
}

QString JispIconSet::parseSmiles(const QString& input)
{
    QString result = input;
    for(QMap<QString, QString>::iterator it = m_smiles.begin(); it != m_smiles.end(); ++it)
    {
        if(result.contains(it.key()))
        {
            result.replace(it.key(), QString("<img src=\"sim:icons/%1\" />").arg(it.value()));
        }
    }
    return result;
}

QString JispIconSet::parseAllSmilesByName(const QString& name)
{
    QString result = name.toLower();
    log(L_DEBUG, name);
    for(QMap<QString, QString>::iterator it = m_images.begin(); it != m_images.end(); ++it)
    {
        
        if(result.contains(it.key().toLower()))
        {
            result.replace(it.key(), QString("<img src=\"sim:icons/%1\" />").arg(it.value()));
            log(L_DEBUG, result);
        }
    }
    return result;
}

QString JispIconSet::getSmileName(const QString& iconId)
{
    return m_smiles[iconId];
}

QString JispIconSet::getSmileNamePretty(const QString& iconId, bool localized)
{
    int i=getSmileName(iconId).toInt();
    QString str= getSmileName(iconId);
    if (i!=0)
    {
        if (localized)
            return i18n(iconId);
        return iconId;
    }
    else
    {
        if (getSmileName(iconId).length()<2)
        {
            if (localized)
                return i18n(iconId);
            return iconId;
        }
        if (localized)
            return i18n(str = str.left(1).toUpper()+str.mid(1));
        return str = str.left(1).toUpper()+str.mid(1);
    }
}

} // namespace SIM
