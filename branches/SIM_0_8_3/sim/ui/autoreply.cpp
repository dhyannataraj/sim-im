/***************************************************************************
                          autoreply.cpp  -  description
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

#include "autoreply.h"
#include "client.h"
#include "mainwin.h"
#include "icons.h"
#include "enable.h"

#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qaccel.h>

AutoReplyDlg::AutoReplyDlg(QWidget *p, unsigned long _status)
        : AutoreplyBase(p, "autoreply", false, WDestructiveClose), status(_status)
{
    setButtonsPict(this);
    setIcon(Pict(SIMClient::getStatusIcon(status)));
    setWFlags(WDestructiveClose);
    connect(btnOK, SIGNAL(clicked()), this, SLOT(apply()));
    timeLeft = 15;
    tick();
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    switch (status){
    case ICQ_STATUS_AWAY:
        edtMessage->setText(QString::fromLocal8Bit(
                                pClient->getAutoResponse(NULL, offsetof(UserSettings, AutoResponseAway))));
        break;
    case ICQ_STATUS_NA:
        edtMessage->setText(QString::fromLocal8Bit(
                                pClient->getAutoResponse(NULL, offsetof(UserSettings, AutoResponseNA))));
        break;
    case ICQ_STATUS_OCCUPIED:
        edtMessage->setText(QString::fromLocal8Bit(
                                pClient->getAutoResponse(NULL, offsetof(UserSettings, AutoResponseOccupied))));
        break;
    case ICQ_STATUS_DND:
        edtMessage->setText(QString::fromLocal8Bit(
                                pClient->getAutoResponse(NULL, offsetof(UserSettings, AutoResponseDND))));
        break;
    case ICQ_STATUS_FREEFORCHAT:
        edtMessage->setText(QString::fromLocal8Bit(
                                pClient->getAutoResponse(NULL, offsetof(UserSettings, AutoResponseFFC))));
        break;
    }
    connect(edtMessage, SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(chkNoShow, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
    QAccel *accel = new QAccel(this);
    accel->connectItem(accel->insertItem(QAccel::stringToKey("Ctrl+Enter")), this, SLOT(apply()));
    accel->connectItem(accel->insertItem(QAccel::stringToKey("Ctrl+Return")), this, SLOT(apply()));
}

void AutoReplyDlg::toggled(bool)
{
    textChanged();
}

void AutoReplyDlg::textChanged()
{
    if (timeLeft < 0) return;
    lblTimer->setText("");
    timeLeft = -1;
    timer->stop();
}

void AutoReplyDlg::tick()
{
    if (timeLeft == 0){
        close();
        return;
    }
    if (timeLeft < 0) return;
    lblTimer->setText(i18n("Close in %1 sec") .arg(QString::number(timeLeft)));
    timeLeft--;
}

void AutoReplyDlg::apply()
{
    SIMUser *u = static_cast<SIMUser*>(pClient->owner);
    switch (status){
    case ICQ_STATUS_AWAY:
        set(&u->settings.AutoResponseAway, edtMessage->text());
        pMain->setNoShowAway(chkNoShow->isChecked());
        break;
    case ICQ_STATUS_NA:
        set(&u->settings.AutoResponseNA, edtMessage->text());
        pMain->setNoShowNA(chkNoShow->isChecked());
        break;
    case ICQ_STATUS_DND:
        set(&u->settings.AutoResponseDND, edtMessage->text());
        pMain->setNoShowDND(chkNoShow->isChecked());
        break;
    case ICQ_STATUS_OCCUPIED:
        set(&u->settings.AutoResponseOccupied, edtMessage->text());
        pMain->setNoShowOccupied(chkNoShow->isChecked());
        break;
    case ICQ_STATUS_FREEFORCHAT:
        set(&u->settings.AutoResponseFFC, edtMessage->text());
        pMain->setNoShowFFC(chkNoShow->isChecked());
        break;
    }
    close();
}

#ifndef _WINDOWS
#include "autoreply.moc"
#endif

