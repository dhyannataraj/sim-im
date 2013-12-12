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

#include <qwidget.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qfile.h>
#include <qpainter.h>
#include <qsplashscreen.h>

#include "aboutdata.h"
#include "misc.h"

#include "splash.h"

using namespace SIM;

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
        QPixmap pict(app_file("pict/sim-im-splash.png"));
		// FIXME: better use QSplash with QSplashScreen::drawContents()
        if (!pict.isNull()){
			QString version = VERSION;
			QString build = "build ";
#ifdef CVS_BUILD
			build += "git ";
			build +=  REVISION_NUMBER;
			build += ", ";
#endif
// FIXME: Later we should better use cmake's string(TIMESTAMP ...) to get build date
			// Example of __DATE__ string: "Jul 27 2012"
			//                              01234567890
			QString build_date;
			QString raw_date = __DATE__;
			QString months[] = {NULL,"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
			for (int i = 1; i<=12; i++)
			{
			  if (raw_date.contains(months[i]))
			  {
			    build_date = QString::number(i);
			  }
			}
			if (build_date.length()<2) build_date = "0" + build_date;
			build_date = "" + raw_date[4] + raw_date[5] + "." + build_date;
			build_date += "." + raw_date[9] + raw_date[10];
			build += build_date;
			QPainter p(&pict);
			QFont f("Sans Serif",10);
			f.setStyleHint(QFont::SansSerif);
			f.setStyleStrategy(QFont::NoAntialias);
			//f.setStyleStrategy(QFont::PreferBitmap); // FIXME May be this would be better for windows, as bitmap fonts are AFAIK disabled in Linux 
			p.setFont(f);
			int x = 104;
			int y = 76;
			p.setPen(QColor(0xa2, 0xa2, 0xa2));
			
			p.drawText(x, y, version);
			y=91;
			f.setPointSize(6);
			p.setFont(f);
			
			p.drawText(x, y, build);
			splash = new QWidget(NULL, "splash",
								 QWidget::WType_TopLevel | QWidget::WStyle_Customize |
								 QWidget::WStyle_NoBorderEx | QWidget::WStyle_StaysOnTop);

			QWidget *desktop =  qApp->desktop();  //QApplication::desktop();
			int desk_width = desktop->geometry().width();
			int desk_height = desktop->geometry().height();
			if ((desk_width/desk_height)==2) //widescreen or double screen
				splash->move((desktop->width()/2 - pict.width()) / 2, (desktop->height() - pict.height()) / 2);
			else //normal screen 
				splash->move((desktop->width() - pict.width()) / 2, (desktop->height() - pict.height()) / 2);
			splash->setBackgroundPixmap(pict);
			splash->resize(pict.width(), pict.height());
			splash->repaint();
			const QBitmap *mask = pict.mask();
			p.end();
			if (mask)
				splash->setMask(*mask);
			splash->show();
        }
    }
}

SplashPlugin::~SplashPlugin()
{
    delete splash;
}

bool SplashPlugin::processEvent(Event *e)
{
    if(e->type() == eEventInit) {
        if (splash){
            delete splash;
            splash = NULL;
        }
    }
    return false;
}
