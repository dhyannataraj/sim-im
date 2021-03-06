/***************************************************************************
                          icondll.h  -  description
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

#ifndef _ICONDLL_H
#define _ICONDLL_H

#include "simapi.h"
#include <qiconset.h>
#include <map>
using namespace std;

class IconsMap : public map<unsigned, QIconSet>
{
public:
    IconsMap() {};
    const QIconSet *get(unsigned id);
};

class IconDLL
{
public:
    IconDLL();
    ~IconDLL();
    bool load(const QString &file);
	const QIconSet *get(unsigned id);
    QString name;
    IconsMap  *icon_map;
};

#endif

