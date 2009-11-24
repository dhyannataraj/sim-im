/***************************************************************************
                          icq.cpp  -  description
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

#include "icq.h"
#include "icqconfig.h"
#include "core.h"
#include "moc_core.cpp"

#include "contacts/protocolmanager.h"

using namespace SIM;

Plugin *createICQPlugin(unsigned base, bool, Buffer*)
{
    Plugin *plugin = new ICQPlugin(base);
    return plugin;
}

static PluginInfo info =
    {
        NULL,
        NULL,
        VERSION,
        createICQPlugin,
        PLUGIN_PROTOCOL
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

ICQProtocol::ICQProtocol(Plugin *plugin)
        : Protocol(plugin)
{
}

ICQProtocol::~ICQProtocol()
{
}

Client *ICQProtocol::createClient(Buffer *cfg)
{
    return new ICQClient(this, cfg, false);
}

static CommandDef icq_descr =
    CommandDef (
        0,
        I18N_NOOP("ICQ"),
        "ICQ_online",
        "ICQ_invisible",
        "http://www.icq.com/password/",
        0,
        0,
        0,
        0,
        0,
        PROTOCOL_INFO | PROTOCOL_SEARCH | PROTOCOL_INVISIBLE | PROTOCOL_AR_USER | PROTOCOL_ANY_PORT | PROTOCOL_NODATA,
        NULL,
        QString::null
    );

const CommandDef *ICQProtocol::description()
{
    return &icq_descr;
}

static CommandDef icq_status_list[] =
    {
        CommandDef (
            STATUS_ONLINE,
            I18N_NOOP("Online"),
            "ICQ_online",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_AWAY,
            I18N_NOOP("Away"),
            "ICQ_away",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_NA,
            I18N_NOOP("N/A"),
            "ICQ_na",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_DND,
            I18N_NOOP("Do not Disturb"),
            "ICQ_dnd",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_OCCUPIED,
            I18N_NOOP("Occupied"),
            "ICQ_occupied",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_FFC,
            I18N_NOOP("Free for chat"),
            "ICQ_ffc",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_OFFLINE,
            I18N_NOOP("Offline"),
            "ICQ_offline",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef ()
    };

const CommandDef *ICQProtocol::statusList()
{
    return icq_status_list;
}

const CommandDef *ICQProtocol::_statusList()
{
    return icq_status_list;
}

AIMProtocol::AIMProtocol(Plugin *plugin)
        : Protocol(plugin)
{
}

AIMProtocol::~AIMProtocol()
{
}

Client *AIMProtocol::createClient(Buffer *cfg)
{
    return new ICQClient(this, cfg, true);
}

static CommandDef aim_descr =
    CommandDef (
        0,
        I18N_NOOP("AIM"),
        "AIM_online",
        QString::null,
        "http://www.aim.com/help_faq/forgot_password/password.adp",
        0,
        0,
        0,
        0,
        0,
        PROTOCOL_INFO | PROTOCOL_AR | PROTOCOL_ANY_PORT,
        NULL,
        QString::null
    );

const CommandDef *AIMProtocol::description()
{
    return &aim_descr;
}

static CommandDef aim_status_list[] =
    {
        CommandDef (
            STATUS_ONLINE,
            I18N_NOOP("Online"),
            "AIM_online",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_AWAY,
            I18N_NOOP("Away"),
            "AIM_away",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef (
            STATUS_OFFLINE,
            I18N_NOOP("Offline"),
            "AIM_offline",
            QString::null,
            QString::null,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            QString::null
        ),
        CommandDef ()
    };

const CommandDef *AIMProtocol::statusList()
{
    return aim_status_list;
}

//Protocol *ICQPlugin::m_icq = NULL;
//Protocol *ICQPlugin::m_aim = NULL;

ICQPlugin *ICQPlugin::icq_plugin = NULL;

ICQPlugin::ICQPlugin(unsigned base)
        : Plugin(base)
{
    icq_plugin = this;

    OscarPacket = registerType();
    getContacts()->addPacketType(OscarPacket, "Oscar");
    ICQDirectPacket = registerType();
    getContacts()->addPacketType(ICQDirectPacket, "ICQ.Direct");
    AIMDirectPacket = registerType();
    getContacts()->addPacketType(AIMDirectPacket, "AIM.Direct");

    m_icq = ProtocolPtr(new ICQProtocol(this));
	getProtocolManager()->addProtocol(m_icq);
    m_aim = ProtocolPtr(new AIMProtocol(this));
	getProtocolManager()->addProtocol(m_aim);

    EventMenu(MenuSearchResult, EventMenu::eAdd).process();
    EventMenu(MenuIcqGroups, EventMenu::eAdd).process();

    Command cmd;
    cmd->id          = CmdVisibleList;
    cmd->text        = I18N_NOOP("Visible list");
    cmd->menu_id     = MenuContactGroup;
    cmd->menu_grp    = 0x8010;
    cmd->flags		 = COMMAND_CHECK_STATE;
    EventCommandCreate(cmd).process();

    cmd->id			 = CmdInvisibleList;
    cmd->text		 = I18N_NOOP("Invisible list");
    cmd->menu_grp	 = 0x8011;
    EventCommandCreate(cmd).process();

    cmd->id			 = CmdIcqSendMessage;
    cmd->text		 = I18N_NOOP("&Message");
    cmd->icon		 = "message";
    cmd->menu_id	 = MenuSearchResult;
    cmd->menu_grp	 = 0x1000;
    cmd->bar_id		 = 0;
    cmd->popup_id	 = 0;
    cmd->flags		 = COMMAND_DEFAULT;
    EventCommandCreate(cmd).process();

    cmd->id			 = CmdInfo;
    cmd->text		 = I18N_NOOP("User &info");
    cmd->icon		 = "info";
    cmd->menu_grp	 = 0x1001;
    EventCommandCreate(cmd).process();

    cmd->id			 = CmdGroups;
    cmd->text		 = I18N_NOOP("&Add to group");
    cmd->icon		 = QString::null;
    cmd->menu_grp	 = 0x1002;
    cmd->popup_id	 = MenuIcqGroups;
    EventCommandCreate(cmd).process();

    cmd->id			 = CmdGroups;
    cmd->text		 = "_";
    cmd->menu_id	 = MenuIcqGroups;
    cmd->flags		 = COMMAND_CHECK_STATE;
    EventCommandCreate(cmd).process();

    registerMessages();

    RetrySendDND = registerType();
    RetrySendOccupied = registerType();
}

ICQPlugin::~ICQPlugin()
{
    unregisterMessages();
	getProtocolManager()->removeProtocol(m_aim);
	getProtocolManager()->removeProtocol(m_icq);

    getContacts()->removePacketType(OscarPacket);
    getContacts()->removePacketType(ICQDirectPacket);
    getContacts()->removePacketType(AIMDirectPacket);

    EventCommandRemove(CmdVisibleList).process();
    EventCommandRemove(CmdInvisibleList).process();

    EventMenu(MenuSearchResult, EventMenu::eRemove).process();
    EventMenu(MenuIcqGroups, EventMenu::eRemove).process();
}
