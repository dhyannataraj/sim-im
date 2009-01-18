/***************************************************************************
                          proxyerror.cpp  -  description
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

#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3BoxLayout>

#include "icons.h"
#include "misc.h"

#include "proxyerror.h"
#include "proxycfg.h"

using namespace SIM;

ProxyError::ProxyError(ProxyPlugin *plugin, TCPClient *client, const QString& msg) : QDialog(NULL, NULL, false, Qt::WDestructiveClose)
        //: ProxyErrorBase(NULL, NULL, false, Qt::WDestructiveClose)
{
	setupUi(this);
    SET_WNDPROC("proxy")
    setIcon(Pict("error"));
    setButtonsPict(this);
    setCaption(caption());
    m_plugin = plugin;
    m_client = client;
    lblMessage->setText(msg);
    if (layout() && layout()->inherits("QBoxLayout")){
        Q3BoxLayout *lay = static_cast<Q3BoxLayout*>(layout());
        ProxyConfig *cfg = new ProxyConfig(this, m_plugin, NULL, m_client);
        lay->insertWidget(1, cfg);
        cfg->show();
        setMinimumSize(sizeHint());
        connect(this, SIGNAL(apply()), cfg, SLOT(apply()));
    }
}

ProxyError::~ProxyError()
{
    if (m_client && (m_client->getState() == Client::Error))
        m_client->setStatus(STATUS_OFFLINE, false);
}

bool ProxyError::processEvent(Event *e)
{
    if (e->type() == eEventClientsChanged){
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            if (getContacts()->getClient(i) == m_client)
                return false;
        }
        m_client = NULL;
        close();
    }
    return false;
}

void ProxyError::accept()
{
    if (m_client){
        emit apply();
        m_client->setStatus(m_client->getManualStatus(), m_client->getCommonStatus());
    }
    QDialog::accept();
}

/*
#ifndef NO_MOC_INCLUDES
#include "proxyerror.moc"
#endif
*/

