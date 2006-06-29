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

#include "simapi.h"

class CorePlugin;

class CorePlugin;

const unsigned IcqCmdBase	= 0x00040000;

const unsigned EventSearch			= IcqCmdBase;
const unsigned EventSearchDone		= IcqCmdBase + 1;
const unsigned EventAutoReplyFail	= IcqCmdBase + 2;
const unsigned EventRandomChat		= IcqCmdBase + 3;
const unsigned EventRandomChatInfo	= IcqCmdBase + 4;
const unsigned EventServiceReady	= IcqCmdBase + 5;

const unsigned long CmdVisibleList		= IcqCmdBase;
const unsigned long CmdInvisibleList		= IcqCmdBase + 1;
const unsigned long CmdGroups			= IcqCmdBase + 2;
//const unsigned long CmdCheckInvisibleAll	= IcqCmdBase + 3;
//const unsigned long CmdCheckInvisible	= IcqCmdBase + 4;
const unsigned long CmdIcqSendMessage	= IcqCmdBase + 5;
const unsigned long CmdShowWarning		= IcqCmdBase + 6;
const unsigned long CmdPasswordFail		= IcqCmdBase + 7;

const unsigned long MenuSearchResult		= IcqCmdBase;
//const unsigned long MenuCheckInvisible	= IcqCmdBase + 1;
const unsigned long MenuIcqGroups		= IcqCmdBase + 2;

class ICQProtocol : public Protocol
{
public:
    ICQProtocol(Plugin *plugin);
    ~ICQProtocol();
    Client	*createClient(Buffer *cfg);
    const CommandDef *description();
    const CommandDef *statusList();
    static const CommandDef *_statusList();
    virtual const DataDef *userDataDef();
    static const DataDef *icqUserData;
};

class AIMProtocol : public Protocol
{
public:
    AIMProtocol(Plugin *plugin);
    ~AIMProtocol();
    Client	*createClient(Buffer *cfg);
    const CommandDef *description();
    const CommandDef *statusList();
    virtual const DataDef *userDataDef();
    static const DataDef *icqUserData;
};

class ICQPlugin : public Plugin
{
public:
    ICQPlugin(unsigned base);
    virtual ~ICQPlugin();
    unsigned OscarPacket;
    unsigned ICQDirectPacket;
    unsigned AIMDirectPacket;
    unsigned RetrySendDND;
    unsigned RetrySendOccupied;
    static Protocol *m_icq;
    static Protocol *m_aim;
    static ICQPlugin  *icq_plugin;
    static CorePlugin *core;
    void registerMessages();
    void unregisterMessages();
};

#endif
