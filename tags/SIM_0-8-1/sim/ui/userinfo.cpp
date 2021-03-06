/***************************************************************************
                          userinfo.cpp  -  description
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

#include "userinfo.h"
#include "icons.h"
#include "client.h"
#include "setupdlg.h"

#include "ui/maininfo.h"
#include "ui/homeinfo.h"
#include "ui/workinfo.h"
#include "ui/moreinfo.h"
#include "ui/aboutinfo.h"
#include "ui/interestsinfo.h"
#include "ui/pastinfo.h"
#include "ui/phonebook.h"
#include "ui/alertdialog.h"
#include "ui/msgdialog.h"
#include "ui/accept.h"
#include "ui/soundsetup.h"
#include "ui/enable.h"

#include <qlistview.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>

#define PAGE(A)	  static QWidget *p_##A(QWidget *p, unsigned) { return new A(p, true); }

PAGE(MainInfo)
PAGE(HomeInfo)
PAGE(WorkInfo)
PAGE(MoreInfo)
PAGE(AboutInfo)
PAGE(InterestsInfo)
PAGE(PastInfo)
PAGE(PhoneBookDlg)
PAGE(AlertDialog)
PAGE(AcceptDialog)
PAGE(SoundSetup)

static QWidget *p_MsgDialog(QWidget *p, unsigned param) { return new MsgDialog(p, param, true); }

UserInfo::UserInfo(QWidget *parent, unsigned long uin, int page)
        : UserInfoBase(parent)
{
    inSave = false;
    setButtonsPict(this);

    m_nUin = uin;
    ICQUser *u = pClient->getUser(m_nUin);
    if (uin && (u == NULL)) return;

    lstBars->clear();
    lstBars->header()->hide();
    lstBars->setSorting(1);
    connect(lstBars, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    itemMain = new QListViewItem(lstBars, i18n("User info"), QString::number(SETUP_DETAILS));
    itemMain->setOpen(true);

    addWidget(p_MainInfo, SETUP_MAININFO, i18n("Main info"), "main");
    if (u && (u->Type == USER_TYPE_ICQ)){
        addWidget(p_HomeInfo, SETUP_HOMEINFO, i18n("Home info"), "home");
        addWidget(p_WorkInfo, SETUP_WORKINFO, i18n("Work info"), "work");
        addWidget(p_MoreInfo, SETUP_MOREINFO, i18n("More info"), "more");
        addWidget(p_AboutInfo, SETUP_ABOUT, i18n("About info"), "info");
        addWidget(p_InterestsInfo, SETUP_INTERESTS, i18n("Interests"), "interest");
        addWidget(p_PastInfo, SETUP_PAST, i18n("Group/Past"), "past");
    }
    addWidget(p_PhoneBookDlg, SETUP_PHONE, i18n("Phone book"), "phone");

    if (u && (u->Type == USER_TYPE_ICQ)){
        itemMain = new QListViewItem(lstBars, i18n("Preferences"), QString::number(SETUP_PREFERENCES));
        itemMain->setOpen(true);
        addWidget(p_AlertDialog, SETUP_ALERT, i18n("Alert"), "alert");
        addWidget(p_AcceptDialog, SETUP_ACCEPT, i18n("Accept"), "message");
        addWidget(p_SoundSetup, SETUP_SOUND, i18n("Sound"), "sound");
        addWidget(p_MsgDialog, SETUP_AR_AWAY,
                  SIMClient::getStatusText(ICQ_STATUS_AWAY), SIMClient::getStatusIcon(ICQ_STATUS_AWAY),
                  ICQ_STATUS_AWAY);
        addWidget(p_MsgDialog, SETUP_AR_NA,
                  SIMClient::getStatusText(ICQ_STATUS_NA), SIMClient::getStatusIcon(ICQ_STATUS_NA),
                  ICQ_STATUS_NA);
        addWidget(p_MsgDialog, SETUP_AR_OCCUPIED,
                  SIMClient::getStatusText(ICQ_STATUS_OCCUPIED), SIMClient::getStatusIcon(ICQ_STATUS_OCCUPIED),
                  ICQ_STATUS_OCCUPIED);
        addWidget(p_MsgDialog, SETUP_AR_DND,
                  SIMClient::getStatusText(ICQ_STATUS_DND), SIMClient::getStatusIcon(ICQ_STATUS_DND),
                  ICQ_STATUS_DND);
        addWidget(p_MsgDialog, SETUP_AR_FREEFORCHAT,
                  SIMClient::getStatusText(ICQ_STATUS_FREEFORCHAT), SIMClient::getStatusIcon(ICQ_STATUS_FREEFORCHAT),
                  ICQ_STATUS_FREEFORCHAT);
    }
    raiseWidget(page ? page : SETUP_MAININFO);

    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(saveInfo()));
    connect(btnUpdate, SIGNAL(clicked()), this, SLOT(update()));

    loadInfo();
}

void UserInfo::addWidget(PAGEPROC *proc, int index, const QString &name, const char *icon, int param)
{
    QListViewItem *item = new QListViewItem(itemMain, name, QString::number(index));
    item->setPixmap(0, Pict(icon));
    item->setText(3, QString::number((unsigned)proc));
    item->setText(4, QString::number(param));
}

void UserInfo::update()
{
    pClient->addInfoRequest(m_nUin, true);
}

void UserInfo::saveInfo()
{
    ICQUser *u = pClient->getUser(m_nUin, m_nUin == 0);
    if (u == NULL) return;
    if (m_nUin == 0) m_nUin = u->Uin;
    inSave = true;
    emit saveInfo(u);
    inSave = false;
    ICQEvent e(EVENT_INFO_CHANGED, m_nUin);
    processEvent(&e);
}

void UserInfo::processEvent(ICQEvent *e)
{
    if (inSave) return;
    btnUpdate->setEnabled(pClient->isLogged());
    if ((e->type() != EVENT_INFO_CHANGED) || (e->Uin() != m_nUin)) return;
    loadInfo();
}

void UserInfo::loadInfo()
{
    ICQUser *u = pClient->getUser(m_nUin);
    if (u == NULL) return;
    emit loadInfo(u);
}

void UserInfo::selectionChanged()
{
    QListViewItem *item = lstBars->currentItem();
    if (item == NULL) return;
    unsigned id = item->text(1).toULong();
    if (id == 0) return;
    QWidget *w = tabBars->widget(id);
    if (w == NULL){
        PAGEPROC *p = (PAGEPROC*)(item->text(3).toUInt());
        if (p == NULL) return;
        QWidget *page = p(tabBars, item->text(4).toUInt());
        connect(this, SIGNAL(loadInfo(ICQUser*)), page, SLOT(load(ICQUser*)));
        connect(this, SIGNAL(saveInfo(ICQUser*)), page, SLOT(save(ICQUser*)));
        tabBars->addWidget(page, id);
        loadInfo();
        tabBars->setMinimumSize(tabBars->minimumSizeHint());
    }
    tabBars->raiseWidget(id);
}

void UserInfo::raiseWidget(int id)
{
    for (QListViewItem *item = lstBars->firstChild(); item != NULL; item = item->nextSibling())
        if (raiseWidget(item, id)) break;
}

bool UserInfo::raiseWidget(QListViewItem *i, unsigned id)
{
    if (id == i->text(1).toULong()){
        lstBars->setCurrentItem(i);
        return true;
    }
    for (QListViewItem *item = i->firstChild(); item != NULL; item = item->nextSibling())
        if (raiseWidget(item, id)) return true;
    return false;
}

#ifndef _WINDOWS
#include "userinfo.moc"
#endif

