/***************************************************************************
                          msn.cpp  -  description
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

#include "msnclient.h"
#include "msnconfig.h"
#include "msnpacket.h"
#include "msn.h"
#include "msninfo.h"
#include "msnsearch.h"
#include "core.h"

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifndef INADDR_NONE
#define INADDR_NONE     0xFFFFFFFF
#endif

#include <time.h>
#include <qtimer.h>
#include <qregexp.h>

#include <qfile.h>

using namespace std;

const unsigned long PING_TIMEOUT	= 60;

const unsigned long FT_TIMEOUT		= 60;
const unsigned long TYPING_TIME		= 10;
const unsigned MAX_FT_PACKET		= 2045;

/*
typedef struct MSNUserData
{
    char		*EMail;
    char		*ScreenName;
    unsigned	Status;
    unsigned	StatusTime;
    unsigned	OnlineTime;
	char		*PhoneHome;
	char		*PhoneWork;
	char		*PhoneMobile;
	unsigned	Mobile;
	unsigned	Group;
	unsigned	Flags;
	unsigned	sFlags;
} MSNUserData;
*/

using namespace SIM;

static DataDef msnUserData[] =
    {
        { "", DATA_ULONG, 1, DATA(3) },		// Sign
        { "LastSend", DATA_ULONG, 1, 0 },
        { "EMail", DATA_UTF, 1, 0 },
        { "Screen", DATA_UTF, 1, 0 },
        { "", DATA_ULONG, 1, DATA(1) },	// Status
        { "StatusTime", DATA_ULONG, 1, 0 },
        { "OnlineTime", DATA_ULONG, 1, 0 },
        { "PhoneHome", DATA_UTF, 1, 0 },
        { "PhoneWork", DATA_UTF, 1, 0 },
        { "PhoneMobile", DATA_UTF, 1, 0 },
        { "Mobile", DATA_BOOL, 1, 0 },
        { "Group", DATA_ULONG, 1, 0 },
        { "Flags", DATA_ULONG, 1, 0 },
        { "", DATA_ULONG, 1, 0 },				// sFlags
        { "", DATA_ULONG, 1, 0 },
        { "IP", DATA_IP, 1, 0 },
        { "RealIP", DATA_IP, 1, 0 },
        { "Port", DATA_ULONG, 1, 0 },
        { "", DATA_OBJECT, 1, 0 },				// sb
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

/*
typedef struct MSNClientData
{
    char		*Server;
    unsigned	Port;
	MSNUserData	owner;
} MSNClientData;
*/
static DataDef msnClientData[] =
    {
        { "Server", DATA_STRING, 1, "messenger.hotmail.com" },
        { "Port", DATA_ULONG, 1, DATA(1863) },
        { "ListVer", DATA_ULONG, 1, 0 },
        { "ListRequests", DATA_UTF, 1, 0 },
        { "Version", DATA_STRING, 1, "5.0.0540" },
        { "MinPort", DATA_ULONG, 1, DATA(1024) },
        { "MaxPort", DATA_ULONG, 1, DATA(0xFFFF) },
        { "UseHTTP", DATA_BOOL, 1, 0 },
        { "AutoHTTP", DATA_BOOL, 1, DATA(1) },
        { "Deleted", DATA_STRLIST, 1, 0 },
        { "NDeleted", DATA_ULONG, 1, 0 },
        { "AutoAuth", DATA_BOOL, 1, DATA(1) },
        { "", DATA_STRUCT, sizeof(MSNUserData) / sizeof(Data), DATA(msnUserData) },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

MSNClient::MSNClient(Protocol *protocol, ConfigBuffer *cfg)
        : TCPClient(protocol, cfg)
{
    load_data(msnClientData, &data, cfg);
    m_packetId  = 1;
    m_msg       = NULL;
    m_bFirst    = (cfg == NULL);
    QString s = getListRequests();
    while (!s.isEmpty()){
        QString item = getToken(s, ';');
        MSNListRequest lr;
        lr.Type = getToken(item, ',').toUInt();
        lr.Name = item;
    }
    setListRequests("");
    m_bJoin = false;
    m_bFirstTry = false;
}

MSNClient::~MSNClient()
{
    TCPClient::setStatus(STATUS_OFFLINE, false);
    free_data(msnClientData, &data);
    freeData();
}

QString MSNClient::name()
{
    return "MSN." + getLogin();
}

QWidget	*MSNClient::setupWnd()
{
    return new MSNConfig(NULL, this, false);
}

QString MSNClient::getConfig()
{
    QString listRequests;
    for (list<MSNListRequest>::iterator it = m_requests.begin(); it != m_requests.end(); ++it){
        if (!listRequests.isEmpty())
            listRequests += ";";
        listRequests += QString::number((*it).Type) + "," + (*it).Name;
    }
    setListRequests(listRequests);
    QString res = Client::getConfig();
    if (res.length())
        res += "\n";
    res += save_data(msnClientData, &data);
    setListRequests("");
    return res;
}

const DataDef *MSNProtocol::userDataDef()
{
    return msnUserData;
}

void MSNClient::connect_ready()
{
    m_bFirstTry = false;
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    log(L_DEBUG, "Connect ready");
    TCPClient::connect_ready();
    MSNPacket *packet = new VerPacket(this);
    packet->send();
}

void MSNClient::setStatus(unsigned status)
{
    if (status  == m_status)
        return;
    time_t now = time(NULL);
    if (m_status == STATUS_OFFLINE)
        data.owner.OnlineTime.asULong() = now;
    data.owner.StatusTime.asULong() = now;
    m_status = status;
    data.owner.Status.asULong() = m_status;
    Event e(EventClientChanged, static_cast<Client*>(this));
    e.process();
    if (status == STATUS_OFFLINE){
        if (m_status != STATUS_OFFLINE){
            m_status = status;
            data.owner.Status.asULong() = status;
            data.owner.StatusTime.asULong() = time(NULL);
            MSNPacket *packet = new OutPacket(this);
            packet->send();
        }
        return;
    }
    if (Client::m_state != Connected){
        m_logonStatus = status;
        return;
    }
    m_status = status;
    MSNPacket *packet = new ChgPacket(this);
    packet->send();
}

void MSNClient::connected()
{
    setState(Client::Connected);
    setStatus(m_logonStatus);
    processRequests();
}

void MSNClient::setInvisible(bool bState)
{
    if (bState == getInvisible())
        return;
    TCPClient::setInvisible(bState);
    if (getStatus() == STATUS_OFFLINE)
        return;
    MSNPacket *packet = new ChgPacket(this);
    packet->send();
}

void MSNClient::disconnected()
{
    stop();
    Contact *contact;
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        bool bChanged = false;
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            if (data->Status.toULong() != STATUS_OFFLINE){
                data->Status.asULong() = STATUS_OFFLINE;
                data->StatusTime.asULong() = time(NULL);
                SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
                if (sock){
                    delete sock;
                    data->sb.clear();
                }
                bChanged = true;
            }
            if (bChanged){
                StatusMessage m;
                m.setContact(contact->id());
                m.setClient(dataName(data));
                m.setFlags(MESSAGE_RECEIVED);
                m.setStatus(STATUS_OFFLINE);
                Event e(EventMessageReceived, &m);
                e.process();
            }
        }
    }
    m_packetId = 0;
    m_pingTime = 0;
    m_state    = None;
    m_authChallenge = "";
    clearPackets();
}

void MSNClient::clearPackets()
{
    if (m_msg){
        delete m_msg;
        m_msg = NULL;
    }
    for (list<MSNPacket*>::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
        delete *it;
    }
    m_packets.clear();
}

void MSNClient::packet_ready()
{
    if (m_socket->readBuffer.writePos() == 0)
        return;
    MSNPlugin *plugin = static_cast<MSNPlugin*>(protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->MSNPacket);
    if (m_msg){
        if (!m_msg->packet())
            return;
        delete m_msg;
        m_msg = NULL;
    }
    for (;;){
        QCString s;
        if (!m_socket->readBuffer.scan("\r\n", s))
            break;
        getLine(s);
    }
    if (m_socket->readBuffer.readPos() == m_socket->readBuffer.writePos())
        m_socket->readBuffer.init(0);
}

typedef struct statusText
{
    unsigned	status;
    const char	*name;
} statusText;

statusText st[] =
    {
        { STATUS_ONLINE, "NLN" },
        { STATUS_OFFLINE, "FLN" },
        { STATUS_OFFLINE, "HDN" },
        { STATUS_NA, "IDL" },
        { STATUS_AWAY, "AWY" },
        { STATUS_DND, "BSY" },
        { STATUS_BRB, "BRB" },
        { STATUS_PHONE, "PHN" },
        { STATUS_LUNCH, "LUN" },
        { 0, NULL }
    };

static unsigned str2status(const char *str)
{
    for (const statusText *s = st; s->name; s++){
        if (!strcmp(str, s->name))
            return s->status;
    }
    return STATUS_OFFLINE;
}

void MSNClient::processLSG(unsigned id, const QString &name)
{
    if (id == 0)
        return;
    Group *grp;
    MSNListRequest *lr = findRequest(id, LR_GROUPxREMOVED);
    if (lr)
        return;
    MSNUserData *data = findGroup(id, QString::null, grp);
    if (data){
        lr = findRequest(grp->id(), LR_GROUPxCHANGED);
        if (lr){
            data->sFlags.asULong() |= MSN_CHECKED;
            return;
        }
    }
    data = findGroup(id, name, grp);
    data->sFlags.asULong() |= MSN_CHECKED;
}

void MSNClient::processLST(const QString &mail, const QString &name, unsigned state, unsigned grp)
{
    if ((state & MSN_FORWARD) == 0){
        for (unsigned i = 1; i <= getNDeleted(); i++){
            if (getDeleted(i) == mail)
                return;
        }
    }

    m_curBuddy = mail;
    Contact *contact;
    MSNListRequest *lr = findRequest(mail, LR_CONTACTxREMOVED);
    if (lr)
        return;
    bool bNew = false;
    MSNUserData *data = findContact(mail, contact);
    if (data == NULL){
        data = findContact(mail, name, contact);
        bNew = true;
    }else{
        data->EMail.str() = mail;
        data->ScreenName.str() = name;
        if (data->ScreenName.str() != contact->getName())
            contact->setName(name);
    }
    data->sFlags.asULong() |= MSN_CHECKED;
    data->Flags.asULong() = state;
    if (state & MSN_BLOCKED)
        contact->setIgnore(true);

    lr = findRequest(mail, LR_CONTACTxCHANGED);
    data->Group.asULong() = grp;
    data->PhoneHome.clear();
    data->PhoneWork.clear();
    data->PhoneMobile.clear();
    data->Mobile.asBool() = false;
    Group *group = NULL;
    if ((grp == 0) || (grp == NO_GROUP)){
        group = getContacts()->group(0);
    }else{
        findGroup(grp, QString::null, group);
    }
    if (lr == NULL){
        bool bChanged = ((data->Flags.toULong() & MSN_FLAGS) != (data->sFlags.toULong() & MSN_FLAGS));
        if (getAutoAuth() && (data->Flags.toULong() & MSN_FORWARD) && ((data->Flags.toULong() & MSN_ACCEPT) == 0) && ((data->Flags.toULong() & MSN_BLOCKED) == 0))
            bChanged = true;
        unsigned grp = 0;
        if (group)
            grp = group->id();
        if (grp != contact->getGroup())
            bChanged = true;
        if (bChanged){
            MSNListRequest lr;
            lr.Type = LR_CONTACTxCHANGED;
            lr.Name = data->EMail.str();
            m_requests.push_back(lr);
        }
        if (data->Flags.toULong() & MSN_FORWARD)
            contact->setGroup(grp);
    }
}

void MSNClient::checkEndSync()
{
    if (m_nBuddies || m_nGroups)
        return;
    ContactList::GroupIterator itg;
    Group *grp;
    list<Group*>	grpRemove;
    list<Contact*>	contactRemove;
    while ((grp = ++itg) != NULL){
        ClientDataIterator it(grp->clientData, this);
        MSNUserData *data = (MSNUserData*)(++it);
        if (grp->id() && (data == NULL)){
            MSNListRequest lr;
            lr.Type = LR_GROUPxCHANGED;
            lr.Name = QString::number(grp->id());
            m_requests.push_back(lr);
            continue;
        }
        if (data == NULL)
            continue;
        if ((data->sFlags.toULong() & MSN_CHECKED) == 0)
            grpRemove.push_back(grp);
    }
    Contact *contact;
    ContactList::ContactIterator itc;
    while ((contact = ++itc) != NULL){
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        list<void*> forRemove;
        while ((data = (MSNUserData*)(++it)) != NULL){
            if (data->sFlags.toULong() & MSN_CHECKED){
                if ((data->sFlags.toULong() & MSN_REVERSE) && ((data->Flags.toULong() & MSN_REVERSE) == 0))
                    auth_message(contact, MessageRemoved, data);
                if (!m_bFirst && ((data->sFlags.toULong() & MSN_REVERSE) == 0) && (data->Flags.toULong() & MSN_REVERSE)){
                    if ((data->Flags.toULong() & MSN_ACCEPT) || getAutoAuth()){
                        auth_message(contact, MessageAdded, data);
                    }else{
                        auth_message(contact, MessageAuthRequest, data);
                    }
                }
                setupContact(contact, data);
                Event e(EventContactChanged, contact);
                e.process();
            }else{
                forRemove.push_back(data);
            }
        }
        if (forRemove.empty())
            continue;
        for (list<void*>::iterator itr = forRemove.begin(); itr != forRemove.end(); ++itr)
            contact->clientData.freeData(*itr);
        if (contact->clientData.size() == 0)
            contactRemove.push_back(contact);
    }
    for (list<Contact*>::iterator rc = contactRemove.begin(); rc != contactRemove.end(); ++rc)
        delete *rc;
    for (list<Group*>::iterator rg = grpRemove.begin(); rg != grpRemove.end(); ++rg)
        delete *rg;
    if (m_bJoin){
        Event e(EventJoinAlert, this);
        e.process();
    }
    m_bFirst = false;
    connected();
}

static unsigned toInt(const QString &str)
{
    if (str.isEmpty())
        return 0;
    return str.toLong();
}

void MSNClient::getLine(const QCString &line)
{
    QString l = QString::fromUtf8(line);
    l = l.replace(QRegExp("\r"), "");
    QCString ll = l.local8Bit();
    log(L_DEBUG, "Get: %s", (const char*)ll);
    QString cmd = getToken(l, ' ');
    if ((cmd == "715") || (cmd == "228"))
        return;
    if (cmd == "XFR"){
        QString id   = getToken(l, ' ');	// ID
        QString type = getToken(l, ' ');	// NS
        if (type == "NS"){
            l = getToken(l, ' ');				// from
            QString host = getToken(l, ':');
            unsigned short port = l.toUShort();
            if (host.isEmpty() || (port == 0)){
                log(L_WARN, "Bad host on XFR");
                m_socket->error_state(I18N_NOOP("MSN protocol error"));
                return;
            }
            clearPackets();
            m_socket->close();
            m_socket->readBuffer.init(0);
            m_socket->connect(host, port, this);
            return;
        }
        l = id + " " + type + " " + l;
    }
    if (cmd == "MSG"){
        getToken(l, ' ');
        getToken(l, ' ');
        unsigned size = getToken(l, ' ').toUInt();
        if (size == 0){
            log(L_WARN, "Bad server message size");
            m_socket->error_state(I18N_NOOP("MSN protocol error"));
            return;
        }
        m_msg = new MSNServerMessage(this, size);
        packet_ready();
        return;
    }
    if (cmd == "CHL"){
        getToken(l, ' ');
        MSNPacket *packet = new QryPacket(this, getToken(l, ' '));
        packet->send();
        return;
    }
    if (cmd == "QNG")
        return;
    if (cmd == "BPR"){
        Contact *contact;
        MSNUserData *data = findContact(getToken(l, ' '), contact);
        if (data){
            QString info = getToken(l, ' ');
            QString type = getToken(l, ' ');
            bool bChanged = false;
            if (type == "PHH"){
                bChanged = data->PhoneHome.setStr(unquote(info));
            }else if (type == "PHW"){
                bChanged = data->PhoneWork.setStr(unquote(info));
            }else if (type == "PHM"){
                bChanged = data->PhoneMobile.setStr(unquote(info));
            }else if (type == "MOB"){
                data->Mobile.asBool() = ((info[0] == 'Y') != 0);
            }else{
                log(L_DEBUG, "Unknown BPR type %s", type.latin1());
            }
            if (bChanged){
                setupContact(contact, data);
                Event e(EventContactChanged, contact);
                e.process();
            }
        }
        return;
    }
    if (cmd == "ILN"){
        getToken(l, ' ');
        unsigned status = str2status(getToken(l, ' '));
        Contact *contact;
        MSNUserData *data = findContact(getToken(l, ' '), contact);
        if (data && (data->Status.toULong() != status)){
            time_t now = time(NULL);
            data->Status.asULong() = status;
            if (data->Status.toULong() == STATUS_OFFLINE){
                data->OnlineTime.asULong() = now;
                set_ip(&data->IP, 0);
                set_ip(&data->RealIP, 0);
            }
            data->StatusTime.asULong() = now;
            StatusMessage m;
            m.setContact(contact->id());
            m.setClient(dataName(data));
            m.setFlags(MESSAGE_RECEIVED);
            m.setStatus(status);
            Event e(EventMessageReceived, &m);
            e.process();
        }
        return;
    }
    if (cmd == "NLN"){
        unsigned status = str2status(getToken(l, ' '));
        Contact *contact;
        MSNUserData *data = findContact(getToken(l, ' '), contact);
        if (data && (data->Status.toULong() != status)){
            time_t now = time(NULL);
            if (data->Status.toULong() == STATUS_OFFLINE){
                data->OnlineTime.asULong() = now;
                set_ip(&data->IP, 0);
                set_ip(&data->RealIP, 0);
            }
            data->StatusTime.asULong() = now;
            data->Status.asULong() = status;
            StatusMessage m;
            m.setContact(contact->id());
            m.setClient(dataName(data));
            m.setFlags(MESSAGE_RECEIVED);
            m.setStatus(status);
            Event e(EventMessageReceived, &m);
            e.process();
            if ((status == STATUS_ONLINE) && !contact->getIgnore()){
                Event e(EventContactOnline, contact);
                e.process();
            }
        }
        return;
    }
    if (cmd == "FLN"){
        Contact *contact;
        MSNUserData *data = findContact(getToken(l, ' '), contact);
        if (data && (data->Status.toULong() != STATUS_OFFLINE)){
            data->StatusTime.asULong() = time(NULL);
            data->Status.asULong() = STATUS_OFFLINE;
            StatusMessage m;
            m.setContact(contact->id());
            m.setClient(dataName(data));
            m.setFlags(MESSAGE_RECEIVED);
            m.setStatus(STATUS_OFFLINE);
            Event e(EventMessageReceived, &m);
            e.process();
        }
        return;
    }
    if (cmd == "ADD"){
        getToken(l, ' ');
        if (getToken(l, ' ') == "RL"){
            setListVer(getToken(l, ' ').toUInt());
            Contact *contact;
            MSNUserData *data = findContact(getToken(l, ' '), getToken(l, ' '), contact);
            if (data){
                data->Flags.asULong() |= MSN_REVERSE;
                if ((data->Flags.toULong() & MSN_ACCEPT) || getAutoAuth()){
                    auth_message(contact, MessageAdded, data);
                }else{
                    auth_message(contact, MessageAuthRequest, data);
                }
            }
        }
        return;
    }
    if (cmd == "REM"){
        getToken(l, ' ');
        if (getToken(l, ' ') == "RL"){
            setListVer(getToken(l, ' ').toUInt());
            Contact *contact;
            MSNUserData *data = findContact(getToken(l, ' '), contact);
            if (data){
                data->Flags.asULong() &= ~MSN_REVERSE;
                auth_message(contact, MessageRemoved, data);
            }
        }
        return;
    }
    if (cmd == "RNG"){
        QString session = getToken(l, ' ');
        QString addr = getToken(l, ' ');
        getToken(l, ' ');
        QString cookie, email, nick;
        cookie = getToken(l, ' ');
        email  = getToken(l, ' ');
        nick   = getToken(l, ' ');
        Contact *contact;
        MSNUserData *data = findContact(email, contact);
        if (data == NULL){
            data = findContact(email, nick, contact);
            contact->setFlags(CONTACT_TEMP);
            Event e(EventContactChanged, contact);
            e.process();
        }
        SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
        if (sock){
            delete sock;
        }
        sock = new SBSocket(this, contact, data);
        sock->connect(addr, session, cookie, false);
        data->sb.setObject(sock);
        return;
    }
    if (cmd == "OUT"){
        m_reconnect = NO_RECONNECT;
        m_socket->error_state(I18N_NOOP("Your account is being used from another location"));
        return;
    }
    if (cmd == "GTC")
        return;
    if (cmd == "BLP")
        return;
    if (cmd == "LSG"){
        unsigned id = toInt(getToken(l, ' '));
        processLSG(id, unquote(getToken(l, ' ')));
        m_nGroups--;
        checkEndSync();
        return;
    }
    if (cmd == "LST"){
        QString mail = unquote(getToken(l, ' '));
        QString name = unquote(getToken(l, ' '));
        unsigned state = toInt(getToken(l, ' '));
        unsigned grp   = toInt(getToken(l, ' '));
        processLST(mail, name, state, grp);
        m_nBuddies--;
        checkEndSync();
        return;
    }
    if (cmd == "PRP"){
        QString cmd = getToken(l, ' ');
        if (cmd == "PHH")
            data.owner.PhoneHome.str() = unquote(getToken(l, ' '));
        if (cmd == "PHW")
            data.owner.PhoneWork.str() = unquote(getToken(l, ' '));
        if (cmd == "PHM")
            data.owner.PhoneMobile.str() = unquote(getToken(l, ' '));
        if (cmd == "MBE")
            data.owner.Mobile.asBool() = (getToken(l, ' ') == "Y");
        return;
    }
    if (cmd == "BPR"){
        Contact *contact;
        MSNUserData *data = findContact(m_curBuddy, contact);
        if (data == NULL)
            return;
        Event e(EventContactChanged, contact);
        e.process();
        QString cmd = getToken(l, ' ');
        if (cmd == "PHH")
            data->PhoneHome.str() = unquote(getToken(l, ' '));
        if (cmd == "PHW")
            data->PhoneWork.str() = unquote(getToken(l, ' '));
        if (cmd == "PHM")
            data->PhoneMobile.str() = unquote(getToken(l, ' '));
        if (cmd == "MBE")
            data->Mobile.asBool() = (getToken(l, ' ') == "Y");
        return;
    }
    unsigned code = cmd.toUInt();
    if (code){
        MSNPacket *packet = NULL;
        unsigned id = getToken(l, ' ').toUInt();
        list<MSNPacket*>::iterator it;
        for (it = m_packets.begin(); it != m_packets.end(); ++it){
            if ((*it)->id() == id){
                packet = *it;
                break;
            }
        }
        if (it == m_packets.end()){
            m_socket->error_state("Bad packet id");
            return;
        }
        m_packets.erase(it);
        packet->error(code);
        delete packet;
        return;
    }
    if (m_packets.empty()){
        log(L_DEBUG, "Packet not found");
        return;
    }
    MSNPacket *packet = NULL;
    unsigned id = getToken(l, ' ').toUInt();
    list<MSNPacket*>::iterator it;
    for (it = m_packets.begin(); it != m_packets.end(); ++it){
        if ((*it)->id() == id){
            packet = *it;
            break;
        }
    }
    if (it == m_packets.end()){
        m_socket->error_state("Bad packet id");
        return;
    }
    if (cmd != packet->cmd()){
        m_socket->error_state("Bad answer cmd");
        return;
    }
    QValueList<QString> args;
    while (l.length())
        args.append(getToken(l, ' ', false));
    packet->answer(args);
    m_packets.erase(it);
    delete packet;
}

void MSNClient::sendLine(const QString &line, bool crlf)
{
    log(L_DEBUG, "Send: %s", line.local8Bit().data());
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << (const char*)line.utf8();
    if (crlf)
        m_socket->writeBuffer << "\r\n";
    MSNPlugin *plugin = static_cast<MSNPlugin*>(protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->MSNPacket);
    m_socket->write();
}

void MSNClient::authFailed()
{
    m_reconnect = NO_RECONNECT;
    m_socket->error_state(I18N_NOOP("Login failed"), AuthError);
}

void MSNClient::authOk()
{
    m_state    = None;
    m_authChallenge = "";
    m_pingTime = time(NULL);
    QTimer::singleShot(TYPING_TIME * 1000, this, SLOT(ping()));
    setPreviousPassword(NULL);
    MSNPacket *packet = new SynPacket(this);
    packet->send();
}

void MSNClient::ping()
{
    if (getState() != Connected)
        return;
    time_t now = time(NULL);
    if ((unsigned)now >= m_pingTime + PING_TIMEOUT){
        sendLine("PNG");
        m_pingTime = now;
    }
    for (list<SBSocket*>::iterator it = m_SBsockets.begin(); it != m_SBsockets.end(); ++it)
        (*it)->timer(now);
    QTimer::singleShot(TYPING_TIME * 1000, this, SLOT(ping()));
}

QString MSNClient::getLogin()
{
    return data.owner.EMail.str();
}

void MSNClient::setLogin(const QString &str)
{
    data.owner.EMail.str() = str;
}

const unsigned MAIN_INFO = 1;
const unsigned NETWORK	 = 2;

static CommandDef msnWnd[] =
    {
        CommandDef (
            MAIN_INFO,
            " ",
            "MSN_online",
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
        CommandDef (),
    };

static CommandDef cfgMsnWnd[] =
    {
        CommandDef (
            MAIN_INFO,
            " ",
            "MSN_online",
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
            NETWORK,
            I18N_NOOP("Network"),
            "network",
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
        CommandDef (),
    };

CommandDef *MSNClient::infoWindows(Contact*, void *_data)
{
    MSNUserData *data = (MSNUserData*)_data;
    QString name = i18n(protocol()->description()->text);
    name += " ";
    name += data->EMail.str();
    msnWnd[0].text_wrk = name;
    return msnWnd;
}

CommandDef *MSNClient::configWindows()
{
    QString name = i18n(protocol()->description()->text);
    name += " ";
    name += data.owner.EMail.str();
    cfgMsnWnd[0].text_wrk = name;
    return cfgMsnWnd;
}

QWidget *MSNClient::infoWindow(QWidget *parent, Contact*, void *_data, unsigned id)
{
    MSNUserData *data = (MSNUserData*)_data;
    switch (id){
    case MAIN_INFO:
        return new MSNInfo(parent, data, this);
    }
    return NULL;
}

QWidget *MSNClient::configWindow(QWidget *parent, unsigned id)
{
    switch (id){
    case MAIN_INFO:
        return new MSNInfo(parent, NULL, this);
    case NETWORK:
        return new MSNConfig(parent, this, true);
    }
    return NULL;
}

bool MSNClient::canSend(unsigned type, void *_data)
{
    if ((_data == NULL) || (((clientData*)_data)->Sign.toULong() != MSN_SIGN))
        return false;
    MSNUserData *data = (MSNUserData*)_data;
    if (getState() != Connected)
        return false;
    switch (type){
    case MessageGeneric:
    case MessageFile:
    case MessageUrl:
        if (getInvisible())
            return false;
        return true;
    case MessageAuthGranted:
    case MessageAuthRefused:
        return (data->Flags.toULong() & MSN_ACCEPT) == 0;
    }
    return false;
}

bool MSNClient::send(Message *msg, void *_data)
{
    if ((_data == NULL) || (getState() != Connected))
        return false;
    MSNUserData *data = (MSNUserData*)_data;
    MSNPacket *packet;
    switch (msg->type()){
    case MessageAuthGranted:
        if (data->Flags.toULong() & MSN_ACCEPT)
            return false;
        packet = new AddPacket(this, "AL", data->EMail.str(), quote(data->ScreenName.str()), 0);
        packet->send();
    case MessageAuthRefused:
        if (data->Flags.toULong() & MSN_ACCEPT)
            return false;
        if (msg->getText().isEmpty()){
            if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                msg->setClient(dataName(data));
                Event e(EventSent, msg);
                e.process();
            }
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            return true;
        }
    case MessageGeneric:
    case MessageFile:
    case MessageUrl: {
            SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
            if (!sock){
                if (getInvisible())
                    return false;
                Contact *contact;
                findContact(data->EMail.str(), contact);
                sock = new SBSocket(this, contact, data);
                sock->connect();
                data->sb.setObject(sock);
            }
            return sock->send(msg);
        }
    case MessageTypingStart: {
            SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
            if (!sock){
                if (getInvisible())
                    return false;
                Contact *contact;
                findContact(data->EMail.str(), contact);
                sock = new SBSocket(this, contact, data);
                sock->connect();
                data->sb.setObject(sock);
            }
            sock->setTyping(true);
            delete msg;
            return true;
        }
    case MessageTypingStop: {
            SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
            if (!sock)
                return false;
            sock->setTyping(false);
            delete msg;
            return true;
        }
    }
    return false;
}

QString MSNClient::dataName(void *_data)
{
    QString res = name();
    MSNUserData *data = (MSNUserData*)_data;
    res += "+";
    res += data->EMail.str();
    return res;
}

bool MSNClient::isMyData(clientData *&_data, Contact *&contact)
{
    if (_data->Sign.toULong() != MSN_SIGN)
        return false;
    MSNUserData *data = (MSNUserData*)_data;
    if (data->EMail.str().lower() == this->data.owner.EMail.str().lower())
        return false;
    MSNUserData *my_data = findContact(data->EMail.str(), contact);
    if (my_data){
        data = my_data;
    }else{
        contact = NULL;
    }
    return true;
}

bool MSNClient::createData(clientData *&_data, Contact *contact)
{
    MSNUserData *data = (MSNUserData*)_data;
    MSNUserData *new_data = (MSNUserData*)(contact->clientData.createData(this));
    new_data->EMail.str() = data->EMail.str();
    _data = (clientData*)new_data;
    return true;
}

void MSNClient::setupContact(Contact *contact, void *_data)
{
    MSNUserData *data = (MSNUserData*)_data;
    QString phones;
    if (!data->PhoneHome.str().isEmpty()){
        phones += data->PhoneHome.str();
        phones += ",Home Phone,1";
    }
    if (!data->PhoneWork.str().isEmpty()){
        if (!phones.isEmpty())
            phones += ";";
        phones += data->PhoneWork.str();
        phones += ",Work Phone,1";
    }
    if (!data->PhoneMobile.str().isEmpty()){
        if (!phones.isEmpty())
            phones += ";";
        phones += data->PhoneMobile.str();
        phones += ",Private Cellular,2";
    }
    bool bChanged = contact->setPhones(phones, name());
    bChanged |= contact->setEMails(data->EMail.str(), name());
    if (contact->getName().isEmpty()){
        QString name = data->ScreenName.str();
        if (name.isEmpty())
            name = data->EMail.str();
        int n = name.find('@');
        if (n > 0)
            name = name.left(n);
        bChanged |= contact->setName(name);
    }
    if (bChanged){
        Event e(EventContactChanged, contact);
        e.process();
    }
}

QString MSNClient::unquote(const QString &s)
{
    QString res;
    for (int i = 0; i < (int)(s.length()); i++){
        QChar c = s[i];
        if (c != '%'){
            res += c;
            continue;
        }
        i++;
        if (i + 2 > (int)(s.length()))
            break;
        res += QChar((char)((fromHex(s[i++]) << 4) + fromHex(s[i])));
    }
    return res;
}

QString MSNClient::quote(const QString &s)
{
    QString res;
    for (int i = 0; i < (int)(s.length()); i++){
        QChar c = s[i];
        if ((c == '%') || (c == ' ')){
            char b[4];
            sprintf(b, "%%%2X", (unsigned)c);
            res += b;
        }else{
            res += QChar(c);
        }
    }
    return res;
}

MSNUserData *MSNClient::findContact(const QString &mail, Contact *&contact)
{
    ContactList::ContactIterator itc;
    while ((contact = ++itc) != NULL){
        MSNUserData *res;
        ClientDataIterator it(contact->clientData, this);
        while ((res = (MSNUserData*)(++it)) != NULL){
            if (res->EMail.str() == mail)
                return res;
        }
    }
    return NULL;
}

QString MSNClient::contactName(void *clientData)
{
    MSNUserData *data = (MSNUserData*)clientData;
    QString res = "MSN: ";
    res += data->EMail.str();
    return res;
}

MSNUserData *MSNClient::findContact(const QString &mail, const QString &name, Contact *&contact, bool bJoin)
{
    unsigned i;
    for (i = 1; i <= getNDeleted(); i++){
        if (getDeleted(i) == mail)
            break;
    }
    if (i <= getNDeleted()){
        list<string> deleted;
        for (i = 1; i <= getNDeleted(); i++){
            if (getDeleted(i) == mail)
                continue;
            deleted.push_back(getDeleted(i));
        }
        setNDeleted(0);
        for (list<string>::iterator it = deleted.begin(); it != deleted.end(); ++it){
            setNDeleted(getNDeleted() + 1);
            setDeleted(getNDeleted(), (*it).c_str());
        }
    }
    QString name_str = unquote(name);
    MSNUserData *data = findContact(mail, contact);
    if (data){
        data->ScreenName.str() = name;
        setupContact(contact, data);
        return data;
    }
    if (bJoin){
        ContactList::ContactIterator it;
        while ((contact = ++it) != NULL){
            if (contact->getName() == name_str){
                data = (MSNUserData*)(contact->clientData.createData(this));
                data->EMail.str() = mail;
                data->ScreenName.str() = name;
                setupContact(contact, data);
                Event e(EventContactChanged, contact);
                e.process();
                return data;
            }
        }
        it.reset();
        while ((contact = ++it) != NULL){
            if (contact->getName().lower() == name_str.lower()){
                data = (MSNUserData*)(contact->clientData.createData(this));
                data->EMail.str() = mail;
                data->ScreenName.str() = name;
                setupContact(contact, data);
                Event e(EventContactChanged, contact);
                e.process();
                m_bJoin = true;
                return data;
            }
        }
        int n = name_str.find('@');
        if (n > 0){
            name_str = name_str.left(n);
            it.reset();
            while ((contact = ++it) != NULL){
                if (contact->getName().lower() == name_str.lower()){
                    data = (MSNUserData*)(contact->clientData.createData(this));
                    data->EMail.str() = mail;
                    data->ScreenName.str() = name;
                    setupContact(contact, data);
                    Event e(EventContactChanged, contact);
                    e.process();
                    m_bJoin = true;
                    return data;
                }
            }
        }
    }
    contact = getContacts()->contact(0, true);
    data = (MSNUserData*)(contact->clientData.createData(this));
    data->EMail.str() = mail;
    data->ScreenName.str() = name;
    contact->setName(name_str);
    Event e(EventContactChanged, contact);
    e.process();
    return data;
}

MSNUserData *MSNClient::findGroup(unsigned long id, const QString &name, Group *&grp)
{
    ContactList::GroupIterator itg;
    while ((grp = ++itg) != NULL){
        ClientDataIterator it(grp->clientData, this);
        MSNUserData *res = (MSNUserData*)(++it);
        if ((res == NULL) || (res->Group.toULong() != id))
            continue;
        if (!name.isEmpty() && res->ScreenName.setStr(name)){
            grp->setName(name);
            Event e(EventGroupChanged, grp);
            e.process();
        }
        return res;
    }
    if (name.isEmpty())
        return NULL;
    QString grpName = name;
    itg.reset();
    while ((grp = ++itg) != NULL){
        if (grp->getName() != grpName)
            continue;
        MSNUserData *res = (MSNUserData*)(grp->clientData.createData(this));
        res->Group.asULong() = id;
        res->ScreenName.str() = name;
        return res;
    }
    grp = getContacts()->group(0, true);
    MSNUserData *res = (MSNUserData*)(grp->clientData.createData(this));
    res->Group.asULong() = id;
    res->ScreenName.str() = name;
    grp->setName(grpName);
    Event e(EventGroupChanged, grp);
    e.process();
    return res;
}

void MSNClient::auth_message(Contact *contact, unsigned type, MSNUserData *data)
{
    AuthMessage msg(type);
    msg.setClient(dataName(data));
    msg.setContact(contact->id());
    msg.setFlags(MESSAGE_RECEIVED);
    Event e(EventMessageReceived, &msg);
    e.process();
}

bool MSNClient::done(unsigned code, Buffer&, const char *headers)
{
    string h;
    switch (m_state){
    case LoginHost:
        if (code == 200){
            h = getHeader("PassportURLs", headers);
            if (h.empty()){
                m_socket->error_state("No PassportURLs answer");
                break;
            }
            string loginHost = getValue("DALogin", h.c_str());
            if (loginHost.empty()){
                m_socket->error_state("No DALogin in PassportURLs answer");
                break;
            }
            string loginUrl = "https://";
            loginUrl += loginHost;
            requestTWN(loginUrl.c_str());
        }else{
            m_socket->error_state("Bad answer code");
        }
        break;
    case TWN:
        if (code == 200){
            h = getHeader("Authentication-Info", headers);
            if (h.empty()){
                m_socket->error_state("No Authentication-Info answer");
                break;
            }
            string twn = getValue("from-PP", h.c_str());
            if (twn.empty()){
                m_socket->error_state("No from-PP in Authentication-Info answer");
                break;
            }
            MSNPacket *packet = new UsrPacket(this, twn.c_str());
            packet->send();
        }else if (code == 401){
            authFailed();
        }else{
            m_socket->error_state("Bad answer code");
        }
        break;
    default:
        log(L_WARN, "Fetch done in bad state");
    }
    return false;
}

void *MSNClient::processEvent(Event *e)
{
    TCPClient::processEvent(e);
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == static_cast<MSNPlugin*>(protocol()->plugin())->MSNInitMail){
            Event eGo(EventGoURL, (void*)&m_init_mail);
            eGo.process();
            return e->param();
        }
        if (cmd->id == static_cast<MSNPlugin*>(protocol()->plugin())->MSNNewMail){
            Event eGo(EventGoURL, (void*)&m_new_mail);
            eGo.process();
            return e->param();
        }
    }
    if (e->type() == EventAddContact){
        addContact *ac = (addContact*)(e->param());
        if (ac->proto && !strcmp(protocol()->description()->text, ac->proto)){
            Contact *contact = NULL;
            findContact(ac->addr, ac->nick, contact);
            if (contact && (contact->getGroup() != ac->group)){
                contact->setGroup(ac->group);
                Event e(EventContactChanged, contact);
                e.process();
            }
            return contact;
        }
        return NULL;
    }
    if (e->type() == EventDeleteContact){
        char *addr = (char*)(e->param());
        ContactList::ContactIterator it;
        Contact *contact;
        while ((contact = ++it) != NULL){
            MSNUserData *data;
            ClientDataIterator itc(contact->clientData, this);
            while ((data = (MSNUserData*)(++itc)) != NULL){
                if (data->EMail.str() == QString::fromUtf8(addr)){
                    contact->clientData.freeData(data);
                    ClientDataIterator itc(contact->clientData);
                    if (++itc == NULL)
                        delete contact;
                    return e->param();
                }
            }
        }
        return NULL;
    }
    if (e->type() == EventGetContactIP){
        Contact *contact = (Contact*)(e->param());
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            if (data->IP.ip())
                return data->IP.ip();
        }
        return NULL;
    }
    if (e->type() == EventMessageAccept){
        messageAccept *ma = static_cast<messageAccept*>(e->param());
        Contact *contact = getContacts()->contact(ma->msg->contact());
        if (contact == NULL)
            return NULL;
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            if (dataName(data) == ma->msg->client()){
                SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
                if (sock)
                    sock->acceptMessage(ma->msg, ma->dir, ma->overwrite);
                return e->param();
            }
        }
    }
    if (e->type() == EventMessageDecline){
        messageDecline *md = (messageDecline*)(e->param());
        Contact *contact = getContacts()->contact(md->msg->contact());
        if (contact == NULL)
            return NULL;
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            if (dataName(data) == md->msg->client()){
                SBSocket *sock = dynamic_cast<SBSocket*>(data->sb.object());
                if (sock)
                    sock->declineMessage(md->msg, md->reason);
                return e->param();
            }
        }
    }
    if (e->type() == EventContactChanged){
        Contact *contact = (Contact*)(e->param());
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            bool bChanged = false;
            if (contact->getIgnore() != ((data->Flags.toULong() & MSN_BLOCKED) != 0))
                bChanged = true;
            if (contact->getGroup() != (data->Group.toULong()))
                bChanged = true;
            if (contact->getName() != data->ScreenName.str())
                bChanged = true;
            if (!bChanged)
                continue;
            findRequest(data->EMail.str(), LR_CONTACTxCHANGED, true);
            MSNListRequest lr;
            lr.Type = LR_CONTACTxCHANGED;
            lr.Name = data->EMail.str();
            m_requests.push_back(lr);
        }
        processRequests();
        return NULL;
    }
    if (e->type() == EventContactDeleted){
        Contact *contact = (Contact*)(e->param());
        MSNUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (MSNUserData*)(++it)) != NULL){
            findRequest(data->EMail.str(), LR_CONTACTxCHANGED, true);
            MSNListRequest lr;
            if (data->Group.toULong() != NO_GROUP){
                lr.Type  = LR_CONTACTxREMOVED;
                lr.Name  = data->EMail.str();
                lr.Group = data->Group.toULong();
                m_requests.push_back(lr);
            }
            if (data->Flags.toULong() & MSN_BLOCKED){
                lr.Type = LR_CONTACTxREMOVED_BL;
                lr.Name  = data->EMail.str();
                m_requests.push_back(lr);
            }
        }
        processRequests();
        return NULL;
    }
    if (e->type() == EventGroupChanged){
        Group *grp = (Group*)(e->param());
        ClientDataIterator it(grp->clientData, this);
        MSNUserData *data = (MSNUserData*)(++it);
        if ((data == NULL) || (grp->getName() != data->ScreenName.str())){
            findRequest(grp->id(), LR_GROUPxCHANGED, true);
            MSNListRequest lr;
            lr.Type = LR_GROUPxCHANGED;
            lr.Name = QString::number(grp->id());
            m_requests.push_back(lr);
            processRequests();
        }
        return NULL;
    }
    if (e->type() == EventGroupDeleted){
        Group *grp = (Group*)(e->param());
        ClientDataIterator it(grp->clientData, this);
        MSNUserData *data = (MSNUserData*)(++it);
        if (data){
            findRequest(grp->id(), LR_GROUPxCHANGED, true);
            MSNListRequest lr;
            lr.Type = LR_GROUPxREMOVED;
            lr.Name = QString::number(data->Group.toULong());
            m_requests.push_back(lr);
            processRequests();
        }
    }
    if (e->type() == EventMessageCancel){
        Message *msg = (Message*)(e->param());
        for (list<SBSocket*>::iterator it = m_SBsockets.begin(); it != m_SBsockets.end(); ++it){
            if ((*it)->cancelMessage(msg))
                return msg;
        }
    }
    return NULL;
}

void MSNClient::requestLoginHost(const char *url)
{
    if (!isDone())
        return;
    m_state = LoginHost;
    fetch(url);
}

void MSNClient::requestTWN(const char *url)
{
    if (!isDone())
        return;
    QString auth = "Authorization: Passport1.4 OrgVerb=GET,OrgURL=http%%3A%%2F%%2Fmessenger%%2Emsn%%2Ecom,sign-in=";
    auth += quote(getLogin()).utf8();
    auth += ",pwd=";
    auth += quote(getPassword()).utf8();
    auth += ",";
    auth += m_authChallenge;
    m_state = TWN;
    fetch(url, auth);
}

string MSNClient::getValue(const char *key, const char *str)
{
    string s = str;
    while (!s.empty()){
        string k = getToken(s, '=');
        string v;
        if (s[0] == '\''){
            getToken(s, '\'');
            v = getToken(s, '\'');
            getToken(s, ',');
        }else{
            v = getToken(s, ',');
        }
        if (k == key)
            return v;
    }
    return "";
}

string MSNClient::getHeader(const char *_name, const char *_headers)
{
    QCString hdr = _headers;
    QCString name = _name;
    int idx = hdr.find(name + ":");
    if(idx != -1) {
        const char *p;
        for (p = &hdr[idx]; *p; p++)
            if (*p != ' ')
                break;
        return p;
    }
    return "";
}

MSNListRequest *MSNClient::findRequest(unsigned long id, unsigned type, bool bDelete)
{
    if (m_requests.empty())
        return NULL;
    QString name = QString::number(id);
    return findRequest(name, type, bDelete);
}

MSNListRequest *MSNClient::findRequest(const QString &name, unsigned type, bool bDelete)
{
    if (m_requests.empty())
        return NULL;
    for (list<MSNListRequest>::iterator it = m_requests.begin(); it != m_requests.end(); ++it){
        if (((*it).Type == type) && ((*it).Name == name)){
            if (bDelete){
                m_requests.erase(it);
                return NULL;
            }
            return &(*it);
        }
    }
    return NULL;
}

void MSNClient::processRequests()
{
    if (m_requests.empty() || (getState() != Connected))
        return;
    for (list<MSNListRequest>::iterator it = m_requests.begin(); it != m_requests.end(); ++it){
        Group *grp;
        Contact *contact;
        MSNPacket *packet = NULL;
        MSNUserData *data;
        switch ((*it).Type){
        case LR_CONTACTxCHANGED:
            data = findContact((*it).Name, contact);
            if (data){
                bool bBlock = (data->Flags.toULong() & MSN_BLOCKED) != 0;
                if (contact->getIgnore() != bBlock){
                    if (contact->getIgnore()){
                        if (data->Flags.toULong() & MSN_FORWARD)
                            packet = new RemPacket(this, "FL", (*it).Name);
                        if (data->Flags.toULong() & MSN_ACCEPT){
                            if (packet)
                                packet->send();
                            packet = new RemPacket(this, "AL", (*it).Name);
                        }
                        data->Flags.asULong() &= ~(MSN_FORWARD | MSN_ACCEPT);
                        if (packet)
                            packet->send();
                        packet = new AddPacket(this, "BL", data->EMail.str(), quote(contact->getName()));
                        data->ScreenName.str() = contact->getName();
                        data->Flags.asULong() |= MSN_BLOCKED;
                    }else{
                        packet = new RemPacket(this, "BL", data->EMail.str());
                        data->Flags.asULong() &= ~MSN_BLOCKED;
                    }
                }
                if (data->Flags.toULong() & MSN_BLOCKED)
                    break;
                unsigned grp_id = 0;
                if (contact->getGroup()){
                    Group *grp = getContacts()->group(contact->getGroup());
                    ClientDataIterator it(grp->clientData, this);
                    MSNUserData *res = (MSNUserData*)(++it);
                    if (res)
                        grp_id = res->Group.toULong();
                }
                if (((data->Flags.toULong() & MSN_FORWARD) == 0) || (data->Group.toULong() == NO_GROUP)){
                    if (packet)
                        packet->send();
                    packet = new AddPacket(this, "FL", data->EMail.str(), quote(data->ScreenName.str()), grp_id);
                    data->Group.asULong() = grp_id;
                    data->Flags.asULong() |= MSN_FORWARD;
                }
                if (getAutoAuth() && (data->Flags.toULong() & MSN_FORWARD) && ((data->Flags.toULong() & MSN_ACCEPT) == 0) && ((data->Flags.toULong() & MSN_BLOCKED) == 0)){
                    if (packet)
                        packet->send();
                    packet = new AddPacket(this, "AL", data->EMail.str(), quote(data->ScreenName.str()), 0);
                    data->Group.asULong() = grp_id;
                    data->Flags.asULong() |= MSN_ACCEPT;
                }
                if (data->Group.toULong() != grp_id){
                    if (packet)
                        packet->send();
                    packet = new AddPacket(this, "FL", data->EMail.str(), quote(data->ScreenName.str()), grp_id);
                    packet->send();
                    packet = NULL;
                    packet = new RemPacket(this, "FL", data->EMail.str(), data->Group.toULong());
                    data->Group.asULong() = grp_id;
                }
                if (contact->getName() != data->ScreenName.str()){
                    if (packet)
                        packet->send();
                    packet = new ReaPacket(this, data->EMail.str(), quote(contact->getName()));
                    data->ScreenName.str() = contact->getName();
                }
            }
            break;
        case LR_CONTACTxREMOVED:
            packet = new RemPacket(this, "AL", (*it).Name);
            packet->send();
            packet = new RemPacket(this, "FL", (*it).Name);
            setNDeleted(getNDeleted() + 1);
            setDeleted(getNDeleted(), (*it).Name.utf8());
            break;
        case LR_CONTACTxREMOVED_BL:
            packet = new RemPacket(this, "BL", (*it).Name);
            break;
        case LR_GROUPxCHANGED:
            grp = getContacts()->group(it->Name.toULong());
            if (grp){
                ClientDataIterator it(grp->clientData, this);
                data = (MSNUserData*)(++it);
                if (data){
                    packet = new RegPacket(this, data->Group.toULong(), quote(grp->getName()));
                }else{
                    packet = new AdgPacket(this, grp->id(), quote(grp->getName()));
                    data = (MSNUserData*)(grp->clientData.createData(this));
                }
                data->ScreenName.str() = grp->getName();
            }
            break;
        case LR_GROUPxREMOVED:
            packet = new RmgPacket(this, (*it).Name.toULong());
            break;
        }
        if (packet)
            packet->send();
    }
    m_requests.clear();
}

bool MSNClient::add(const QString &mail, const QString &name, unsigned grp)
{
    Contact *contact;
    MSNUserData *data = findContact(mail, contact);
    if (data){
        if (contact->getGroup() != grp){
            contact->setGroup(grp);
            Event e(EventContactChanged, contact);
            e.process();
        }
        return false;
    }
    data = findContact(mail, name, contact);
    if (data == NULL)
        return false;
    contact->setGroup(grp);
    Event e(EventContactChanged, contact);
    e.process();
    return true;
}

bool MSNClient::compareData(void *d1, void *d2)
{
    return (((MSNUserData*)d1)->EMail.str() == ((MSNUserData*)d2)->EMail.str());
}

static void addIcon(QString *s, const QString &icon, const QString &statusIcon)
{
    if (s == NULL)
        return;
    if (statusIcon == icon)
        return;
    QString str = *s;
    while (!str.isEmpty()){
        QString item = getToken(str, ',');
        if (item == icon)
            return;
    }

    if (!s->isEmpty())
        *s += ',';
    *s += icon;
}

void MSNClient::contactInfo(void *_data, unsigned long &curStatus, unsigned&, QString &statusIcon, QString *icons)
{
    MSNUserData *data = (MSNUserData*)_data;
    unsigned cmp_status = data->Status.toULong();
    const CommandDef *def;
    for (def = protocol()->statusList(); def->text; def++){
        if (def->id == cmp_status)
            break;
    }
    if ((cmp_status == STATUS_BRB) || (cmp_status == STATUS_PHONE) || (cmp_status == STATUS_LUNCH))
        cmp_status = STATUS_AWAY;
    if (data->Status.toULong() > curStatus){
        curStatus = data->Status.toULong();
        if (statusIcon && icons){
            QString iconSave = *icons;
            *icons = statusIcon;
            if (iconSave.length())
                addIcon(icons, iconSave, statusIcon);
        }
        statusIcon = def->icon;
    }else{
        if (statusIcon){
            addIcon(icons, def->icon, statusIcon);
        }else{
            statusIcon = def->icon;
        }
    }
    if (icons && data->typing_time.toULong())
        addIcon(icons, "typing", statusIcon);
}

QString MSNClient::contactTip(void *_data)
{
    MSNUserData *data = (MSNUserData*)_data;
    unsigned long status = STATUS_UNKNOWN;
    unsigned style  = 0;
    QString statusIcon;
    contactInfo(data, status, style, statusIcon);
    QString res;
    res += "<img src=\"icon:";
    res += statusIcon;
    res += "\">";
    QString statusText;
    for (const CommandDef *cmd = protocol()->statusList(); !cmd->text.isEmpty(); cmd++){
        if (!strcmp(cmd->icon, statusIcon)){
            res += " ";
            statusText = i18n(cmd->text);
            res += statusText;
            break;
        }
    }
    res += "<br>";
    res += data->EMail.str();
    res += "</b>";
    if (data->Status.toULong() == STATUS_OFFLINE){
        if (data->StatusTime.toULong()){
            res += "<br><font size=-1>";
            res += i18n("Last online");
            res += ": </font>";
            res += formatDateTime(data->StatusTime.toULong());
        }
    }else{
        if (data->OnlineTime.toULong()){
            res += "<br><font size=-1>";
            res += i18n("Online");
            res += ": </font>";
            res += formatDateTime(data->OnlineTime.toULong());
        }
        if (data->Status.toULong() != STATUS_ONLINE){
            res += "<br><font size=-1>";
            res += statusText;
            res += ": </font>";
            res += formatDateTime(data->StatusTime.toULong());
        }
    }
    if (data->IP.ip()){
        res += "<br>";
        res += formatAddr(data->IP, data->Port.toULong());
    }
    if (data->RealIP.ip() && ((data->IP.ip() == NULL) || (get_ip(data->IP) != get_ip(data->RealIP)))){
        res += "<br>";
        res += formatAddr(data->RealIP, data->Port.toULong());
    }
    return res;
}

QWidget *MSNClient::searchWindow(QWidget *parent)
{
    if (getState() != Connected)
        return NULL;
    return new MSNSearch(this, parent);
}

SBSocket::SBSocket(MSNClient *client, Contact *contact, MSNUserData *data)
{
    m_state		= Unknown;
    m_client	= client;
    m_contact	= contact;
    m_data		= data;
    m_socket	= new ClientSocket(this, client->createSBSocket());
    m_packet_id = 0;
    m_messageSize = 0;
    m_invite_cookie = get_random();
    m_bTyping	= false;
    m_client->m_SBsockets.push_back(this);
}

SBSocket::~SBSocket()
{
    if (m_packet)
        m_packet->clear();
    if (m_socket)
        delete m_socket;
    list<SBSocket*>::iterator it = find(m_client->m_SBsockets.begin(), m_client->m_SBsockets.end(), this);
    if (it != m_client->m_SBsockets.end())
        m_client->m_SBsockets.erase(it);
    if (m_data){
        m_data->sb.clear();
        if (m_data->typing_time.toULong()){
            m_data->typing_time.asULong() = 0;
            Event e(EventContactStatus, m_contact);
            e.process();
        }
    }
    list<Message*>::iterator itm;
    for (itm = m_queue.begin(); itm != m_queue.end(); ++itm){
        Message *msg = (*itm);
        msg->setError(I18N_NOOP("Contact go offline"));
        Event e(EventMessageSent, msg);
        e.process();
        delete msg;
    }
    list<msgInvite>::iterator itw;
    for (itw = m_waitMsg.begin(); itw != m_waitMsg.end(); ++itw){
        Message *msg = (*itw).msg;
        msg->setError(I18N_NOOP("Contact go offline"));
        Event e(EventMessageSent, msg);
        e.process();
        delete msg;
    }
    for (itw = m_acceptMsg.begin(); itw != m_acceptMsg.end(); ++itw){
        Message *msg = (*itw).msg;
        Event e(EventMessageDeleted, msg);
        e.process();
        delete msg;
    }
}

void SBSocket::connect()
{
    m_packet = new XfrPacket(m_client, this);
    m_packet->send();
}

void SBSocket::connect(const QString &addr, const QString &session, const QString &cookie, bool bDirection)
{
    m_packet = NULL;
    if (m_state != Unknown){
        log(L_DEBUG, "Connect in bad state");
        return;
    }
    if (bDirection){
        m_state = ConnectingSend;
    }else{
        m_state = ConnectingReceive;
    }
    m_cookie = cookie;
    m_session = session;
    QString ip = addr;
    unsigned short port = 0;
    int n = ip.find(':');
    if (n > 0){
        port = ip.mid(n + 1).toUShort();
        ip = ip.left(n);
    }
    if (port == 0){
        m_socket->error_state("Bad address");
        return;
    }
    m_socket->connect(ip, port, m_client);
}

bool SBSocket::send(Message *msg)
{

    m_bTyping = false;
    m_queue.push_back(msg);
    switch (m_state){
    case Unknown:
        connect();
        break;
    case Connected:
        process();
        break;
    default:
        break;
    }
    return true;
}

bool SBSocket::error_state(const QString&, unsigned)
{
    if (m_queue.size()){
        m_socket->close();
        connect();
        return false;
    }
    return true;
}

void SBSocket::connect_ready()
{
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    QString args = m_client->data.owner.EMail.str();
    args += " ";
    args += m_cookie;
    m_cookie = "";
    switch (m_state){
    case ConnectingSend:
        send("USR", args);
        m_state = WaitJoin;
        break;
    case ConnectingReceive:
        args += " ";
        args += m_session;
        send("ANS", args);
        m_state = Connected;
        process();
        break;
    default:
        log(L_WARN, "Bad state for connect ready");
    }
}

void SBSocket::packet_ready()
{
    if (m_socket->readBuffer.writePos() == 0)
        return;
    MSNPlugin *plugin = static_cast<MSNPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->MSNPacket);
    for (;;){
        if (m_messageSize && !getMessage())
            break;
        QCString s;
        if (!m_socket->readBuffer.scan("\r\n", s))
            break;
        getLine(s);
    }
    if (m_socket->readBuffer.readPos() == m_socket->readBuffer.writePos())
        m_socket->readBuffer.init(0);
}

void SBSocket::getMessage(unsigned size)
{
    m_messageSize = size;
    m_message = "";
    getMessage();
}

bool SBSocket::getMessage()
{
    unsigned tail = m_socket->readBuffer.writePos() - m_socket->readBuffer.readPos();
    if (tail > m_messageSize)
        tail = m_messageSize;
    QString msg;
    m_socket->readBuffer.unpack(msg, tail);
    m_message += msg;
    m_messageSize -= tail;
    if (m_messageSize)
        return false;
    messageReady();
    return true;
}

void SBSocket::send(const QString &cmd, const QString &args)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer
    << (const char*)cmd.utf8()
    << " "
    << (const char*)QString::number(++m_packet_id).utf8();
    if (!args.isEmpty()){
        m_socket->writeBuffer
        << " "
        << (const char*)args.utf8();
    }
    m_socket->writeBuffer << "\r\n";
    MSNPlugin *plugin = static_cast<MSNPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->MSNPacket);
    m_socket->write();
}

void SBSocket::getLine(const QCString &_line)
{
    QString line = QString::fromUtf8(_line);
    QString cmd = getToken(line, ' ');
    if (cmd == "BYE"){
        m_socket->error_state("");
        return;
    }
    if (cmd == "MSG"){
        QString email = getToken(line, ' ');
        getToken(line, ' ');
        unsigned size = line.toUInt();
        getMessage(size);
    }
    if (cmd == "JOI"){
        if (m_state != WaitJoin){
            log(L_WARN, "JOI in bad state");
            return;
        }
        m_state = Connected;
        process();
    }
    if (cmd == "USR")
        send("CAL", m_data->EMail.str());
    if ((cmd == "ACK") || (cmd == "NAK")){
        unsigned id = getToken(line, ' ').toUInt();
        if (id != m_msg_id){
            log(L_WARN, "Bad ACK id");
            return;
        }
        if (m_queue.empty())
            return;
        Message *msg = m_queue.front();
        if (cmd == "NAK"){
            m_msgText = "";
            msg->setError(I18N_NOOP("Send message failed"));
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            m_queue.erase(m_queue.begin());
            process(false);
            return;
        }
        if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
            Message m(MessageGeneric);
            m.setContact(m_contact->id());
            m.setClient(m_client->dataName(m_data));
            m.setText(m_msgPart);
            m.setForeground(msg->getForeground());
            m.setBackground(0xFFFFFF);
            m.setFont(msg->getFont());
            Event e(EventSent, &m);
            e.process();
        }
        if (m_msgText.isEmpty()){
            if (msg->type() == MessageFile){
                sendFile();
            }else{
                Event e(EventMessageSent, msg);
                e.process();
                delete msg;
                m_queue.erase(m_queue.begin());
            }
        }
        process();
    }
}

typedef map<QString, QString>	STR_VALUES;

static STR_VALUES parseValues(const QString &str)
{
    STR_VALUES res;
    QString s = str.stripWhiteSpace();
    while (!s.isEmpty()){
        QString p = getToken(s, ';', false).stripWhiteSpace();
        QString key = getToken(p, '=', false);
        STR_VALUES::iterator it = res.find(key);
        if (it == res.end()){
            res.insert(STR_VALUES::value_type(key, p));
        }else{
            (*it).second = p;
        }
        s = s.stripWhiteSpace();
    }
    return res;
}

static char FT_GUID[] = "{5D3E02AB-6190-11d3-BBBB-00C04F795683}";

void SBSocket::messageReady()
{
    log(L_DEBUG, "MSG: [%s]", m_message.local8Bit().data());
    QString content_type;
    QString charset;
    QString font;
    QString typing;
    unsigned color = 0;
    bool bColor = false;
    while (!m_message.isEmpty()){
        int n = m_message.find("\r\n");
        if (n < 0){
            log(L_DEBUG, "Error parse message");
            return;
        }
        if (n == 0){
            m_message = m_message.mid(2);
            break;
        }
        QString line = m_message.left(n);
        m_message = m_message.mid(n + 2);
        QString key = getToken(line, ':', false);
        if (key == "Content-Type"){
            line = line.stripWhiteSpace();
            content_type = getToken(line, ';').stripWhiteSpace();
            STR_VALUES v = parseValues(line.stripWhiteSpace());
            STR_VALUES::iterator it = v.find("charset");
            if (it != v.end())
                charset = (*it).second;
            continue;
        }
        if (key == "X-MMS-IM-Format"){
            STR_VALUES v = parseValues(line.stripWhiteSpace());
            STR_VALUES::iterator it = v.find("FN");
            if (it != v.end())
                font = m_client->unquote((*it).second);
            it = v.find("EF");
            if (it != v.end()){
                QString effects = (*it).second;
                if (effects.find('B') != -1)
                    font += ", bold";
                if (effects.find('I') != -1)
                    font += ", italic";
                if (effects.find('S') != -1)
                    font += ", strikeout";
                if (effects.find('U') != -1)
                    font += ", underline";
            }
            it = v.find("CO");
            if (it != v.end())
                color = (*it).second.toULong(&bColor, 16);
            continue;
        }
        if (key == "TypingUser"){
            typing = line.stripWhiteSpace();
            continue;
        }
    }
    if (content_type == "text/plain"){
        if (m_data->typing_time.toULong()){
            m_data->typing_time.asULong() = 0;
            Event e(EventContactStatus, m_contact);
            e.process();
        }
        QString msg_text = m_message;
        msg_text = msg_text.replace(QRegExp("\\r"), "");
        Message *msg = new Message(MessageGeneric);
        msg->setFlags(MESSAGE_RECEIVED);
        if (bColor){
            msg->setBackground(0xFFFFFF);
            msg->setForeground(color);
        }
        msg->setFont(font);
        msg->setText(msg_text);
        msg->setContact(m_contact->id());
        msg->setClient(m_client->dataName(m_data));
        Event e(EventMessageReceived, msg);
        if (!e.process())
            delete msg;
        return;
    }
    if (content_type == "text/x-msmsgscontrol"){
        if (typing.lower() == m_data->EMail.str().lower()){
            bool bEvent = (m_data->typing_time.toULong() == 0);
            m_data->typing_time.asULong() = time(NULL);
            if (bEvent){
                Event e(EventContactStatus, m_contact);
                e.process();
            }
        }
    }
    if (content_type == "text/x-msmsgsinvite"){
        QString file;
        QString command;
        QString guid;
        QString code;
        QString ip_address;
        QString ip_address_internal;
        unsigned short port = 0;
        unsigned short port_x = 0;
        unsigned cookie = 0;
        unsigned auth_cookie = 0;
        unsigned fileSize = 0;
        while (!m_message.isEmpty()){
            QString line;
            int n = m_message.find("\r\n");
            if (n < 0){
                line = m_message;
                m_message = "";
            }else{
                line = m_message.left(n);
                m_message = m_message.mid(n + 2);
            }
            QString key = getToken(line, ':', false);
            line = line.stripWhiteSpace();
            if (key == "Application-GUID"){
                guid = line;
                continue;
            }
            if (key == "Invitation-Command"){
                command = line;
                continue;
            }
            if (key == "Invitation-Cookie"){
                cookie = line.toULong();
                continue;
            }
            if (key == "Application-File"){
                file = line;
                continue;
            }
            if (key == "Application-FileSize"){
                fileSize = line.toULong();
                continue;
            }
            if (key == "Cancel-Code"){
                code = line;
                continue;
            }
            if (key == "IP-Address"){
                ip_address = line;
                continue;
            }
            if (key == "IP-Address-Internal"){
                ip_address_internal = (unsigned short)line.toULong();
                continue;
            }
            if (key == "Port"){
                port =  (unsigned short)line.toULong();
                continue;
            }
            if (key == "PortX"){
                port_x = (unsigned short)line.toULong();
                continue;
            }
            if (key == "AuthCookie"){
                auth_cookie = line.toULong();
                continue;
            }

        }
        if (cookie == 0){
            log(L_WARN, "No cookie in message");
            return;
        }
        if (command == "INVITE"){
            if (guid != FT_GUID){
                log(L_WARN, "Unknown GUID %s", guid.latin1());
                return;
            }
            if (file.isEmpty()){
                log(L_WARN, "No file in message");
                return;
            }
            FileMessage *msg = new FileMessage;
            msg->setDescription(m_client->unquote(file));
            msg->setSize(fileSize);
            msg->setFlags(MESSAGE_RECEIVED | MESSAGE_TEMP);
            msg->setContact(m_contact->id());
            msg->setClient(m_client->dataName(m_data));
            msgInvite m;
            m.msg    = msg;
            m.cookie = cookie;
            m_acceptMsg.push_back(m);
            Event e(EventMessageReceived, msg);
            if (e.process()){
                for (list<msgInvite>::iterator it = m_acceptMsg.begin(); it != m_acceptMsg.end(); ++it){
                    if ((*it).msg == msg){
                        m_acceptMsg.erase(it);
                        break;
                    }
                }
            }
        }else if (command == "ACCEPT"){
            unsigned ip      = inet_addr(ip_address);
            unsigned real_ip = inet_addr(ip_address_internal);
            if (ip != INADDR_NONE)
                set_ip(&m_data->IP, ip);
            if (real_ip != INADDR_NONE)
                set_ip(&m_data->RealIP, real_ip);
            if (port)
                m_data->Port.asULong() = port;
            list<msgInvite>::iterator it;
            for (it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
                if ((*it).cookie == cookie){
                    Message *msg = (*it).msg;
                    if (msg->type() == MessageFile){
                        m_waitMsg.erase(it);
                        FileMessage *m = static_cast<FileMessage*>(msg);
                        MSNFileTransfer *ft;
                        bool bNew = false;
                        if (m->m_transfer){
                            ft = static_cast<MSNFileTransfer*>(m->m_transfer);
                        }else{
                            ft = new MSNFileTransfer(m, m_client, m_data);
                            bNew = true;
                        }
                        ft->ip1   = real_ip;
                        ft->port1 = port;
                        ft->ip2	  = ip;
                        ft->port2 = port_x;
                        ft->auth_cookie = auth_cookie;

                        if (bNew){
                            Event e(EventMessageAcked, msg);
                            e.process();
                        }
                        ft->connect();
                        break;
                    }
                    msg->setError("Bad type");
                    Event e(EventMessageSent, msg);
                    e.process();
                    delete msg;
                    return;
                }
            }
            if (it == m_waitMsg.end())
                log(L_WARN, "No message for accept");
            return;
        }else if (command == "CANCEL"){
            if (code == "REJECT"){
                list<msgInvite>::iterator it;
                for (it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
                    if ((*it).cookie == cookie){
                        Message *msg = (*it).msg;
                        msg->setError(I18N_NOOP("Message declined"));
                        Event e(EventMessageSent, msg);
                        e.process();
                        delete msg;
                        m_waitMsg.erase(it);
                        break;
                    }
                }
                if (it == m_waitMsg.end())
                    log(L_WARN, "No message for cancel");
                return;
            }
            list<msgInvite>::iterator it;
            for (it = m_acceptMsg.begin(); it != m_acceptMsg.end(); ++it){
                if ((*it).cookie == cookie){
                    Message *msg = (*it).msg;
                    Event e(EventMessageDeleted, msg);
                    e.process();
                    delete msg;
                    m_acceptMsg.erase(it);
                    break;
                }
            }
            if (it == m_acceptMsg.end())
                log(L_WARN, "No message for cancel");
        }else{
            log(L_WARN, "Unknown command %s", command.latin1());
            return;
        }
    }
}

void SBSocket::timer(unsigned now)
{
    if (m_data->typing_time.toULong()){
        if (now >= m_data->typing_time.toULong() + TYPING_TIME){
            m_data->typing_time.asULong() = 0;
            Event e(EventContactStatus, m_contact);
            e.process();
        }
    }
    sendTyping();
}

void SBSocket::setTyping(bool bTyping)
{
    if (m_bTyping == bTyping)
        return;
    m_bTyping = bTyping;
    sendTyping();
}

void SBSocket::sendTyping()
{
    if (m_bTyping && (m_state == Connected)){
        QString message;
        message += "MIME-Version: 1.0\r\n";
        message += "Content-Type: text/x-msmsgcontrol\r\n";
        message += "TypingUser: ";
        message += m_client->data.owner.EMail.str();
        message += "\r\n";
        message += "\r\n";
        sendMessage(message, "U");
    }
}

void SBSocket::sendMessage(const QString &str, const char *type)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer
    << "MSG "
    << (const char*)QString::number(++m_packet_id).utf8()
    << " "
    << type
    << " "
    << (const char*)QString::number(str.utf8().length()).utf8()
    << "\r\n"
    << (const char*)str.utf8();
    MSNPlugin *plugin = static_cast<MSNPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->MSNPacket);
    m_socket->write();
}

bool SBSocket::cancelMessage(Message *msg)
{
    if (m_queue.empty())
        return false;
    if (m_queue.front() == msg){
        m_msgPart = "";
        m_msgText = "";
        m_msg_id = 0;
        m_queue.erase(m_queue.begin());
        process();
        return true;
    }
    list<Message*>::iterator it = find(m_queue.begin(), m_queue.end(), msg);
    if (it == m_queue.end())
        return false;
    m_queue.erase(it);
    delete msg;
    return true;
}

void SBSocket::sendFile()
{
    if (m_queue.empty())
        return;
    Message *msg = m_queue.front();
    if (msg->type() != MessageFile)
        return;
    m_queue.erase(m_queue.begin());
    FileMessage *m = static_cast<FileMessage*>(msg);
    if (++m_invite_cookie == 0)
        m_invite_cookie++;
    msgInvite mi;
    mi.msg    = msg;
    mi.cookie = m_invite_cookie;
    m_waitMsg.push_back(mi);
    QString message;
    message += "MIME-Version: 1.0\r\n";
    message += "Content-Type: text/x-msmsgsinvite; charset=UTF-8\r\n\r\n"
               "Application-Name: File Transfer\r\n"
               "Application-GUID: ";
    message += FT_GUID;
    message += "\r\n"
               "Invitation-Command: INVITE\r\n"
               "Invitation-Cookie: ";
    message += QString::number(m_invite_cookie);
    message += "\r\n"
               "Application-File: ";
    QString name;
    unsigned size;
    if (m->m_transfer){
        name = m->m_transfer->m_file->name();
        size = m->m_transfer->fileSize();
    }else{
        FileMessage::Iterator it(*m);
        if (it[0])
            name = *it[0];
        size = it.size();
    }
    name = name.replace(QRegExp("\\\\"), "/");
    int n = name.findRev("/");
    if (n >= 0)
        name = name.mid(n + 1);
    message += m_client->quote(name);
    message += "\r\n"
               "Application-FileSize: ";
    message += QString::number(size);
    message += "\r\n"
               "Connectivity: N\r\n\r\n";
    sendMessage(message, "S");
}

void SBSocket::process(bool bTyping)
{
    if (bTyping)
        sendTyping();
    if (m_msgText.isEmpty() && !m_queue.empty()){
        Message *msg = m_queue.front();
        m_msgText = msg->getPlainText();
        QCString cstr = m_msgText.utf8();
        messageSend ms;
        ms.msg  = msg;
        ms.text = &cstr;
        Event e(EventSend, &ms);
        e.process();
        m_msgText = QString::fromUtf8(cstr);
        if (msg->type() == MessageUrl){
            UrlMessage *m = static_cast<UrlMessage*>(msg);
            QString msgText = m->getUrl();
            msgText += "\r\n";
            msgText += m_msgText;
            m_msgText = msgText;
        }
        if ((msg->type() == MessageFile) && static_cast<FileMessage*>(msg)->m_transfer)
            m_msgText = "";
        if (m_msgText.isEmpty()){
            if (msg->type() == MessageFile){
                sendFile();
                return;
            }
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            m_queue.erase(m_queue.begin());
        }
        m_msgText = m_msgText.replace(QRegExp("\\n"), "\r\n");
    }
    if (m_msgText.isEmpty())
        return;
    m_msgPart = getPart(m_msgText, 1664);
    Message *msg = m_queue.front();
    char color[10];
    sprintf(color, "%06lX", msg->getBackground());
    QString message;
    message += "MIME-Version: 1.0\r\n";
    message += "Content-Type: text/plain; charset=UTF-8\r\n";
    message += "X-MMS_IM-Format: ";
    if (msg->getFont()){
        QString font = msg->getFont();
        if (!font.isEmpty()){
            QString font_type;
            int n = font.find(", ");
            if (n > 0){
                font_type = font.mid(n + 2);
                font = font.left(n);
            }
            message += "FN=";
            message += m_client->quote(font);
            string effect;
            while (!font_type.isEmpty()){
                QString type = font_type;
                int n = font_type.find(", ");
                if (n > 0){
                    type = font_type.mid(n);
                    font_type = font_type.mid(n + 2);
                }else{
                    font_type = "";
                }
                if (type == "bold")
                    effect += "B";
                if (type == "italic")
                    effect += "I";
                if (type == "strikeout")
                    effect += "S";
                if (type == "underline")
                    effect += "U";
            }
            if (!effect.empty()){
                message += "; EF=";
                message += effect;
            }
        }
    }
    message += "; CO=";
    message += color;
    message += "; CS=0\r\n";
    message += "\r\n";
    message += m_msgPart;
    sendMessage(message, "A");
    m_msg_id = m_packet_id;
}

MSNFileTransfer::MSNFileTransfer(FileMessage *msg, MSNClient *client, MSNUserData *data)
        : FileTransfer(msg)
{
    m_socket = new ClientSocket(this);
    m_client = client;
    m_state  = None;
    m_data	 = data;
    m_timer  = NULL;
    m_size   = msg->getSize();
    m_bHeader  = false;
    m_nFiles   = 1;
}

MSNFileTransfer::~MSNFileTransfer()
{
    if (m_socket)
        delete m_socket;
}


void MSNFileTransfer::setSocket(Socket *s)
{
    m_state  = Incoming;
    m_socket->setSocket(s);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    send("VER MSNFTP");
    FileTransfer::m_state = FileTransfer::Negotiation;
    if (m_notify)
        m_notify->process();
}

void MSNFileTransfer::listen()
{
    if (m_notify)
        m_notify->createFile(m_msg->getDescription(), m_size, false);
}

void MSNFileTransfer::connect()
{
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
    if ((m_state == None) || (m_state == Wait)){
        m_state = ConnectIP1;
        if (ip1 && port1){
            struct in_addr addr;
            addr.s_addr = ip1;
            m_socket->connect(inet_ntoa(addr), port1, NULL);
            return;
        }
    }
    if (m_state == ConnectIP1){
        m_state = ConnectIP2;
        if (ip2 && port2){
            struct in_addr addr;
            addr.s_addr = ip2;
            m_socket->connect(inet_ntoa(addr), port2, NULL);
            return;
        }
    }
    if (m_state == ConnectIP2){
        m_state = ConnectIP3;
        if (ip2 && port1){
            struct in_addr addr;
            addr.s_addr = ip2;
            m_socket->connect(inet_ntoa(addr), port1, NULL);
            return;
        }
    }
    error_state(I18N_NOOP("Can't established direct connection"), 0);
}

bool MSNFileTransfer::error_state(const QString &err, unsigned)
{
    if (m_state == WaitDisconnect)
        FileTransfer::m_state = FileTransfer::Done;
    if (m_state == ConnectIP1){
        connect();
        return false;
    }
    if (m_state == Wait)
        return false;
    if (FileTransfer::m_state != FileTransfer::Done){
        m_state = None;
        FileTransfer::m_state = FileTransfer::Error;
        m_msg->setError(err);
    }
    m_msg->m_transfer = NULL;
    m_msg->setFlags(m_msg->getFlags() & ~MESSAGE_TEMP);
    Event e(EventMessageSent, m_msg);
    e.process();
    return true;
}

void MSNFileTransfer::packet_ready()
{
    if (m_state == Receive){
        if (m_bHeader){
            char cmd;
            char s1, s2;
            m_socket->readBuffer >> cmd >> s1 >> s2;
            log(L_DEBUG, "MSN FT header: %02X %02X %02X", cmd & 0xFF, s1 & 0xFF, s2 & 0xFF);
            if (cmd != 0){
                m_socket->error_state(I18N_NOOP("Transfer canceled"), 0);
                return;
            }
            unsigned size = (unsigned char)s1 + ((unsigned char)s2 << 8);
            m_bHeader = false;
            log(L_DEBUG, "MSN FT header: %u", size);
            m_socket->readBuffer.init(size);
        }else{
            unsigned size = m_socket->readBuffer.size();
            if (size == 0)
                return;
            log(L_DEBUG, "MSN FT data: %u", size);
            m_file->writeBlock(m_socket->readBuffer.data(), size);
            m_socket->readBuffer.incReadPos(size);
            m_bytes      += size;
            m_totalBytes += size;
            m_transferBytes += size;
            if (m_notify)
                m_notify->process();
            m_size -= size;
            if (m_size <= 0){
                m_socket->readBuffer.init(0);
                m_socket->setRaw(true);
                send("BYE 16777989");
                m_state = WaitDisconnect;
                if (m_notify)
                    m_notify->transfer(false);
                return;
            }
            m_bHeader = true;
            m_socket->readBuffer.init(3);
        }
        return;
    }
    if (m_socket->readBuffer.writePos() == 0)
        return;
    MSNPlugin *plugin = static_cast<MSNPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->MSNPacket);
    for (;;){
        QCString s;
        if (!m_socket->readBuffer.scan("\r\n", s))
            break;
        if (getLine(s))
            return;
    }
    if (m_socket->readBuffer.readPos() == m_socket->readBuffer.writePos())
        m_socket->readBuffer.init(0);
}

void MSNFileTransfer::connect_ready()
{
    log(L_DEBUG, "Connect ready");
    m_state = Connected;
    FileTransfer::m_state = Negotiation;
    if (m_notify)
        m_notify->process();
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
}

void MSNFileTransfer::startReceive(unsigned pos)
{
    if (pos > m_size){
        SBSocket *sock = dynamic_cast<SBSocket*>(m_data->sb.object());
        FileTransfer::m_state = FileTransfer::Done;
        m_state = None;
        if (sock)
            sock->declineMessage(cookie);
        m_socket->error_state("", 0);
        return;
    }
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer->start(FT_TIMEOUT * 1000);
    m_state = Listen;
    FileTransfer::m_state = FileTransfer::Listen;
    if (m_notify)
        m_notify->process();
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void MSNFileTransfer::send(const QString &line)
{
    log(L_DEBUG, "Send: %s", line.local8Bit().data());
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << (const char*)line.utf8();
    m_socket->writeBuffer << "\r\n";
    MSNPlugin *plugin = static_cast<MSNPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->MSNPacket);
    m_socket->write();
}

bool MSNFileTransfer::getLine(const QCString &line)
{
    QString l = QString::fromUtf8(line);
    l = l.replace(QRegExp("\r"), "");
    QCString ll = l.local8Bit();
    log(L_DEBUG, "Get: %s", (const char*)ll);
    QString cmd = getToken(l, ' ');
    if ((cmd == "VER") && (l == "MSNFTP")){
        if (m_state == Incoming){
            QString usr = "USR ";
            usr += m_client->quote(m_client->data.owner.EMail.str());
            usr += " ";
            usr += QString::number(auth_cookie);
            send(usr);
        }else{
            send("VER MSNFTP");
        }
        return false;
    }
    if (cmd == "USR"){
        QString mail = m_client->unquote(getToken(l, ' '));
        unsigned auth = l.toUInt();
        if (mail.lower() != m_data->EMail.str().lower()){
            error_state("Bad address", 0);
            return false;
        }
        if (auth != auth_cookie){
            error_state("Bad auth cookie", 0);
            return false;
        }
        if (m_file == NULL){
            for (;;){
                if (!openFile()){
                    if (FileTransfer::m_state == FileTransfer::Done)
                        m_socket->error_state("");
                    if (m_notify)
                        m_notify->transfer(false);
                    return false;
                }
                if (!isDirectory())
                    break;
            }
        }
        QString cmd = "FIL ";
        cmd += QString::number(m_fileSize);
        send(cmd);
        return false;
    }
    if (cmd == "TFR"){
        FileTransfer::m_state = FileTransfer::Write;
        m_state = Send;
        if (m_notify)
            m_notify->transfer(true);
        write_ready();
        return false;
    }
    if (cmd == "FIL"){
        send("TFR");
        m_bHeader = true;
        m_socket->readBuffer.init(3);
        m_socket->readBuffer.packetStart();
        m_state = Receive;
        m_socket->setRaw(false);
        FileTransfer::m_state = FileTransfer::Read;
        m_size = l.toULong();
        m_bytes = 0;
        if (m_notify){
            m_notify->transfer(true);
            m_notify->process();
        }
        return true;
    }
    if (cmd == "BYE"){
        if (m_notify)
            m_notify->transfer(false);
        for (;;){
            if (!openFile()){
                if (FileTransfer::m_state == FileTransfer::Done)
                    m_socket->error_state("");
                return true;
            }
            if (isDirectory())
                continue;
            m_state = Wait;
            FileTransfer::m_state = FileTransfer::Wait;
            if (!((Client*)m_client)->send(m_msg, m_data))
                error_state(I18N_NOOP("File transfer failed"), 0);
        }
        if (m_notify)
            m_notify->process();
        m_socket->close();
        return true;
    }
    error_state("Bad line", 0);
    return false;
}

void MSNFileTransfer::timeout()
{
}

void MSNFileTransfer::write_ready()
{
    if (m_state != Send){
        ClientSocketNotify::write_ready();
        return;
    }
    if (m_transfer){
        m_transferBytes += m_transfer;
        m_transfer = 0;
        if (m_notify)
            m_notify->process();
    }
    if (m_bytes >= m_fileSize){
        m_state = WaitBye;
        return;
    }
    time_t now = time(NULL);
    if ((unsigned)now != m_sendTime){
        m_sendTime = now;
        m_sendSize = 0;
    }
    if (m_sendSize > (m_speed << 18)){
        m_socket->pause(1);
        return;
    }
    unsigned long tail = m_fileSize - m_bytes;
    if (tail > MAX_FT_PACKET) tail = MAX_FT_PACKET;
    m_socket->writeBuffer.packetStart();
    char buf[MAX_FT_PACKET + 3];
    buf[0] = 0;
    buf[1] = (char)(tail & 0xFF);
    buf[2] = (char)((tail >> 8) & 0xFF);
    int readn = m_file->readBlock(&buf[3], tail);
    if (readn <= 0){
        m_socket->error_state("Read file error");
        return;
    }
    m_transfer    = readn;
    m_bytes      += readn;
    m_totalBytes += readn;
    m_sendSize   += readn;
    m_socket->writeBuffer.pack(buf, readn + 3);
    m_socket->write();
}

bool MSNFileTransfer::accept(Socket *s, unsigned long ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    log(L_DEBUG, "Accept direct connection %s", inet_ntoa(addr));
    m_socket->setSocket(s);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    FileTransfer::m_state = Negotiation;
    m_state = Incoming;
    if (m_notify)
        m_notify->process();
    send("VER MSNFTP");
    return true;
}

void MSNFileTransfer::bind_ready(unsigned short port)
{
    SBSocket *sock = dynamic_cast<SBSocket*>(m_data->sb.object());
    if (sock == NULL){
        error_state("No switchboard socket", 0);
        return;
    }
    sock->acceptMessage(port, cookie, auth_cookie);
}

bool MSNFileTransfer::error(const char *err)
{
    return error_state(QString::fromUtf8(err), 0);
}

void SBSocket::acceptMessage(unsigned short port, unsigned cookie, unsigned auth_cookie)
{
    QString message;
    message += "MIME-Version: 1.0\r\n"
               "Content-Type: text/x-msmsgsinvite; charset=UTF-8\r\n\r\n"
               "IP-Address: ";
    struct in_addr addr;
    addr.s_addr = get_ip(m_client->data.owner.IP);
    message += inet_ntoa(addr);
    message += "\r\n"
               "IP-Address-Internal: ";
    addr.s_addr = m_client->m_socket->localHost();
    message += inet_ntoa(addr);
    message += "\r\n"
               "Port: ";
    message += QString::number(port);
    message += "\r\n"
               "AuthCookie: ";
    message += QString::number(auth_cookie);
    message += "\r\n"
               "Sender-Connect: TRUE\r\n"
               "Invitation-Command: ACCEPT\r\n"
               "Invitation-Cookie: ";
    message += QString::number(cookie);
    message += "\r\n"
               "Launch-Application: FALSE\r\n"
               "Request-Data: IP-Address:\r\n\r\n";
    sendMessage(message, "N");
}

bool SBSocket::acceptMessage(Message *msg, const QString &dir, OverwriteMode mode)
{
    for (list<msgInvite>::iterator it = m_acceptMsg.begin(); it != m_acceptMsg.end(); ++it){
        if ((*it).msg->id() != msg->id())
            continue;
        Message *msg = (*it).msg;
        unsigned cookie = (*it).cookie;
        m_acceptMsg.erase(it);
        MSNFileTransfer *ft = new MSNFileTransfer(static_cast<FileMessage*>(msg), m_client, m_data);
        ft->setDir(dir);
        ft->setOverwrite(mode);
        ft->auth_cookie = get_random();
        ft->cookie = cookie;
        Event e(EventMessageAcked, msg);
        e.process();
        ft->listen();
        Event eDel(EventMessageDeleted, msg);
        eDel.process();
        return true;
    }
    return false;
}

void SBSocket::declineMessage(unsigned cookie)
{
    QString message;
    message += "MIME-Version: 1.0\r\n"
               "Content-Type: text/x-msmsgsinvite; charset=UTF-8\r\n\r\n"
               "Invitation-Command: CANCEL\r\n"
               "Invitation-Cookie: ";
    message += QString::number(cookie);
    message += "\r\n"
               "Cancel-Code: REJECT\r\n\r\n";
    sendMessage(message, "S");
}

bool SBSocket::declineMessage(Message *msg, const QString &reason)
{
    for (list<msgInvite>::iterator it = m_acceptMsg.begin(); it != m_acceptMsg.end(); ++it){
        if ((*it).msg->id() != msg->id())
            continue;
        Message *msg = (*it).msg;
        unsigned cookie = (*it).cookie;
        m_acceptMsg.erase(it);
        declineMessage(cookie);
        if (!reason.isEmpty()){
            Message *msg = new Message(MessageGeneric);
            msg->setText(reason);
            msg->setFlags(MESSAGE_NOHISTORY);
            if (!m_client->send(msg, m_data))
                delete msg;
        }
        delete msg;
        return true;
    }
    return false;
}

#ifndef NO_MOC_INCLUDES
#include "msnclient.moc"
#endif
