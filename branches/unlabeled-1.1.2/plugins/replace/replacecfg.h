/***************************************************************************
                          replacecfg.h  -  description
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

#ifndef _REPLACECFG_H
#define _REPLACECFG_H

#include "replacecfgbase.h"

class ReplacePlugin;

class ReplaceCfg : public ReplaceCfgBase
{
    Q_OBJECT
public:
    ReplaceCfg(QWidget *parent, ReplacePlugin *plugin);
    virtual ~ReplaceCfg();
public slots:
    void apply();
protected:
    void resizeEvent(QResizeEvent *e);
    ReplacePlugin *m_plugin;
};

#endif

