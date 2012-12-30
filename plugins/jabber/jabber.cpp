/***************************************************************************
                          jabber.cpp  -  description
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

#include "jabberclient.h"
#include "jabber.h"
#include "misc.h"
#include "plugin/pluginmanager.h"
#include "contacts/protocolmanager.h"
#include "clients/clientmanager.h"
#include "ui/jabberloginwidget.h"

#include <QByteArray>

using namespace SIM;

Plugin *createJabberPluginObject(const SIM::Services::Ptr& services)
{
    Plugin *plugin = new JabberPlugin(services);
    return plugin;
}

static PluginInfo info =
    {
        NULL,
        NULL,
        VERSION,
        PLUGIN_PROTOCOL,
        createJabberPluginObject
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

JabberProtocol::JabberProtocol(Plugin *plugin)
        : Protocol(plugin)
{
}

JabberProtocol::~JabberProtocol()
{
}

QWidget* JabberProtocol::createLoginWidget()
{
	return new JabberLoginWidget();
}

SIM::ClientPtr JabberProtocol::createClientWithLoginWidget(QWidget* widget)
{
    JabberLoginWidget* loginwidget = qobject_cast<JabberLoginWidget*>(widget);
    if(!loginwidget)
        return SIM::ClientPtr();

    QString name = this->name() + '.' + loginwidget->jid();
    QString password = loginwidget->password();

    JabberClient* client = new JabberClient(this, name);
    client->setServer(loginwidget->server());
    client->setPort(loginwidget->port());
    client->setID(loginwidget->jid());
	return SIM::ClientPtr(client);
}


QString JabberProtocol::name()
{
    return "Jabber";
}

QString JabberProtocol::iconId()
{
    return "Jabber";
}

SIM::ClientPtr JabberProtocol::createClient(const QString& name)
{
    JabberClient* client = new JabberClient(this, name);
    ClientPtr jabber = ClientPtr(client);
    return jabber;
}


JabberPlugin *JabberPlugin::plugin = NULL;

JabberPlugin::JabberPlugin(const SIM::Services::Ptr& services) : Plugin(),
    m_services(services)
{
    plugin = this;
    m_protocol = ProtocolPtr(new JabberProtocol(this));
	m_services->protocolManager()->addProtocol(m_protocol);
}

JabberPlugin::~JabberPlugin()
{
    m_services->protocolManager()->removeProtocol(m_protocol);
}

// vim: set expandtab:

