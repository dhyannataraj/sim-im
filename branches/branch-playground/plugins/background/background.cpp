/***************************************************************************
                          background.cpp  -  description
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

#include <qpainter.h>
#include <qfile.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3CString>

#include "misc.h"

#include "background.h"
#include "bkgndcfg.h"
#include "log.h"

using namespace SIM;

Plugin *createBackgroundPlugin(unsigned base, bool, Buffer *config)
{
    Plugin *plugin = new BackgroundPlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Background"),
        I18N_NOOP("Plugin provides background pictures for user list"),
        VERSION,
        createBackgroundPlugin,
        PLUGIN_NOLOAD_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

BackgroundPlugin::BackgroundPlugin(unsigned base, Buffer *config)
        : Plugin(base), PropertyHub("background")
{
    redraw();
}

BackgroundPlugin::~BackgroundPlugin()
{
	PropertyHub::save();
}

Q3CString BackgroundPlugin::getConfig()
{
    return Q3CString();
}

QWidget *BackgroundPlugin::createConfigWindow(QWidget *parent)
{
    return new BkgndCfg(parent, this);
}

bool BackgroundPlugin::processEvent(Event *e)
{
    if (e->type() == eEventPaintView){
        EventPaintView *ev = static_cast<EventPaintView*>(e);
        EventPaintView::PaintView *pv = ev->paintView();;
        if (!bgImage.isNull()){
            unsigned w = bgImage.width();
            unsigned h = bgImage.height();
            int x = pv->pos.x();
            int y = pv->pos.y();
            bool bTiled = false;
            unsigned pos = property("Position").toUInt();
            switch(pos){
            case ContactLeft:
                h = pv->height;
                bTiled = true;
                break;
            case ContactScale:
                h = pv->height;
                w = pv->win->width();
                bTiled = true;
                break;
            case WindowTop:
                break;
            case WindowBottom:
                y += (bgImage.height() - pv->win->height());
                break;
            case WindowCenter:
                y += (bgImage.height() - pv->win->height()) / 2;
                break;
            case WindowScale:
                w = pv->win->width();
                h = pv->win->height();
                break;
            }
            const QPixmap &bg = makeBackground(w, h);
            if (bTiled){
                for (int py = 0; py < pv->size.height(); py += bg.height()){
                    pv->p->drawPixmap(QPoint(0, py), bgScale, QRect(x, 0, w, h));
                }
            }else{
                pv->p->drawPixmap(QPoint(0, 0), bgScale, QRect(x, y, pv->size.width(), pv->size.height()));
                pv->isStatic = true;
            }
        }
        pv->margin = pv->isGroup ? property("MarginGroup").toUInt() : property("MarginContact").toUInt();
    }
	else if(e->type() == eEventPluginLoadConfig)
	{
		PropertyHub::load();
		redraw();
	}
    return false;
}

void BackgroundPlugin::redraw()
{
    bgImage = QImage();
    bgScale = QPixmap();
    if (property("Background").toString().isEmpty())
        return;
    bgImage = QImage(property("Background").toString());
    EventRepaintView e;
    e.process();
}

QPixmap &BackgroundPlugin::makeBackground(int w, int h)
{
    if (bgImage.isNull())
        return bgScale;
    if ((bgScale.width() != w) || (bgScale.height() != h)){
        if ((bgImage.width() == w) && (bgImage.height() == h)){
            bgScale.convertFromImage(bgImage);
        }else{
            QImage img = bgImage.smoothScale(w, h);
            bgScale.convertFromImage(img);
        }
    }
    return bgScale;
}
