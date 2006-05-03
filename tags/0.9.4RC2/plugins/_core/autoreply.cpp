/***************************************************************************
                          autoreply.cpp  -  description
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

#include "autoreply.h"
#include "core.h"
#include "ballonmsg.h"
#include "editfile.h"

#include <qpixmap.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include <qlabel.h>

AutoReplyDialog::AutoReplyDialog(unsigned status)
        : AutoReplyBase(NULL, NULL, true)
{
    m_status = status;
    SET_WNDPROC("mainwnd");
    const char *text = NULL;
    const char *icon = NULL;
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        for (const CommandDef *d = getContacts()->getClient(i)->protocol()->statusList(); d->text; d++){
            if (d->id == status){
                text = d->text;
                icon = d->icon;
                break;
            }
        }
        if (text)
            break;
    }
    if (text == NULL)
        return;
    setCaption(i18n("Autoreply message") + " " + i18n(text));
    setIcon(Pict(icon));
    m_time = 15;
    lblTime->setText(i18n("Close after %n second", "Close after %n seconds", m_time));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer->start(1000);
    ARUserData *ar = (ARUserData*)getContacts()->getUserData(CorePlugin::m_plugin->ar_data_id);
    text = get_str(ar->AutoReply, m_status);
    if ((text == NULL) || (*text == 0))
        text = get_str(ar->AutoReply, m_status);
    if (text)
        edtAutoResponse->setText(QString::fromUtf8(text));
    connect(edtAutoResponse, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(chkNoShow, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(help()));
    Event e(EventTmplHelpList);
    edtAutoResponse->helpList = (const char**)e.process();
}

AutoReplyDialog::~AutoReplyDialog()
{
}

void AutoReplyDialog::textChanged()
{
    stopTimer();
}

void AutoReplyDialog::toggled(bool)
{
    stopTimer();
}

void AutoReplyDialog::stopTimer()
{
    if (m_timer == NULL)
        return;
    delete m_timer;
    m_timer = NULL;
    lblTime->hide();
}

void AutoReplyDialog::timeout()
{
    if (--m_time <= 0){
        accept();
        return;
    }
    lblTime->setText(i18n("Close after %n second", "Close after %n seconds", m_time));
}

void AutoReplyDialog::accept()
{
    CorePlugin::m_plugin->setNoShowAutoReply(m_status, chkNoShow->isChecked() ? "1" : "");
    ARUserData *ar = (ARUserData*)(getContacts()->getUserData(CorePlugin::m_plugin->ar_data_id));
    set_str(&ar->AutoReply, m_status, edtAutoResponse->text().utf8());
    AutoReplyBase::accept();
}

void AutoReplyDialog::help()
{
    stopTimer();
    QString helpString = i18n("In text you can use:");
    helpString += "\n";
    Event e(EventTmplHelp, &helpString);
    e.process();
    BalloonMsg::message(helpString, btnHelp, false, 400);
}

#ifndef _MSC_VER
#include "autoreply.moc"
#endif

