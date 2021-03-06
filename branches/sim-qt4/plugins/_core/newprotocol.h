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

#include "simapi.h"
#include "stl.h"

#include "newprotocolbase.h"

#include <Q3Wizard>

class ConnectWnd;
class CorePlugin;

class NewProtocol : public Q3Wizard, public Ui::NewProtocolBase, public EventReceiver
{
    Q_OBJECT
public:
    NewProtocol(QWidget *parent);
    ~NewProtocol();
    Client	*m_client;
    bool	connected() { return m_bConnected; }
signals:
    void apply();
protected slots:
    void protocolChanged(int);
    void okEnabled(bool);
    void pageChanged(const QString&);
    void loginComplete();
protected:
    virtual void *processEvent(Event*);
    virtual void reject();
    vector<Protocol*>	m_protocols;
    ConnectWnd	*m_connectWnd;
    QWidget *m_setup;
    QWidget *m_last;
    bool	m_bConnect;
    bool	m_bConnected;
    bool	m_bStart;
};

#endif

