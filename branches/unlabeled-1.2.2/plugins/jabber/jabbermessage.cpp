/***************************************************************************
                          jabbermessage.cpp  -  description
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

#include "jabbermessage.h"
#include "jabber.h"
#include "core.h"

static DataDef jabberMessageData[] =
    {
        { "Subject", DATA_UTF, 1, 0 },
        { NULL, 0, 0, 0 }
    };

JabberMessage::JabberMessage(const char *cfg)
        : Message(MessageJabber, cfg)
{
    load_data(jabberMessageData, &data, cfg);
}

JabberMessage::~JabberMessage()
{
    free_data(jabberMessageData, &data);
}

string JabberMessage::save()
{
    string res = Message::save();
    string s = save_data(jabberMessageData, &data);
    if (!s.empty()){
        if (!res.empty())
            res += "\n";
        res += s;
    }
    return res;
}

QString JabberMessage::presentation()
{
    QString res = i18n("<p>Subject: %1</p>")
                  .arg(getSubject());
    res += Message::presentation();
    return res;
}

static Message *createJabberMessage(const char *cfg)
{
    return new JabberMessage(cfg);
}

static MessageDef defJabber =
    {
        NULL,
        MESSAGE_DEFAULT,
        MessageGeneric,
        NULL,
        NULL,
        createJabberMessage,
        NULL,
        NULL,
        NULL
    };

static Message *createJabberOnlineMessage(const char *cfg)
{
    return new AuthMessage(MessageJabberOnline, cfg);
}

static MessageDef defJabberOnline =
    {
        NULL,
        MESSAGE_HIDDEN,
        0,
        I18N_NOOP("Log On"),
        NULL,
        createJabberOnlineMessage,
        NULL,
        NULL,
        NULL
    };

static Message *createJabberOfflineMessage(const char *cfg)
{
    return new AuthMessage(MessageJabberOffline, cfg);
}

static MessageDef defJabberOffline =
    {
        NULL,
        MESSAGE_HIDDEN,
        0,
        I18N_NOOP("Log Off"),
        NULL,
        createJabberOfflineMessage,
        NULL,
        NULL,
        NULL
    };

void JabberPlugin::registerMessages()
{
    Command cmd;
    cmd->id			 = MessageJabber;
    cmd->text		 = "Jabber";
    cmd->icon		 = "message";
    cmd->flags		 = COMMAND_DEFAULT;
    cmd->param		 = &defJabber;
    Event eMsg(EventCreateMessageType, cmd);
    eMsg.process();

    cmd->id			 = MessageJabberOnline;
    cmd->text		 = I18N_NOOP("Log On");
    cmd->icon		 = "Jabber_online";
    cmd->accel		 = "Ctrl+L";
    cmd->menu_grp	 = 0x3020;
    cmd->param		 = &defJabberOnline;
    Event eMsgOnline(EventCreateMessageType, cmd);
    eMsg.process();

    cmd->id			 = MessageJabberOffline;
    cmd->text		 = I18N_NOOP("Log Off");
    cmd->icon		 = "Jabber_offline";
    cmd->param		 = &defJabberOffline;
    Event eMsgOffline(EventCreateMessageType, cmd);
    eMsg.process();
}

void JabberPlugin::unregisterMessages()
{
    Event eMsg(EventRemoveMessageType, (void*)MessageJabber);
    eMsg.process();
}

