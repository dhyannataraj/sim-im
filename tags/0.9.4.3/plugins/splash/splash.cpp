/***************************************************************************
                          splash.cpp  -  description
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

#include "splash.h"
#include "simapi.h"
#include "aboutdata.h"

#include <qwidget.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qfile.h>
#include <qpainter.h>

Plugin *createSplashPlugin(unsigned base, bool bStart, Buffer*)
{
    Plugin *plugin = new SplashPlugin(base, bStart);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Splash"),
        I18N_NOOP("Plugin provides splash screen"),
        VERSION,
        createSplashPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

SplashPlugin::SplashPlugin(unsigned base, bool bStart)
        : Plugin(base), EventReceiver(LowestPriority)
{
    splash = NULL;
    m_bStart = bStart;
    if (m_bStart){
        string pictPath = app_file("pict/splash.png");
        QPixmap pict(QFile::decodeName(pictPath.c_str()));
        if (!pict.isNull()){
            KAboutData *about_data = getAboutData();
            QString text = about_data->appName();
            text += " ";
            text += about_data->version();
            QPainter p(&pict);
            QFont f = qApp->font();
            f.setBold(true);
            p.setFont(f);
            QRect rc = p.boundingRect(0, 0, pict.width(), pict.height(), Qt::AlignLeft | Qt::AlignTop, text);
            int x = pict.width() - 7 - rc.width();
            int y = 7 + rc.height();
            p.setPen(QColor(0x80, 0x80, 0x80));
            p.drawText(x, y, text);
            x -= 2;
            y -= 2;
            p.setPen(QColor(0xFF, 0xFF, 0xE0));
            p.drawText(x, y, text);
            p.end();
            splash = new QWidget(NULL, "splash",
                                 QWidget::WType_TopLevel | QWidget::WStyle_Customize |
                                 QWidget::WStyle_NoBorderEx | QWidget::WStyle_StaysOnTop);
            splash->resize(pict.width(), pict.height());
            QWidget *desktop = QApplication::desktop();
            splash->move((desktop->width() - pict.width()) / 2, (desktop->height() - pict.height()) / 2);
            splash->setBackgroundPixmap(pict);
            const QBitmap *mask = pict.mask();
            if (mask) splash->setMask(*mask);
            splash->show();
        }
    }
}

SplashPlugin::~SplashPlugin()
{
    if (splash)
        delete splash;
}

void *SplashPlugin::processEvent(Event *e)
{
    switch(e->type()){
    case EventInit:
        if (splash){
            delete splash;
            splash = NULL;
        }
        break;
    }
    return NULL;
}
