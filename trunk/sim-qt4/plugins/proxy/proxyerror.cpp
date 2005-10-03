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

#include "simapi.h"
#include "proxyerror.h"
#include "proxycfg.h"

#include <QPixmap>
#include <QLayout>
#include <QLabel>

#include <QBoxLayout>

ProxyError::ProxyError(ProxyPlugin *plugin, TCPClient *client, const char *msg)
        : QDialog( NULL)
{
    setAttribute( Qt::WA_DeleteOnClose);
    setupUi( this);
    SET_WNDPROC("proxy")
    setWindowIcon(getIcon("error"));
    setButtonsPict(this);
    setWindowTitle(caption());
    m_plugin = plugin;
    m_client = client;
    if (msg && *msg)
        lblMessage->setText(i18n(msg));
    if (layout() && layout()->inherits("QBoxLayout")){
        QBoxLayout *lay = static_cast<QBoxLayout*>(layout());
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

void *ProxyError::processEvent(Event *e)
{
    if (e->type() == EventClientsChanged){
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            if (getContacts()->getClient(i) == m_client)
                return NULL;
        }
        m_client = NULL;
        close();
    }
    return NULL;
}

void ProxyError::accept()
{
    if (m_client){
        emit apply();
        m_client->setStatus(m_client->getManualStatus(), m_client->getCommonStatus());
    }
    QDialog::accept();
}

#ifndef WIN32
#include "proxyerror.moc"
#endif

