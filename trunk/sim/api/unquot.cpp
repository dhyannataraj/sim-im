/***************************************************************************
                          unquote.cpp  -  description
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

#include "html.h"
#include "icons.h"

#include <qregexp.h>

class UnquoteParser : public HTMLParser
{
public:
    UnquoteParser();
    QString parse(const QString &str);
protected:
    virtual void text(const QString &text);
    virtual void tag_start(const QString &tag, const list<QString> &options);
    virtual void tag_end(const QString &tag);
    QString res;
    bool m_bPar;
    bool m_bTD;
    bool m_bTR;
    bool m_bPre;
};

UnquoteParser::UnquoteParser()
{
}

QString UnquoteParser::parse(const QString &str)
{
    res = "";
    m_bPar = false;
    m_bTD  = false;
    m_bTR  = false;
    m_bPre = true;
    HTMLParser::parse(str);
    return res;
}

void UnquoteParser::text(const QString &text)
{
    int len = text.length();
    if (len)
        m_bPre = false;
    for (int i = 0; i < len; i++){
        QChar c = text[i];
        if (c.unicode() == 160){
            res += " ";
        }else{
            res += c;
        }
    }
}

void UnquoteParser::tag_start(const QString &tag, const list<QString> &options)
{
    if (tag == "pre"){
        if (!m_bPre)
            res += "\n";
    }else if (tag == "br"){
        res += "\n";
    }else if (tag == "hr"){
        if (!res.isEmpty() && (res[(int)(res.length() - 1)] != '\n'))
            res += "\n";
        res += "---------------------------------------------------\n";
    }else if (tag == "td"){
        if (m_bTD){
            res += "\t";
            m_bTD = false;
        }
    }else if (tag == "tr"){
        if (m_bTR){
            res += "\n";
            m_bTR = false;
        }
    }else if (tag == "p"){
        if (m_bPar){
            res += "\n";
            m_bPar = false;
        }
    }else if (tag == "img"){
        QString src;
        QString alt;
        for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
            QString opt   = *it;
            ++it;
            QString value = *it;
            if (opt == "src")
                src = value;
            if (opt == "alt")
                alt = value;
        }
        if (!alt.isEmpty()){
            res += unquoteString(alt);
            return;
        }
        list<string> smiles = getIcons()->getSmile(src.latin1());
        if (!smiles.empty())
            res += QString::fromUtf8(smiles.front().c_str());
    }
}

void UnquoteParser::tag_end(const QString &tag)
{
    if (tag == "pre"){
        res += "\n";
        m_bPre = true;
    }
    if (tag == "p")
        m_bPar = true;
    if (tag == "td"){
        m_bPar = false;
        m_bTD  = true;
    }
    if (tag == "tr"){
        m_bPar = false;
        m_bTD  = false;
        m_bTR  = true;
    }
    if (tag == "table"){
        m_bPar = true;
        m_bTD  = false;
        m_bTR  = false;
    }
}

QString SIM::unquoteText(const QString &text)
{
    UnquoteParser p;
    return p.parse(text);
}

QString SIM::unquoteString(const QString &text)
{
    QString res = text;
    res = res.replace(QRegExp("&gt;"), ">");
    res = res.replace(QRegExp("&lt;"), "<");
    res = res.replace(QRegExp("&quot;"), "\"");
    res = res.replace(QRegExp("&amp;"), "&");
    res = res.replace(QRegExp("&nbsp;"), " ");
    res = res.replace(QRegExp("<br/?>"), "\n");
    return res;
}

EXPORT QString SIM::quoteString(const QString &_str, quoteMode mode)
{
    QString str = _str;
    str.replace(QRegExp("&"), "&amp;");
    str.replace(QRegExp("<"), "&lt;");
    str.replace(QRegExp(">"), "&gt;");
    str.replace(QRegExp("\""), "&quot;");
    str.replace(QRegExp("\r"), "");
    str.replace(QRegExp("\t"), "&nbsp;&nbsp;");
    switch (mode){
    case quoteHTML:
        str.replace(QRegExp("\n"), "<br>\n");
        break;
    case quoteXML:
        str.replace(QRegExp("\n"), "<br/>\n");
        break;
    default:
        break;
    }
    QRegExp re("  +");
    int len;
    int pos = 0;
    /*  match() is obsolete since 3.0 so there is a big chance
        that this will be replaced and we get
        bug-reports from the users ... */
#if COMPAT_QT_VERSION < 0x030000
    while ((pos = re.match(str, pos, &len)) != -1) {
#else
    while ((pos = re.search(str, pos)) != -1) {
        len = re.matchedLength();
#endif
        if (len == 1)
            continue;
        QString s = " ";
        for (int i = 1; i < len; i++)
            s += "&nbsp;";
        str.replace(pos, len, s);
    }
    return str;
}


