/***************************************************************************
                          styles.h  -  description
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

#ifndef _STYLES_H
#define _STYLES_H

#include "cfg.h"
#include "plugins.h"

class QFont;
class QStyle;
class QPalette;

struct StylesData
{
    SIM::Data	Style;
    SIM::Data	SystemFonts;
    SIM::Data	BaseFont;
    SIM::Data	MenuFont;
    SIM::Data	MessageFont;
    SIM::Data	SystemColors;
    SIM::Data	BtnColor;
    SIM::Data	BgColor;
};

class StylesPlugin : public SIM::Plugin
{
public:
    StylesPlugin(unsigned, Buffer*);
    ~StylesPlugin();
    PROP_STR(Style);
    PROP_BOOL(SystemFonts);
    PROP_STR(BaseFont);
    PROP_STR(MenuFont);
    PROP_STR(MessageFont);
    PROP_BOOL(SystemColors);
    PROP_ULONG(BtnColor);
    PROP_ULONG(BgColor);
    virtual QCString getConfig();
    QWidget *createConfigWindow(QWidget *parent);
    void setFonts();
    void setupDefaultFonts();
    void setColors();
    void setStyles();
protected:
    QFont		*m_saveBaseFont;
    QFont		*m_saveMenuFont;
    QPalette	*m_savePalette;
    QStyle		*m_saveStyle;
    StylesData	data;
    friend class FontConfig;
};

#endif

