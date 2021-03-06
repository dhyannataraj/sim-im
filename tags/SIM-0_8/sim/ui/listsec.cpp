/***************************************************************************
                          listsec.cpp  -  description
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

#include "listsec.h"
#include "icons.h"
#include "client.h"
#include "usertbl.h"

#include <qlabel.h>
#include <qpixmap.h>

ListSecurity::ListSecurity(QWidget *parent)
        : ListSecurityBase(parent)
{
    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(tblUsers, SIGNAL(changed()), this, SLOT(tblChanged()));
    tblUsers->setEnabled(pClient->m_state == ICQClient::Logged);
}

void ListSecurity::init()
{
    list<ICQUser*> &users = pClient->contacts.users;
    list<ICQUser*>::iterator it;
    for (it = users.begin(); it != users.end(); it++){
        if (!checkUser(*it)) continue;
        oldValue.push_back((*it)->Uin);
        tblUsers->addUin((*it)->Uin);
    }
}

void ListSecurity::apply(ICQUser*)
{
}

void ListSecurity::processEvent(ICQEvent *e)
{
    tblUsers->setEnabled(pClient->m_state == ICQClient::Logged);
    if ((e->type() != EVENT_INFO_CHANGED) || (e->Uin() == 0)) return;
    ICQUser *u = pClient->getUser(e->Uin(), true);
    if (u == NULL) return;
    if (checkUser(u)){
        tblUsers->addUin(u->Uin);
        oldValue.push_back(u->Uin);
    }else{
        tblUsers->deleteUin(u->Uin);
        oldValue.remove(u->Uin);
    }
}

void ListSecurity::tblChanged()
{
    list<unsigned long> newValue;
    tblUsers->fillList(newValue);
    list<unsigned long>::iterator it;
    for (it = newValue.begin(); it != newValue.end(); it++){
        list<unsigned long>::iterator it1;
        for (it1 = oldValue.begin(); it1 != oldValue.end(); it1++)
            if ((*it) == (*it1)) break;
        if (it1 == oldValue.end()){
            ICQUser *u = pClient->getUser(*it);
            if (u) changeUser(u, true);
        }
    }
    for (it = oldValue.begin(); it != oldValue.end(); it++){
        list<unsigned long>::iterator it1;
        for (it1 = newValue.begin(); it1 != newValue.end(); it1++)
            if ((*it) == (*it1)) break;
        if (it1 == newValue.end()){
            ICQUser *u = pClient->getUser(*it);
            if (u) changeUser(u, false);
        }
    }
}

VisibleListSetup::VisibleListSetup(QWidget *parent)
        : ListSecurity(parent)
{
    init();
    lblHead->setText(i18n("Visible list"));
    lblPict->setPixmap(Pict("visiblelist"));
}

bool VisibleListSetup::checkUser(ICQUser *u)
{
    return u->inVisible;
}

void VisibleListSetup::changeUser(ICQUser *u, bool bSet)
{
    pClient->setInVisible(u, bSet);
}

InvisibleListSetup::InvisibleListSetup(QWidget *parent)
        : ListSecurity(parent)
{
    init();
    lblHead->setText(i18n("Invisible list"));
    lblPict->setPixmap(Pict("invisiblelist"));
}

bool InvisibleListSetup::checkUser(ICQUser *u)
{
    return u->inInvisible;
}

void InvisibleListSetup::changeUser(ICQUser *u, bool bSet)
{
    pClient->setInInvisible(u, bSet);
}

IgnoreListSetup::IgnoreListSetup(QWidget *parent)
        : ListSecurity(parent)
{
    init();
    lblHead->setText(i18n("Ignore list"));
    lblPict->setPixmap(Pict("ignorelist"));
}

bool IgnoreListSetup::checkUser(ICQUser *u)
{
    return u->inIgnore;
}

void IgnoreListSetup::changeUser(ICQUser *u, bool bSet)
{
    pClient->setInIgnore(u, bSet);
}

#ifndef _WINDOWS
#include "listsec.moc"
#endif

