/***************************************************************************
                          newprotocol.cpp  -  description
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

#include "newprotocol.h"


#include "profilemanager.h"
#include "newprotocol.h"
#include "core.h"
#include "contacts/client.h"
#include "contacts/protocolmanager.h"
#include "log.h"
#include "imagestorage/imagestorage.h"

#include <QPixmap>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>

using namespace std;
using namespace SIM;

NewProtocol::NewProtocol(const QString& profileName, QWidget *parent) :
		QDialog(parent),
		m_profileName(profileName),
		m_ui(new Ui::NewProtocol)
{
	m_ui->setupUi(this);

	loadProtocolPlugins();
	fillProtocolsCombobox();
}

NewProtocol::~NewProtocol()
{
    log(L_DEBUG, "NewProtocol::~NewProtocol()");
}

void NewProtocol::loadProtocolPlugins()
{
	QStringList plugins = getPluginManager()->enumPlugins();
	for(const QString& s: plugins)
	{
		if(getPluginManager()->isPluginProtocol(s))
		{
			PluginPtr plugin = getPluginManager()->plugin(s);
			if(plugin)
			{
				m_protocolPlugins.append(plugin);
			}
		}
	}
}

void NewProtocol::accept()
{
	ProtocolPtr protocol = protocolByIndex(m_ui->cb_protocol->currentIndex());
	getProfileManager()->newProfile(m_profileName);
	getProfileManager()->selectProfile(m_profileName);
	ProfilePtr profile = getProfileManager()->currentProfile();
	profile->enablePlugin(protocol->plugin()->name());
	QDialog::accept();
}

void NewProtocol::currentProtocolChanged(int index)
{
	destroyProtocolParametersWidget();

	ProtocolPtr protocol = protocolByIndex(index);
	setProtocolParametersWidget(protocol->createLoginWidget());
}

void NewProtocol::fillProtocolsCombobox()
{
	int totalProtocolCount = getProtocolManager()->protocolCount();
	for(int i = 0; i < totalProtocolCount; i++)
	{
		ProtocolPtr protocol = getProtocolManager()->protocol(i);
		QIcon icon = SIM::getImageStorage()->icon(protocol->iconId());
		QString protocolName = protocol->name();
		m_ui->cb_protocol->addItem(icon, protocolName);
	}
}

void NewProtocol::destroyProtocolParametersWidget()
{

}

SIM::ProtocolPtr NewProtocol::protocolByIndex(int index)
{
	return getProtocolManager()->protocol(index);
}

void NewProtocol::NewProtocol::setProtocolParametersWidget(QWidget* widget)
{

}

// vim: set expandtab:


