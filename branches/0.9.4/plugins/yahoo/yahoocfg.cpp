/***************************************************************************
                          yahoocfg.cpp  -  description
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

#include "yahoocfg.h"
#include "yahooclient.h"
#include "linklabel.h"

#include <qtimer.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>

YahooConfig::YahooConfig(QWidget *parent, YahooClient *client, bool bConfig)
        : YahooConfigBase(parent)
{
    m_client = client;
    m_bConfig = bConfig;
    if (m_bConfig)
        tabCfg->removePage(tabYahoo);
    QTimer::singleShot(0, this, SLOT(changed()));
    edtLogin->setText(m_client->getLogin());
    edtPassword->setText(m_client->getPassword());
    edtServer->setText(QString::fromLocal8Bit(m_client->getServer()));
    edtPort->setValue(m_client->getPort());
    edtMinPort->setValue(m_client->getMinPort());
    edtMaxPort->setValue(m_client->getMaxPort());
    connect(edtLogin, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString&)));
    connect(edtPassword, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString&)));
    connect(edtServer, SIGNAL(textChanged(const QString&)), this, SLOT(changed(const QString&)));
    connect(edtPort, SIGNAL(valueChanged(const QString&)), this, SLOT(changed(const QString&)));
    lnkReg->setText(i18n("Get a Yahoo! ID"));
    lnkReg->setUrl("http://edit.yahoo.com/config/eval_register");
    chkHTTP->setChecked(m_client->getUseHTTP());
    chkAuto->setChecked(m_client->getAutoHTTP());
    connect(chkAuto, SIGNAL(toggled(bool)), this, SLOT(autoToggled(bool)));
    autoToggled(m_client->getAutoHTTP());
}

void YahooConfig::apply(Client*, void*)
{
}

void YahooConfig::apply()
{
    if (!m_bConfig){
        m_client->setLogin(edtLogin->text());
        m_client->setPassword(edtPassword->text());
    }
    m_client->setServer(edtServer->text().local8Bit());
    m_client->setPort((unsigned short)atol(edtPort->text()));
    m_client->setMinPort((unsigned short)atol(edtMinPort->text()));
    m_client->setMaxPort((unsigned short)atol(edtMaxPort->text()));
    m_client->setUseHTTP(chkHTTP->isChecked());
    m_client->setAutoHTTP(chkAuto->isChecked());
}

void YahooConfig::autoToggled(bool bState)
{
    chkHTTP->setEnabled(!bState);
}

void YahooConfig::changed(const QString&)
{
    changed();
}

void YahooConfig::changed()
{
    emit okEnabled(!edtLogin->text().isEmpty() &&
                   !edtPassword->text().isEmpty() &&
                   !edtServer->text().isEmpty() &&
                   atol(edtPort->text()));
}

#ifndef NO_MOC_INCLUDES
#include "yahoocfg.moc"
#endif

