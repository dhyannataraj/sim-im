/***************************************************************************
                          usertbl.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : shutoff@mail.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "defs.h"

#include "themes.h"
#include "mainwin.h"
#include "log.h"
#include "cfg.h"

#ifdef USE_KDE
#include <kapp.h>
extern KApplication *kApp;
#endif

#if QT_VERSION >= 300
#include <qstylefactory.h>
#else
#include <qwindowsstyle.h>
#include <qmotifstyle.h>
#include <qcdestyle.h>
#include <qplatinumstyle.h>
#include <qsgistyle.h>
#include <qmotifplusstyle.h>
#endif
#include <qlistbox.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qfont.h>
#include <qstringlist.h>

#if QT_VERSION < 300

#ifdef WIN32
#include <windows.h>

class XpStyle
{
public:
    XpStyle();
};

typedef QStyle *WINAPI CREATE_STYLE ();

CREATE_STYLE *createXpStyle = NULL;

XpStyle::XpStyle()
{
    HINSTANCE hLib = LoadLibraryA("UxTheme.dll");

    if (hLib != NULL)

        hLib = LoadLibraryA("xpstyle.dll");
    if (hLib != NULL)
        (DWORD&)createXpStyle = (DWORD)GetProcAddress(hLib,"_createXpStyle@0");
}

XpStyle _xpStyle;

#endif

class QStyleFactory
{
public:
    static QStringList keys();
    static QStyle *create(const QString&);
};

QStringList QStyleFactory::keys()
{
    QStringList styles;
    styles << "windows" << "motif" << "cde" << "platinum" << "sgi" << "motifplus";
#ifdef WIN32
    if (createXpStyle) styles << "xp";
#endif
    return styles;
}

QStyle *QStyleFactory::create(const QString &name)
{
    if (name == "windows")
        return new QWindowsStyle();
    if (name == "motif")
        return new QMotifStyle();
    if (name == "cde")
        return new QCDEStyle();
    if (name == "platinum")
        return new QPlatinumStyle();
    if (name == "sgi")
        return new QSGIStyle();
    if (name == "motifplus")
        return new QMotifPlusStyle();
#ifdef WIN32
    if (name == "xp"){
        QStyle *xpStyle = NULL;
        if (createXpStyle) xpStyle = createXpStyle();
        if (xpStyle) return xpStyle;
        return new QWindowsStyle();
    }
#endif
    return NULL;
}

#endif

Themes::Themes(QWidget *parent)
        : QObject(parent)
{
}

void Themes::fillList(QListBox *box)
{
#ifdef USE_KDE
    box->insertItem(i18n("Default style"));
#endif
    box->insertStringList(QStyleFactory::keys());
}

void Themes::setTheme(const QString &styleName)
{
    if (current == styleName) return;
#ifdef USE_KDE
    if (styleName == i18n("Default style")){
        current = "";
        kApp->disableStyles();
        kApp->enableStyles();
        return;
    }
#endif
    if (styleName.length() == 0) return;
    string name(styleName.local8Bit());
    log(L_DEBUG, "Create style %s", name.c_str());
    QStyle *style = QStyleFactory::create(styleName);
    log(L_DEBUG, "Style create %u", (unsigned)style);
    if (style == NULL) return;
    log(L_DEBUG, "Set style");
    qApp->setStyle(style);
    log(L_DEBUG, "Style OK");
    pMain->setFonts();
    current = styleName;
}

#ifndef _WINDOWS
#include "themes.moc"
#endif

