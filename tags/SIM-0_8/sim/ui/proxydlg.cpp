/***************************************************************************
                          proxydlg.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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

#include "proxydlg.h"
#include "client.h"
#include "icons.h"
#include "enable.h"

#include <qpixmap.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>

ProxyDialog::ProxyDialog(QWidget *p, const QString &msg)
        : ProxyDlgBase(p, "proxy", true)
{
    setIcon(Pict("network"));
    lblMessage->setText(msg);

    connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(btnOK, SIGNAL(clicked()), this, SLOT(apply()));

    cmbProxy->insertItem(i18n("No proxy"));
    cmbProxy->insertItem(i18n("SOCKS4"));
    cmbProxy->insertItem(i18n("SOCKS5"));
    cmbProxy->insertItem(i18n("HTTP"));
#ifdef USE_OPENSSL
    cmbProxy->insertItem(i18n("HTTPS"));
#endif
    cmbProxy->setCurrentItem(pClient->ProxyType);

    edtProxyHost->setText(QString::fromLocal8Bit(pClient->ProxyHost.c_str()));
    edtProxyPort->setText(QString::number(pClient->ProxyPort));
    edtProxyPort->setValidator(new QIntValidator(1, 0xFFFF, edtProxyPort));

    edtProxyUser->setText(QString::fromLocal8Bit(pClient->ProxyUser.c_str()));
    edtProxyPasswd->setText(QString::fromLocal8Bit(pClient->ProxyPasswd.c_str()));

    chkProxyAuth->setChecked(pClient->ProxyAuth);

    connect(cmbProxy, SIGNAL(activated(int)), this, SLOT(proxyChanged(int)));
    connect(chkProxyAuth, SIGNAL(toggled(bool)), this, SLOT(proxyChanged(bool)));

    edtPort->setValidator(new QIntValidator(1024, 0xFFFF, edtPort));
    edtHost->setText(QString::fromLocal8Bit(pClient->ServerHost.c_str()));
    edtPort->setText(QString::number(pClient->ServerPort));

    proxyChanged(0);
}

void ProxyDialog::apply()
{
    set(pClient->ServerHost, edtHost->text());
    pClient->ServerPort = edtPort->text().toUInt();
    pClient->ProxyType = cmbProxy->currentItem();
    set(pClient->ProxyHost, edtProxyHost->text());
    pClient->ProxyPort = edtProxyPort->text().toUInt();
    set(pClient->ProxyUser, edtProxyUser->text());
    set(pClient->ProxyPasswd, edtProxyPasswd->text());
    pClient->ProxyAuth = chkProxyAuth->isChecked();
    close();
}

void ProxyDialog::proxyChanged(bool)
{
    proxyChanged(0);
}

void ProxyDialog::proxyChanged(int)
{
    int proxyType = cmbProxy->currentItem();
    lblProxyHost->setEnabled(proxyType > 0);
    lblProxyPort->setEnabled(proxyType > 0);
    edtProxyHost->setEnabled(proxyType > 0);
    edtProxyPort->setEnabled(proxyType > 0);
    chkProxyAuth->setEnabled(proxyType > 1);
    lblProxyUser->setEnabled((proxyType > 1) && chkProxyAuth->isChecked());
    lblProxyPasswd->setEnabled((proxyType > 1) && chkProxyAuth->isChecked());
    edtProxyUser->setEnabled((proxyType > 1) && chkProxyAuth->isChecked());
    edtProxyPasswd->setEnabled((proxyType > 1) && chkProxyAuth->isChecked());
}

#ifndef _WINDOWS
#include "proxydlg.moc"
#endif

