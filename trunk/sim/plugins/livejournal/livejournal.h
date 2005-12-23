/***************************************************************************
                          livejournal.h  -  description
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

#ifndef _LIVEJOURNAL_H
#define _LIVEJOURNAL_H

#include "simapi.h"
#include "stl.h"
#include "buffer.h"
#include "socket.h"
#include "fetch.h"

const unsigned long JournalCmdBase			= 0x00070000;
const unsigned long MessageJournal			= JournalCmdBase;
const unsigned long MessageUpdated			= JournalCmdBase + 1;
const unsigned long CmdDeleteJournalMessage	= JournalCmdBase + 2;
const unsigned long CmdMenuWeb				= JournalCmdBase + 3;
const unsigned long MenuWeb					= JournalCmdBase + 0x10;

const unsigned LIVEJOURNAL_SIGN	= 5;

const unsigned COMMENT_ENABLE	= 0;
const unsigned COMMENT_NO_MAIL	= 1;
const unsigned COMMENT_DISABLE	= 2;

typedef struct LiveJournalUserData
{
    clientData		base;
    Data	User;
    Data	Shared;
    Data	bChecked;
} LiveJournalUserData;

typedef struct JournalMessageData
{
    Data	Subject;
    Data	Private;
    Data	Time;
    Data	ID;
    Data	OldID;
    Data	Mood;
    Data	Comments;
} JournalMessageData;

class JournalMessage : public Message
{
public:
    JournalMessage(Buffer *cfg = NULL);
    ~JournalMessage();
    string save();
    PROP_UTF8(Subject);
    PROP_ULONG(Private);
    PROP_ULONG(Time);
    PROP_ULONG(ID);
    PROP_ULONG(OldID);
    PROP_ULONG(Mood);
    PROP_ULONG(Comments);
protected:
    QString presentation();
    JournalMessageData data;
};

class CorePlugin;

class LiveJournalPlugin : public Plugin
{
public:
    LiveJournalPlugin(unsigned);
    virtual ~LiveJournalPlugin();
    static CorePlugin *core;
    static unsigned MenuCount;
protected:
    Protocol *m_protocol;
};

class LiveJournalProtocol : public Protocol
{
public:
    LiveJournalProtocol(Plugin *plugin);
    ~LiveJournalProtocol();
    Client	*createClient(Buffer *cfg);
    const CommandDef *description();
    const CommandDef *statusList();
    const DataDef *userDataDef();
};

typedef struct LiveJournalClientData
{
    Data	Server;
    Data	URL;
    Data	Port;
    Data	Interval;
    Data	Mood;
    Data	Moods;
    Data	Menu;
    Data	MenuUrl;
    Data	FastServer;
    Data	LastUpdate;
    LiveJournalUserData	owner;
} LiveJournalClientData;

class LiveJournalClient;

class LiveJournalRequest
{
public:
    LiveJournalRequest(LiveJournalClient *client, const char *mode);
    virtual ~LiveJournalRequest();
    void addParam(const char *key, const char *value);
    void result(Buffer*);
    virtual void result(const char *key, const char *value) = 0;
protected:
    LiveJournalClient *m_client;
    Buffer *m_buffer;
    bool getLine(Buffer *b, string &line);
    friend class LiveJournalClient;
};

class QTimer;

class LiveJournalClient : public TCPClient, public FetchClient
{
    Q_OBJECT
public:
    LiveJournalClient(Protocol*, Buffer *cfg);
    ~LiveJournalClient();
    PROP_STR(Server);
    PROP_STR(URL);
    PROP_USHORT(Port);
    PROP_ULONG(Interval);
    PROP_STRLIST(Mood);
    PROP_ULONG(Moods);
    PROP_STRLIST(Menu);
    PROP_STRLIST(MenuUrl);
    PROP_BOOL(FastServer);
    PROP_STR(LastUpdate);
    void auth_fail(const char *err);
    void auth_ok();
    LiveJournalUserData	*findContact(const char *user, Contact *&contact, bool bCreate=true, bool bJoin=true);
    QTimer  *m_timer;
    virtual bool error_state(const char *err, unsigned code);
    bool add(const char *name);
public slots:
    void timeout();
    void send();
    void messageUpdated();
protected:
    virtual bool done(unsigned code, Buffer &data, const char *headers);
    virtual string getConfig();
    virtual string name();
    virtual string dataName(void*);
    virtual QWidget	*setupWnd();
    virtual bool isMyData(clientData*&, Contact*&);
    virtual bool createData(clientData*&, Contact*);
    virtual void setupContact(Contact*, void *data);
    virtual bool send(Message*, void *data);
    virtual bool canSend(unsigned type, void *data);
    virtual void setStatus(unsigned status);
    virtual void socketConnect();
    virtual void disconnected();
    virtual void packet_ready();
    virtual void *processEvent(Event*);
    virtual void contactInfo(void*, unsigned long &curStatus, unsigned&, const char *&statusIcon, string *icons);
    QWidget *searchWindow(QWidget *parent);
    CommandDef *configWindows();
    QWidget *configWindow(QWidget *parent, unsigned id);
    void statusChanged();
    list<LiveJournalRequest*> m_requests;
    LiveJournalRequest		  *m_request;
    LiveJournalClientData	data;
    friend class LiveJournalCfg;
    friend class LiveJournalRequest;
};

#endif

