/***************************************************************************
                          maininfo.h  -  description
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

#ifndef _MAININFO_H
#define _MAININFO_H

#include "defs.h"
#include "maininfobase.h"
#include "client.h"

class ICQUser;
class EMailInfo;
class QStringList;

class MainInfo : public MainInfoBase
{
    Q_OBJECT
public:
    MainInfo(QWidget *p, bool readOnly=false);
public slots:
    void apply(ICQUser *u);
    void load(ICQUser *u);
    void save(ICQUser *u);
    void load(ICQGroup *g);
    void save(ICQGroup *g);
protected slots:
    void setButtons(int);
    void addEmail();
    void editEmail();
    void removeEmail();
    void defaultEmail();
    void aliasChanged(const QString&);
protected:
    EMailInfo *currentMail();
    void addString(QStringList &list, QString str);
    void setCurrentEncoding(int mib);
    int  getCurrentEncoding();
    void reloadList();
    QString getAlias();
    EMailList mails;
    QString oldName;
    bool bReadOnly;
    bool bDirty;
};

#endif

