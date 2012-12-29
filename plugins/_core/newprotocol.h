/***************************************************************************
                          newprotocol.h  -  description
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

#ifndef _NEWPROTOCOL_H
#define _NEWPROTOCOL_H

#include <vector>
#include <QWizard>
#include "plugins.h"
#include "clients/client.h"
#include "services.h"

#include "ui_newprotocolbase.h"

class ConnectWnd;
class CorePlugin;

class NewProtocol : public QDialog
{
    Q_OBJECT
public:
    NewProtocol(const SIM::Services::Ptr& services, const QString& profileName, QWidget *parent);
    ~NewProtocol();

protected slots:
	void accept();
	void currentProtocolChanged(int index);

private:

	void loadProtocolPlugins();
	void fillProtocolsCombobox();
	void destroyProtocolParametersWidget();
	SIM::ProtocolPtr protocolByIndex(int index);
	void setProtocolParametersWidget(QWidget* widget);

	QWidget* m_connectionParameters;
	QList<SIM::PluginPtr> m_protocolPlugins;
	QString m_profileName;
	Ui::NewProtocol* m_ui;
    SIM::Services::Ptr m_services;
};

#endif

