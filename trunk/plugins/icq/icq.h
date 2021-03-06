/***************************************************************************
                          icq.h  -  description
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

#ifndef _ICQ_H
#define _ICQ_H

#include "contacts.h"

class CorePlugin;

const unsigned IcqCmdBase	= 0x00040000;

#include "icq_events.h"

const unsigned long CmdVisibleList		= IcqCmdBase;
const unsigned long CmdInvisibleList    = IcqCmdBase + 1;
const unsigned long CmdGroups			= IcqCmdBase + 2;
const unsigned long CmdIcqSendMessage	= IcqCmdBase + 5;
const unsigned long CmdShowWarning		= IcqCmdBase + 6;
const unsigned long CmdPasswordFail		= IcqCmdBase + 7;
const unsigned long CmdPasswordSuccess	= IcqCmdBase + 8;

const unsigned long MenuSearchResult		= IcqCmdBase;
const unsigned long MenuIcqGroups		= IcqCmdBase + 2;

class ICQProtocol : public SIM::Protocol
{
public:
    ICQProtocol(SIM::Plugin *plugin);
    ~ICQProtocol();
    SIM::Client	*createClient(Buffer *cfg);
    const SIM::CommandDef *description();
    const SIM::CommandDef *statusList();
    static const SIM::CommandDef *_statusList();
    virtual const SIM::DataDef *userDataDef();
    static const SIM::DataDef *icqUserData;
};

class AIMProtocol : public SIM::Protocol
{
public:
    AIMProtocol(SIM::Plugin *plugin);
    ~AIMProtocol();
    SIM::Client	*createClient(Buffer *cfg);
    const SIM::CommandDef *description();
    const SIM::CommandDef *statusList();
    virtual const SIM::DataDef *userDataDef();
    static const SIM::DataDef *icqUserData;
};

class ICQPlugin : public SIM::Plugin
{
public:
    ICQPlugin(unsigned base);
    virtual ~ICQPlugin();
    unsigned OscarPacket;
    unsigned ICQDirectPacket;
    unsigned AIMDirectPacket;
    unsigned RetrySendDND;
    unsigned RetrySendOccupied;
    static SIM::Protocol *m_icq;
    static SIM::Protocol *m_aim;
    static ICQPlugin  *icq_plugin;
    static CorePlugin *core;
    void registerMessages();
    void unregisterMessages();
};

#endif

