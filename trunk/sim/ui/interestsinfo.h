/***************************************************************************
                          interestsinfo.h  -  description
                             -------------------
    begin                : Sun Mar 24 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _INTERESTSINFO_H
#define _INTERESTSINFO_H

#include "defs.h"
#include "interestsinfobase.h"

class ICQUser;

class InterestsInfo : public InterestsInfoBase
{
    Q_OBJECT
public:
    InterestsInfo(QWidget *p, bool readOnly=false);
public slots:
    void apply(ICQUser *u);
    void load(ICQUser *u);
    void save(ICQUser *u);
protected slots:
    void adjustEnabled(int);
};

#endif

