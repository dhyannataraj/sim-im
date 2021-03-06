/***************************************************************************
                          accept.h  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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

#ifndef _ACCEPT_H
#define _ACCEPT_H

#include "defs.h"
#include "acceptbase.h"

class ICQUser;
class ICQGroup;
struct UserSettings;

class AcceptDialog : public AcceptBase
{
    Q_OBJECT
public:
    AcceptDialog(QWidget *p, bool bReadOnly=false);
public slots:
    void load(ICQUser *u);
    void save(ICQUser *u);
    void load(ICQGroup *g);
    void save(ICQGroup *g);
    void apply(ICQUser *u);
protected slots:
    void overrideChanged(bool);
    void overrideMsgChanged(bool);
    void modeChanged(int);
protected:
    void load(UserSettings *fileSettings, UserSettings *msgSettings);
    void save(UserSettings *fileSettings, UserSettings *msgSettings);
    bool bReadOnly;
};

#endif

