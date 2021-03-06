/***************************************************************************
                          smiles.cpp  -  description
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

#include <libxml/parser.h>

#include "smiles.h"
#include "icondll.h"
#include "buffer.h"

#include <qfile.h>
#include <qregexp.h>
#include <qpainter.h>
#include <qbitmap.h>

#include <qimage.h>

typedef struct xepRecord
{
    unsigned	index;
    string		smiles;
    string		title;
} xepRecord;

class XepParser
{
public:
    XepParser();
    ~XepParser();
    bool parse(QFile &f);
    list<xepRecord>		m_rec;
    QPixmap				pict(unsigned index);
protected:
    QPixmap				m_image;
    string				*m_data;
    string				m_str;
    Buffer				m_pict;
    bool				m_bRec;
    unsigned			m_width;
    unsigned			m_height;
    unsigned			parseNumber(const char*);
    xmlSAXHandler		m_handler;
	xmlParserCtxtPtr	m_context;
    void		element_start(const char *el, const char **attr);
    void		element_end(const char *el);
    void		char_data(const char *str, int len);
    void		cdata(const char *str, int len);
    static void p_element_start(void *data, const xmlChar *el, const xmlChar **attr);
    static void p_element_end(void *data, const xmlChar *el);
    static void p_char_data(void *data, const xmlChar *str, int len);
    static void p_cdata(void *data, const xmlChar *str, int len);
};

XepParser::XepParser()
{
    memset(&m_handler, 0, sizeof(m_handler));
	m_handler.startElement = p_element_start;
	m_handler.endElement   = p_element_end;
	m_handler.characters   = p_char_data;
	m_handler.cdataBlock   = p_cdata;
	m_context = xmlCreatePushParserCtxt(&m_handler, this, "", 0, "");
    m_data  = NULL;
    m_bRec = false;
    m_width  = 0;
    m_height = 0;
}

XepParser::~XepParser()
{
    xmlFreeParserCtxt(m_context);
}

void XepParser::p_element_start(void *data, const xmlChar *el, const xmlChar **attr)
{
    ((XepParser*)data)->element_start((char*)el, (const char**)attr);
}

void XepParser::p_element_end(void *data, const xmlChar *el)
{
    ((XepParser*)data)->element_end((char*)el);
}

void XepParser::p_char_data(void *data, const xmlChar *str, int len)
{
    ((XepParser*)data)->char_data((char*)str, len);
}

void XepParser::p_cdata(void *data, const xmlChar *str, int len)
{
    ((XepParser*)data)->cdata((char*)str, len);
}

static void replace(char *b, unsigned size, const char *str1, const char *str2)
{
    unsigned str_size = strlen(str1);
    for (unsigned i = 0; i < size - str_size; i++, b++){
        if (*b != *str1)
            continue;
        if (memcmp(b, str1, str_size))
            continue;
        memcpy(b, str2, strlen(str2));
    }
}

bool XepParser::parse(QFile &f)
{
    char buf[4096];
    char XML_START[] = "<smiles>";
	xmlParseChunk(m_context, XML_START, strlen(XML_START), 0);
    unsigned start = 0;
    for (;;){
        char s32[] = "<32bit_Icons>";
        char e32[] = "</32bit_Icons>";
        int size = f.readBlock(&buf[start], sizeof(buf) - start);
        if (size <= 0)
            break;
        size += start;
        replace(buf, size, s32, "<AA");
        replace(buf, size, e32, "</AA");
        if (size == sizeof(buf)){
            start = strlen(e32);
            size -= start;
        }
		if (xmlParseChunk(m_context, buf, size, 0))
            return false;
        if (start)
            memmove(buf, &buf[sizeof(buf) - start], start);
    }
    if ((m_pict.size() == 0) || (m_width == 0) || (m_height == 0))
        return false;
    Buffer pict;
    pict.fromBase64(m_pict);
    if (pict.size() < 28)
        return false;
    QByteArray arr;
    arr.assign(pict.data(28), pict.size() - 28);
    QImage img(arr);
    if ((img.width() == 0) || (img.height() == 0))
        return false;
    m_image.convertFromImage(img);
    return true;
}

void XepParser::element_start(const char *el, const char **attrs)
{
    if (!strcmp(el, "record")){
        m_data = NULL;
        m_bRec = false;
        for (const char **p = attrs; *p;){
            const char *attr = *(p++);
            const char *val  = *(p++);
            if (!strcmp(attr, "ImageIndex")){
                xepRecord r;
                r.index = atol(val);
                m_rec.push_back(r);
                m_bRec = true;
                break;
            }
        }
    }
    if ((!strcmp(el, "Expression") && m_bRec) ||
            (!strcmp(el, "PasteText") && m_bRec) ||
            !strcmp(el, "ImageWidth") ||
            !strcmp(el, "ImageHeight")){
        m_str = "";
        m_data = &m_str;
    }
}

void XepParser::element_end(const char *el)
{
    if (!strcmp(el, "record")){
        m_bRec = false;
        return;
    }
    if (!strcmp(el, "Expression") && m_data){
        m_data = NULL;
        m_rec.back().smiles = m_str;
    }
    if (!strcmp(el, "PasteText") && m_data){
        m_data = NULL;
        m_rec.back().title = m_str;
    }
    if (!strcmp(el, "ImageWidth"))
        m_width = parseNumber(m_str.c_str());
    if (!strcmp(el, "ImageHeight"))
        m_height = parseNumber(m_str.c_str());
}

void XepParser::char_data(const char *str, int len)
{
    if (m_data)
        m_data->append(str, len);
}

void XepParser::cdata(const char *str, int len)
{
    m_pict.pack(str, len);
}

unsigned XepParser::parseNumber(const char *p)
{
    const char *r = strchr(p, '\"');
    if (r){
        r++;
    }else{
        r = p;
    }
    return atol(r);
}

QPixmap XepParser::pict(unsigned n)
{
    int cols = m_image.width() / m_width;
    int row = n / cols;
    int y = row * m_height;
    if (y >= m_image.height())
        return QPixmap();
    int x = (n - row * cols) * m_width;
    QPixmap res(m_width, m_height);
    QPainter p(&res);
    p.drawPixmap(0, 0, m_image, x, y);
    p.end();
    res.setMask(res.createHeuristicMask());
    return res;
}

Smiles::Smiles()
{
}

Smiles::~Smiles()
{
    clear();
}

static string getValue(const char *p)
{
    string res;
    for (; *p; p++){
        if (*p != '\"')
            continue;
        p++;
        break;
    }
    for (; *p; p++){
        if (*p == '\\'){
            if (*(++p) == 0)
                break;
            res += '\\';
            res += *p;
            continue;
        }
        if (*p == '\"')
            break;
        res += *p;
    }
    return res;
}

bool Smiles::load(const QString &file)
{
    clear();
    for (unsigned i = 0;; i++){
        const smile *s = defaultSmiles(i);
        if (s == NULL)
            break;
        SmileDef sd;
        sd.paste = s->paste;
        sd.icon  = NULL;
        m_smiles.push_back(sd);
    }
    QString fname = file;
    QFile f(fname);
    if (!f.open(IO_ReadOnly))
        return false;
    int pdot = fname.findRev(".");
    if ((pdot > 0) && (fname.mid(pdot + 1).lower() == "xep")){
        XepParser p;
        if (!p.parse(f))
            return false;
        for (list<xepRecord>::iterator it = p.m_rec.begin(); it != p.m_rec.end(); ++it){
            xepRecord &r = *it;
            QPixmap pict = p.pict(r.index);
            if (pict.isNull())
                continue;
            SmileDef sd;
            sd.title = getValue(r.title.c_str());
            sd.paste = sd.title;
            string exp   = getValue(r.smiles.c_str());
            for (const char *p = exp.c_str(); *p; p++){
                if (*p == '\\'){
                    if (*(++p) == 0)
                        break;
                    sd.exp += '\\';
                    sd.exp += *p;
                    continue;
                }
                if ((*p == '{') || (*p == '}'))
                    sd.exp += '\\';
                sd.exp += *p;
            }
            QIconSet *is = new QIconSet(pict);
            m_icons.push_back(is);
            sd.icon	 = is;
            unsigned index = (unsigned)(-1);
            for (index = 0;; index++){
                const smile *s = defaultSmiles(index);
                if (s == NULL)
                    break;
#if QT_VERSION < 300
                QString exp = s->exp;
                bool bMatch = false;
                while (!exp.isEmpty()){
                    QString e = getToken(exp, '|', false);
                    QRegExp re(e);
                    int len;
                    if ((re.match(sd.paste.c_str(), 0, &len) == 0) && (len == (int)(sd.paste.length()))){
                        bMatch = true;
                        break;
                    }
                }
                if (bMatch){
                    sd.title = s->title;
                    break;
                }
#else
                QRegExp re(s->exp);
                int len;
                if ((re.match(sd.paste.c_str(), 0, &len) == 0) && ((unsigned)len == sd.paste.length())){
                    sd.title = s->title;
                    break;
                }
#endif
            }
            if (index < 16){
                m_smiles[index] = sd;
            }else{
                m_smiles.push_back(sd);
            }
        }
        return true;
    }
#ifdef WIN32
    fname = fname.replace(QRegExp("\\"), "/");
#endif
    int pos = fname.findRev("/");
    if (pos >= 0){
        fname = fname.left(pos + 1);
    }else{
        fname = "";
    }
    string s;
    QRegExp start("^ *Smiley *= *");
    QRegExp num("^ *, *-[0-9]+ *, *");
    QRegExp nn("[0-9]+");
    QRegExp re("\\[\\]\\|\\(\\)\\{\\}\\.\\?\\*\\+");
    while (getLine(f, s)){
        QString line = QString::fromLocal8Bit(s.c_str());
        if (line[0] == ';')
            continue;
        int size;
        int pos = start.match(line, 0, &size);
        if (pos < 0)
            continue;
        line = line.mid(size);
        getToken(line, '\"');
        QString dll = getToken(line, '\"', false);
        if (dll.isEmpty())
            continue;
        dll = dll.replace(QRegExp("\\\\"), "/");
        pos = num.match(line, 0, &size);
        if (pos < 0)
            continue;
        QString num = line.left(size);
        line = line.mid(size);
        pos = nn.match(num, 0, &size);
        unsigned nIcon = num.mid(pos, size).toUInt();
        getToken(line, '\"');
        QString pattern = getToken(line, '\"', false);
        getToken(line, '\"');
        QString tip = getToken(line, '\"', false);
        QString dllName = fname + dll;
        dllName = dllName.replace(QRegExp("/\\./"), "/");
        string fn;
        fn = dllName.utf8();
        ICONS_MAP::iterator it = icons.find(fn.c_str());
        IconDLL *icon_dll = NULL;
        if (it == icons.end()){
            icon_dll = new IconDLL;
            if (!icon_dll->load(fn.c_str())){
                delete icon_dll;
                icon_dll = NULL;
            }
            icons.insert(ICONS_MAP::value_type(fn.c_str(), icon_dll));
        }else{
            icon_dll = (*it).second;
        }
        if (icon_dll == NULL)
            continue;
        const QIconSet *icon = icon_dll->get(nIcon);
        if (icon == NULL){
            log(L_DEBUG, "Icon empty %u", nIcon);
            continue;
        }
        QString p;
        QString paste;
        unsigned index = (unsigned)(-1);
        while (!pattern.isEmpty()){
            QString pat = getToken(pattern, ' ', false);
            if (index == (unsigned)(-1)){
                for (index = 0; index < 16; index++){
                    const smile *s = defaultSmiles(index);
                    if (pat == s->paste)
                        break;
                }
            }
            if (paste.isEmpty())
                paste = pat;
            QString res;
            while (!pat.isEmpty()){
                int pos = re.match(pat);
                if (pos < 0)
                    break;
                res += pat.left(pos);
                res += "\\";
                res += pat.mid(pos, 1);
                pat = pat.mid(pos + 1);
            }
            res += pat;
            if (!p.isEmpty())
                p += "|";
            p += res;
        }
        if (tip.isEmpty())
            tip = paste;
        SmileDef sd;
        sd.exp     = p.latin1();
        sd.paste   = paste.latin1();
        sd.title   = tip.latin1();
        sd.icon    = icon;
        if (index < 16){
            m_smiles[index] = sd;
        }else{
            m_smiles.push_back(sd);
        }
    }
    return true;
}

void Smiles::clear()
{
    for (ICONS_MAP::iterator it = icons.begin(); it != icons.end(); ++it){
        if ((*it).second)
            delete (*it).second;
    }
    icons.clear();
    m_smiles.clear();
    for (list<QIconSet*>::iterator iti = m_icons.begin(); iti != m_icons.end(); ++iti)
        delete *iti;
    m_icons.clear();
}


