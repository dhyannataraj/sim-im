/***************************************************************************
                          icons.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
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

#include "sax.h"
#include "icons.h"
#include "qzip/qzip.h"

#include <qiconset.h>
#include <qmime.h>
#include <qimage.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qdragobject.h>
#include <qfile.h>
#include <qmime.h>
#include <qapplication.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qmap.h>

#ifdef USE_KDE
#include <kapp.h>
#include <kipc.h>
#include <kiconloader.h>
#endif

const unsigned ICON_COLOR_MASK  = 0x000000FF;

namespace SIM
{

class WrkIconSet : public IconSet
{
public:
    WrkIconSet();
    PictDef *getPict(const QString &name);
    void clear();
protected:
    PictDef *add(const QString &name, const QImage &pict, unsigned flags);
};

class FileIconSet : public IconSet, public SAXParser
{
public:
    FileIconSet(const QString &file);
    ~FileIconSet();
    PictDef *getPict(const QString &name);
    void clear();
protected:
    virtual void element_start(const char *el, const char **attr);
    virtual void element_end(const char *el);
    virtual void char_data(const char *str, int len);
    QString     m_name;
    QString     m_file;
    QString     m_smile;
    QString     *m_data;
#ifdef USE_KDE
    QString     m_system;
#endif
    unsigned    m_flags;
    UnZip      *m_zip;
};

class MyMimeSourceFactory : public QMimeSourceFactory
{
public:
    MyMimeSourceFactory();
    ~MyMimeSourceFactory();
    virtual const QMimeSource* data(const QString &abs_name) const;
};

unsigned Icons::nSmile = 0;

Icons::Icons()
{
    /* This idea came from kapplication.cpp
       I had a similar idea with setting the old defaultFactory in
       the destructor but this won't work :(
       Christian */
    QMimeSourceFactory* oldDefaultFactory = QMimeSourceFactory::takeDefaultFactory();
    QMimeSourceFactory::setDefaultFactory(new MyMimeSourceFactory());
    if (oldDefaultFactory)
        QMimeSourceFactory::addFactory( oldDefaultFactory );
    addIconSet("icons/sim.jisp", true);
    m_defSets.append(new WrkIconSet);
    addIconSet("icons/smiles.jisp", false);
    addIconSet("icons/icqlite.jisp", false);
    addIconSet("icons/additional.jisp", false);
    
	
}

Icons::~Icons()
{
    QValueListIterator<IconSet*> it;
    for (it = m_customSets.begin(); it != m_customSets.end(); ++it)
        delete *it;
    for (it = m_defSets.begin(); it != m_defSets.end(); ++it)
        delete *it;
}

void *Icons::processEvent(Event *e)
{
    if (e->type() == eEventIconChanged){
        QValueListIterator<IconSet*> it;
        for (it = m_customSets.begin(); it != m_customSets.end(); ++it)
            (*it)->clear();
        for (it = m_defSets.begin(); it != m_defSets.end(); ++it)
            (*it)->clear();
    }
    return NULL;
}

void Icons::iconChanged(int)
{
    EventIconChanged e;
    e.process();
}

PictDef *Icons::getPict(const QString &name)
{
    QValueListIterator<IconSet*> it;
    for (it = m_customSets.begin(); it != m_customSets.end(); ++it){
        PictDef *res = (*it)->getPict(name);
        if (res)
            return res;
    }
    for (it = m_defSets.begin(); it != m_defSets.end(); ++it){
        PictDef *res = (*it)->getPict(name);
        if (res)
            return res;
    }
    return NULL;
}

QStringList Icons::getSmile(const QString &name)
{
    QValueListIterator<IconSet*> it;
    for (it = m_customSets.begin(); it != m_customSets.end(); ++it){
        QStringList res = (*it)->getSmile(name);
        if (!res.empty())
            return res;
    }
    return QStringList();
}

QString Icons::getSmileName(const QString &name)
{
    QValueListIterator<IconSet*> it;
    for (it = m_customSets.begin(); it != m_customSets.end(); ++it){
        QString res = (*it)->getSmileName(name);
        if (!res.isEmpty())
            return res;
    }
    return QString::null;
}

void Icons::getSmiles(QStringList &smiles)
{
    QStringList used;
    QValueListIterator<IconSet*> it;
    for (it = m_customSets.begin(); it != m_customSets.end(); ++it)
        (*it)->getSmiles(smiles, used);
}

QString Icons::parseSmiles(const QString &str)
{
    QString s = str;
    QString res;
    while (!s.isEmpty()){
        unsigned start = (unsigned)(-1);
        unsigned size  = 0;
        QString smile;
        QValueListIterator<IconSet*> it;
        for (it = m_customSets.begin(); it != m_customSets.end(); ++it){
            unsigned pos    = ~0U;
            unsigned length = 0;
            QString n_smile;
            (*it)->parseSmiles(s, pos, length, n_smile);
            if (length == 0)
                continue;
            if (pos < start){
                start = pos;
                size  = length;
                smile = n_smile;
            }
        }
        if (size == 0){
            res += quoteString(s);
            break;
        }
        res += s.left(start);
        res += "<img src=\"icon:";
        res += smile;
        res += "\" alt=\"";
        res += quoteString(s.mid(start, size));
        res += "\">";
        s = s.mid(start + size);
    }
    return res;
}

IconSet *Icons::addIconSet(const QString &name, bool bDefault)
{
    FileIconSet *is = new FileIconSet(name);
    if (bDefault){
        m_defSets.prepend(is);
    }else{
        m_customSets.append(is);
    }
    return is;
}

void Icons::removeIconSet(IconSet *is)
{
    QValueListIterator<IconSet*> it;
    it = m_customSets.find( is );
    if(it != m_customSets.end()){
        delete is;
        m_customSets.remove(it);
        return;
    }
    it = m_defSets.find( is );
    if(it != m_defSets.end()){
        delete is;
        m_defSets.remove(it);
        return;
    }
}

static Icons *icons = NULL;

Icons *getIcons()
{
    return icons;
}

void createIcons()
{
    icons = new Icons;
}

void deleteIcons()
{
    delete icons;
}

PictDef *getPict(const QString &name)
{
    return icons->getPict(name);
}

static QPixmap getPixmap(PictDef *d, const char*)
{
    QPixmap res;
    res.convertFromImage(d->image);
    return res;
}

QIconSet Icon(const QString &name)
{
    PictDef *pict = getPict(name);
    if (pict == NULL)
        return QIconSet();
    QIconSet res(getPixmap(pict, name));
    QString bigName = "big." + name;
    pict = getPict(bigName);
    if (pict)
        res.setPixmap(getPixmap(pict, bigName), QIconSet::Large);
    return res;
}

QImage Image(const QString &name)
{
    PictDef *p = getPict(name);
    if (p == NULL)
        return QImage();
    return p->image;
}

QPixmap Pict(const QString &name, const QColor&)
{
    PictDef *p = getPict(name);
    if (p == NULL)
        return QPixmap();
    return getPixmap(p, name);
}

MyMimeSourceFactory::MyMimeSourceFactory()
        : QMimeSourceFactory()
{
}

MyMimeSourceFactory::~MyMimeSourceFactory()
{
}

const QMimeSource *MyMimeSourceFactory::data(const QString &abs_name) const
{
    QString name = abs_name;
    if (name.left(5) == "icon:"){
        name = name.mid(5);
        PictDef *p = getPict(name);
        if (p)
            ((QMimeSourceFactory*)this)->setImage(abs_name, p->image);
    }
    return QMimeSourceFactory::data(abs_name);
}

IconSet::IconSet()
{
}

IconSet::~IconSet()
{
}

void IconSet::parseSmiles(const QString &text, unsigned &start, unsigned &size, QString &name)
{
    QValueListIterator<smileDef> it;
    for (it = m_smiles.begin(); it != m_smiles.end(); ++it){
        QString pat = (*it).smile;
        int n = text.find(pat);
        if (n < 0)
            continue;
        if (((unsigned)n < start) || (((unsigned)n == start) && (pat.length() > size))){
            start = n;
            size  = pat.length();
            name  = (*it).name;
        }
    }
}

void IconSet::getSmiles(QStringList &smiles, QStringList &used)
{
    QString name;
    bool bOK = false;
    QValueListIterator<smileDef> it;
    for (it = m_smiles.begin(); it != m_smiles.end(); ++it){
        if (name != (*it).name){
            if (bOK && !name.isEmpty())
                smiles.push_back(name);
            name = (*it).name;
            bOK = true;
        }
        const QStringList::const_iterator itu = used.find((*it).smile);
        if (itu == used.end()){
            used.append((*it).smile);
        }else{
            bOK = false;
        }
    }
    if (bOK && !name.isEmpty())
        smiles.append(name);
}

QStringList IconSet::getSmile(const QString &name)
{
    QStringList res;
    PIXMAP_MAP::iterator it = m_icons.find(name);
    if (it == m_icons.end())
        return res;
    QValueListIterator<smileDef> its;
    for (its = m_smiles.begin(); its != m_smiles.end(); ++its){
        if ((*its).name != name)
            continue;
        res.append((*its).smile);
    }
    return res;
}

QString IconSet::getSmileName(const QString &name)
{
    PIXMAP_MAP::iterator it = m_icons.find(name);
    if (it == m_icons.end())
        return "";
    QString res = it.data().file;
    int n = res.find('.');
    if (n > 0)
        res = res.left(n);
    return res;
}

WrkIconSet::WrkIconSet()
{
}

static QImage makeOffline(unsigned flags, const QImage &p)
{
    QImage image = p.copy();
    unsigned swapColor = flags & ICON_COLOR_MASK;
    unsigned int *data = (image.depth() > 8) ? (unsigned int *)image.bits() :
                         (unsigned int *)image.colorTable();
    int pixels = (image.depth() > 8) ? image.width()*image.height() :
                 image.numColors();
    int i;
    for (i = 0; i < pixels; i++){
        QColor c(qRed(data[i]), qGreen(data[i]), qBlue(data[i]));
        int a = qAlpha(data[i]);
        int h, s, v;
        c.hsv(&h, &s, &v);
        if (swapColor){
            h = (swapColor * 2 - h) & 0xFF;
            c.setHsv(h, s, v);
        }else{
            c.setHsv(h, 0, v * 3 / 4);
        }
        data[i] = qRgba(c.red(), c.green(), c.blue(), a);
    }
    return image;
}

static QImage makeInactive(const QImage &p)
{
    QImage image = p.copy();
    unsigned int *data = (image.depth() > 8) ? (unsigned int *)image.bits() :
                         (unsigned int *)image.colorTable();
    int pixels = (image.depth() > 8) ? image.width()*image.height() :
                 image.numColors();
    int i;
    for (i = 0; i < pixels; i++){
        QColor c(qRed(data[i]), qGreen(data[i]), qBlue(data[i]));
        int a = qAlpha(data[i]);
        int h, s, v;
        c.hsv(&h, &s, &v);
        c.setHsv(h, s / 8, v);
        data[i] = qRgba(c.red(), c.green(), c.blue(), a);
    }
    return image;
}

static QImage makeInvisible(unsigned flags, const QImage &p)
{
    QImage image = p.copy();
    unsigned swapColor = flags & ICON_COLOR_MASK;
    char shift = (flags >> 8) & 0xFF;
    if (image.depth() != 32)
        image = image.convertDepth(32);
    unsigned int *data = (unsigned int*)image.bits();
    for (int y = 0; y < image.width(); y++){
        int x = image.width() / 2 - (y - image.height() / 2) * 2 / 3 + shift;
        if (x < 0)
            x = 0;
        if (x > image.width())
            x = image.width();
        unsigned int *line = data + y * (image.width()) + x;
        for (; x < image.width(); x++, line++){
            QColor c(qRed(*line), qGreen(*line), qBlue(*line));
            int a = qAlpha(*line);
            int h, s, v;
            c.hsv(&h, &s, &v);
            if (swapColor){
                h = (swapColor * 2 - h) & 0xFF;
                c.setHsv(h, s / 2, v * 3 / 4);
            }else{
                c.setHsv(h, s / 2, v * 3 / 4);
            }
            *line = qRgba(c.red(), c.green(), c.blue(), a);
        }
    }
    return image;
}

static QImage merge(const QImage &p1, const QImage &p2)
{
    QImage img1 = p1.copy();
    if (img1.depth() != 32)
        img1 = img1.convertDepth(32);
    QImage img2(p2);
    if (img2.depth() != 32)
        img2 = img2.convertDepth(32);
    unsigned int *data1 = (unsigned int*)img1.bits();
    unsigned int *data2 = (unsigned int*)img2.bits();
    for (int y1 = 0; y1 < img1.height(); y1++){
        int y2 = y1 - (img1.height() - img2.height()) / 2;
        if ((y2 < 0) || (y2 >= img2.height()))
            continue;
        unsigned int *line1 = data1 + y1 * img1.width();
        unsigned int *line2 = data2 + y2 * img2.width();
        int w = img1.width();
        if (w < img2.width()){
            line2 += (img2.width() - w) / 2;
        }else if (w > img2.width()){
            w = img2.width();
            line1 += (img1.width() - w) / 2;
        }
        for (int i = 0; i < w; i++, line1++, line2++){
            int r1 = qRed(*line1);
            int g1 = qGreen(*line1);
            int b1 = qBlue(*line1);
            int a1 = qAlpha(*line1);
            int r2 = qRed(*line2);
            int g2 = qGreen(*line2);
            int b2 = qBlue(*line2);
            int a2 = qAlpha(*line2);
            r1 = (r2 * a2 + r1 * (255 - a2)) / 255;
            g1 = (g2 * a2 + g1 * (255 - a2)) / 255;
            b1 = (b2 * a2 + b1 * (255 - a2)) / 255;
            if (a2 > a1)
                a1 = a2;
            *line1 = qRgba(r1, g1, b1, a1);
        }
    }
    return img1;
}

PictDef *WrkIconSet::getPict(const QString &name)
{
    if (name.isEmpty())
        return NULL;
    PIXMAP_MAP::iterator it = m_icons.find(name);
    if (it != m_icons.end())
        return &it.data();

    QString n = name;
    if (n == "online"){
        unsigned i;
        PictDef *p = NULL;
        const char *icon_name = NULL;
        for (i = 0; i < getContacts()->nClients(); i++){
            Client *client = getContacts()->getClient(i);
            icon_name = client->protocol()->description()->icon;
            p = SIM::getPict(icon_name);
            if (p)
                break;
        }
        if (p == NULL){
            icon_name = "SIM";
            p = SIM::getPict(icon_name);
        }
        if (p == NULL)
            return NULL;
        return add(name, p->image, p->flags);
    }
    if (n == "offline"){
        PictDef *p = SIM::getPict("online");
        if (p == NULL)
            return NULL;
        return add(name, makeOffline(p->flags, p->image), p->flags);
    }
    if (n == "inactive"){
        PictDef *p = SIM::getPict("online");
        if (p == NULL)
            return NULL;
        return add(name, makeInactive(p->image), p->flags);
    }
    if (n == "invisible"){
        PictDef *p = SIM::getPict("online");
        if (p == NULL)
            return NULL;
        return add(name, makeInvisible(p->flags, p->image), p->flags);
    }
    int pos = n.find('_');
    if (pos > 0){
        PictDef *p = SIM::getPict(n.left(pos));
        QImage res;
        if (p){
            QString s = n.mid(pos + 1);
            if (s == "online"){
                res = p->image;
            }else if (s == "offline"){
                res = makeOffline(p->flags, p->image);
            }else if (s == "invisible"){
                res = makeInvisible(p->flags, p->image);
            }else if (s == "inactive"){
                res = makeInactive(p->image);
            }else{
                PictDef *pp = SIM::getPict(s);
                if (pp)
                    res = merge(p->image, pp->image);
            }
            return add(name, res, p->flags);
        }
    }
    if (!name.startsWith("big."))
        log(L_DEBUG, "Icon %s not found", name.latin1());
    return NULL;
}

void WrkIconSet::clear()
{
    m_icons.clear();
}

PictDef *WrkIconSet::add(const QString &name, const QImage &pict, unsigned flags)
{
    PictDef p;
    p.image = pict;
    p.flags = flags;
    m_icons.insert(PIXMAP_MAP::value_type(name, p));
    return &m_icons.find(name).data();
}

FileIconSet::FileIconSet(const QString &file)
{
    m_zip = new UnZip(app_file(file));
    QByteArray arr;
    m_data = NULL;
    if (m_zip->open() && (m_zip->readFile("icondef.xml", &arr) || m_zip->readFile(QFileInfo(m_zip->name()).baseName(true) + QDir::separator() + "icondef.xml", &arr)))
        parse(arr.data(), arr.size(), false);
}

FileIconSet::~FileIconSet()
{
    delete m_zip;
}

PictDef *FileIconSet::getPict(const QString &name)
{
    if (name.isEmpty())
        return NULL;
    PIXMAP_MAP::iterator it = m_icons.find(name);
    if (it == m_icons.end())
        return NULL;
    if (it.data().image.isNull()){
#ifdef USE_KDE
        if (!it.data().system.isEmpty()){
            QPixmap pict;
             if (!name.startsWith("big.")){
                pict = SmallIconSet(it.data().system).pixmap(QIconSet::Small, QIconSet::Normal);
            }else{
                pict = DesktopIconSet(it.data().system).pixmap(QIconSet::Large, QIconSet::Normal);
            }
            if (!pict.isNull()){
                it.data().image = pict.convertToImage();
                return &(it.data());
            }
        }
#endif
        if (it.data().file.isEmpty())
            return NULL;
        QByteArray arr;
        if (!m_zip->readFile((*it).file, &arr) && !m_zip->readFile(QFileInfo(m_zip->name()).baseName(true) + QDir::separator() + (*it).file, &arr))
            return NULL;
        it.data().image = QImage(arr).convertDepth(32);
    }
    return &(it.data());
}

void FileIconSet::clear()
{
    for (PIXMAP_MAP::iterator it = m_icons.begin(); it != m_icons.end(); ++it){
        it.data().image = QImage();
    }
}

void FileIconSet::element_start(const char *el, const char **args)
{
    if (!strcmp(el, "icon")){
        m_name  = "";
        m_smile = "";
        m_flags = 0;
        m_file  = "";
#ifdef USE_KDE
        m_system = "";
#endif
        if (args){
            for (; *args; ){
                const char *key = *(args++);
                const char *value = *(args++);
                if (!strcmp(key, "name"))
                    m_name = value;
                if (!strcmp(key, "flags"))
                    m_flags = atol(value);
#ifdef USE_KDE
                if (!strcmp(key, "kicon"))
                    m_system = value;
#endif
            }
        }
        if (m_name.isEmpty()){
            m_name = "s_";
            m_name += QString::number(++Icons::nSmile);
        }
        return;
    }
    if (!strcmp(el, "object") && m_file.isEmpty()){
        QString mime;
        if (args){
            for (; *args; ){
                const char *key = *(args++);
                const char *value = *(args++);
                if (!strcmp(key, "mime"))
                    mime = value;
            }
        }
        if (mime.isEmpty())
            return;
        int n = mime.find('/');
        if (n < 0)
            return;
        if (!mime.startsWith("image"))
            return;
        mime = mime.mid(n + 1);
        QStringList l = QImage::inputFormatList();
        for (unsigned i = 0; i < l.count(); i++){
            if (l[i].lower() != mime.lower())
                continue;
            m_data = &m_file;
            return;
        }
        return;
    }
    if (!strcmp(el, "text")){
        m_smile = "";
        m_data = &m_smile;
    }
}

void FileIconSet::element_end(const char *el)
{
    if (!strcmp(el, "icon")){
        PictDef p;
        p.file   = m_file;
        p.flags  = m_flags;
#ifdef USE_KDE
        p.system = m_system;
#endif
        PIXMAP_MAP::iterator it = m_icons.find(m_name);
        if (it == m_icons.end())
            m_icons.insert(PIXMAP_MAP::value_type(m_name, p));
#ifdef USE_KDE
        if (!m_name.startsWith("big.")){
            QString big_name = "big." + m_name;
            p.file   = "";
            p.flags  = m_flags;
            p.system = m_system;
            it = m_icons.find(big_name);
            if (it == m_icons.end())
                m_icons.insert(PIXMAP_MAP::value_type(big_name, p));
        }
#endif
    }
    if (!strcmp(el, "text")){
        if (!m_smile.isEmpty() && !m_name.isEmpty()){
            smileDef s;
            s.name  = m_name;
            s.smile = m_smile;
            m_smiles.push_back(s);
        }
        m_smile = "";
    }
    m_data = NULL;
}

void FileIconSet::char_data(const char *data, int size)
{
    if (m_data)
        *m_data += QString::fromLatin1(data, size);
}

};

#ifndef NO_MOC_INCLUDES
#include "icons.moc"
#endif


