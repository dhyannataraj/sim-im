/***************************************************************************
                          jabberclient.cpp  -  description
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
#include "jabberconfig.h"
#include "jabber_ssl.h"
#include "jabberadd.h"
#include "jabberinfo.h"
#include "jabberhomeinfo.h"
#include "jabberworkinfo.h"
#include "jabberaboutinfo.h"
#include "jabberpicture.h"
#include "jabbermessage.h"
#include "jabberbrowser.h"
#include "infoproxy.h"
#include "html.h"
#include "icons.h"

#include "core.h"

#include <qtimer.h>
#include <qregexp.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qapplication.h>
#include <qwidgetlist.h>

#include <time.h>

#ifndef WIN32
#include <ctype.h>
#endif

using namespace std;
using namespace SIM;

#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

unsigned PING_TIMEOUT = 50;

/*
typedef struct JabberUserData
{
    char		*ID;
    char		*Resource;
	unsigned	Status;
	char		*FirstName;
	char		*Nick;
	char		*Desc;
	char		*Bday;
	char		*Url;
	char		*OrgName;
	char		*OrgUnit;
	char		*Title;
	char		*Role;
	char		*Street;
	char		*City;
	char		*Region;
	char		*PCode;
	char		*Country;
} JabberUserData;
*/

DataDef jabberUserData[] =
    {
        { "", DATA_ULONG, 1, DATA(2) },		// Sign
        { "LastSend", DATA_ULONG, 1, 0 },
        { "ID", DATA_UTF, 1, 0 },
        { "Node", DATA_UTF, 1, 0 },
        { "Resource", DATA_UTF, 1, 0 },
        { "Name", DATA_UTF, 1, 0 },
        { "", DATA_ULONG, 1, DATA(1) },		// Status
        { "FirstName", DATA_UTF, 1, 0 },
        { "Nick", DATA_UTF, 1, 0 },
        { "Desc", DATA_UTF, 1, 0 },
        { "BirthDay", DATA_UTF, 1, 0 },
        { "Url", DATA_UTF, 1, 0 },
        { "OrgName", DATA_UTF, 1, 0 },
        { "OrgUnit", DATA_UTF, 1, 0 },
        { "Role", DATA_UTF, 1, 0 },
        { "Title", DATA_UTF, 1, 0 },
        { "Street", DATA_UTF, 1, 0 },
        { "ExtAddr", DATA_UTF, 1, 0 },
        { "City", DATA_UTF, 1, 0 },
        { "Region", DATA_UTF, 1, 0 },
        { "PCode", DATA_UTF, 1, 0 },
        { "Country", DATA_UTF, 1, 0 },
        { "EMail", DATA_UTF, 1, 0 },
        { "Phone", DATA_UTF, 1, 0 },
        { "StatusTime", DATA_ULONG, 1, 0 },
        { "OnlineTime", DATA_ULONG, 1, 0 },
        { "Subscribe", DATA_ULONG, 1, 0 },
        { "Group", DATA_UTF, 1, 0 },
        { "", DATA_BOOL, 1, 0 },			// bChecked
        { "", DATA_STRING, 1, 0 },			// TypingId
        { "", DATA_ULONG, 1, 0 },			// ComposeId
        { "", DATA_ULONG, 1, DATA(1) },			// richText
        { "", DATA_BOOL, 1, 0 },
        { "PhotoWidth", DATA_ULONG, 1, 0 },
        { "PhotoHeight", DATA_ULONG, 1, 0 },
        { "LogoWidth", DATA_ULONG, 1, 0 },
        { "LogoHeight", DATA_ULONG, 1, 0 },
        { "", DATA_ULONG, 1, 0 },			// nResources
        { "", DATA_STRLIST, 1, 0 },			// Resources
        { "", DATA_STRLIST, 1, 0 },			// ResourceStatus
        { "", DATA_STRLIST, 1, 0 },			// ResourceReply
        { "", DATA_STRLIST, 1, 0 },			// ResourceStatusTime
        { "", DATA_STRLIST, 1, 0 },			// ResourceOnlineTime
        { "AutoReply", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

/*
typedef struct JabberClientData
{
    char		*ID;
    char		*Server;
    unsigned	Port;
	unsigned	UseSSL;
	unsigned	UsePlain;
	unsigned	UseVHost;
	unsigned	Register;
	char		*ListRequest;
} JabberClientData;
*/
static DataDef jabberClientData[] =
    {
        { "Server", DATA_STRING, 1, "jabber.org" },
        { "Port", DATA_ULONG, 1, DATA(5222) },
        { "UseSSL", DATA_BOOL, 1, 0 },
        { "UsePlain", DATA_BOOL, 1, 0 },
        { "UseVHost", DATA_BOOL, 1, 0 },
        { "", DATA_BOOL, 1, 0 },
        { "Priority", DATA_ULONG, 1, DATA(5) },
        { "ListRequest", DATA_UTF, 1, 0 },
        { "VHost", DATA_UTF, 1, 0 },
        { "Typing", DATA_BOOL, 1, DATA(1) },
        { "RichText", DATA_BOOL, 1, DATA(1) },
        { "ProtocolIcons", DATA_BOOL, 1, DATA(1) },
        { "MinPort", DATA_ULONG, 1, DATA(1024) },
        { "MaxPort", DATA_ULONG, 1, DATA(0xFFFF) },
        { "Photo", DATA_UTF, 1, 0 },
        { "Logo", DATA_UTF, 1, 0 },
        { "AutoSubscribe", DATA_BOOL, 1, DATA(1) },
        { "AutoAccept", DATA_BOOL, 1, DATA(1) },
        { "UseHTTP", DATA_BOOL, 1, 0 },
        { "URL", DATA_STRING, 1, 0 },
        { "InfoUpdated", DATA_BOOL, 1, 0 },
        { "", DATA_STRUCT, sizeof(JabberUserData) / sizeof(Data), DATA(jabberUserData) },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

JabberClient::JabberClient(JabberProtocol *protocol, ConfigBuffer *cfg)
        : TCPClient(protocol, cfg)
{
    load_data(jabberClientData, &data, cfg);
    QString jid = data.owner.ID.str();
    int n = jid.find("@");
    if (n > 0){
        jid = jid.left(n);
        data.owner.ID.str() = jid;
    }
    if (data.owner.Resource.str().isEmpty()){
        QString resource = PACKAGE;
        resource += "_";
        resource += VERSION;
#ifdef WIN32
        resource += "/win32";
#endif
        data.owner.Resource.str() = resource;
    }

    QString listRequests = getListRequest();
    while (!listRequests.isEmpty()){
        QString item = getToken(listRequests, ';', false);
        JabberListRequest lr;
        lr.bDelete = false;
        lr.jid = getToken(item, ',').utf8();
        lr.grp = getToken(item, ',').utf8();
        if (!item.isEmpty())
            lr.bDelete = true;
        m_listRequests.push_back(lr);
    }
    setListRequest(NULL);

    m_bSSL		 = false;
    m_curRequest = NULL;
    m_msg_id	 = 0;
    m_bJoin		 = false;
    init();
}

JabberClient::~JabberClient()
{
    TCPClient::setStatus(STATUS_OFFLINE, false);
    free_data(jabberClientData, &data);
    freeData();
}

const DataDef *JabberProtocol::userDataDef()
{
    return jabberUserData;
}

bool JabberClient::compareData(void *d1, void *d2)
{
    JabberUserData *data1 = (JabberUserData*)d1;
    JabberUserData *data2 = (JabberUserData*)d2;
    return (data1->ID.str() == data2->ID.str());
}

void JabberClient::setID(const QString &id)
{
    data.owner.ID.str() = id;
}

QString JabberClient::getConfig()
{
    QString lr;
    for (list<JabberListRequest>::iterator it = m_listRequests.begin(); it != m_listRequests.end(); ++it){
        if (!lr.isEmpty())
            lr += ";";
        lr += quoteChars((*it).jid, ",;");
        lr += ",";
        lr += quoteChars((*it).grp, ",;");
        if ((*it).bDelete)
            lr += ",1";
    }
    setListRequest(lr);
    QString res = Client::getConfig();
    if (res.length())
        res += "\n";
    return res += save_data(jabberClientData, &data);
}

QString JabberClient::name()
{
    QString res = "Jabber.";
    if (data.owner.ID.toULong()){
        QString server;
        if (getUseVHost())
            server = getVHost();
        if (server.isEmpty())
            server = getServer();
        res += data.owner.ID.str();
        res += '@';
        res += server;
    }
    return res;
}

QWidget	*JabberClient::setupWnd()
{
    return new JabberConfig(NULL, this, false);
}

bool JabberClient::isMyData(clientData *&_data, Contact *&contact)
{
    if (_data->Sign.toULong() != JABBER_SIGN)
        return false;
    string resource;
    JabberUserData *data = (JabberUserData*)_data;
    JabberUserData *my_data = findContact(data->ID.str().utf8(), NULL, false, contact, resource);
    if (my_data){
        data = my_data;
    }else{
        contact = NULL;
    }
    return true;
}

bool JabberClient::createData(clientData *&_data, Contact *contact)
{
    JabberUserData *data = (JabberUserData*)_data;
    JabberUserData *new_data = (JabberUserData*)(contact->clientData.createData(this));
    new_data->ID.str() = data->ID.str();
    _data = (clientData*)new_data;
    return true;
}

void JabberClient::connect_ready()
{
    if (!getUseSSL() || m_bSSL){
        connected();
        return;
    }
#ifdef USE_OPENSSL
    m_bSSL = true;
    SSLClient *ssl = new JabberSSL(m_socket->socket());
    m_socket->setSocket(ssl);
    if (!ssl->init()){
        m_socket->error_state("SSL init error");
        return;
    }
    ssl->connect();
    ssl->process();
#endif
}

void JabberClient::connected()
{
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    log(L_DEBUG, "Connect ready");
    startHandshake();
    TCPClient::connect_ready();
    reset();
}

void JabberClient::packet_ready()
{
    if (m_socket->readBuffer.writePos() == 0)
        return;
    JabberPlugin *plugin = static_cast<JabberPlugin*>(protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->JabberPacket);
    if (!parse(m_socket->readBuffer.data(), m_socket->readBuffer.size(), true))
        m_socket->error_state("XML parse error");
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
}

void *JabberClient::processEvent(Event *e)
{
    TCPClient::processEvent(e);
    if (e->type() == EventAddContact){
        addContact *ac = (addContact*)(e->param());
        if (ac->proto && !strcmp(protocol()->description()->text, ac->proto)){
            Contact *contact = NULL;
            string resource;
            findContact(ac->addr, ac->nick, true, contact, resource);
            if (contact && contact->getGroup() != ac->group){
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
            JabberUserData *data;
            ClientDataIterator itc(contact->clientData, this);
            while ((data = (JabberUserData*)(++itc)) != NULL){
                if (data->ID.str() == QString::fromUtf8(addr)){
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
    if (e->type() == EventGoURL){
        QString url = (const char*)(e->param());
        QString proto;
        int n = url.find(':');
        if (n < 0)
            return NULL;
        proto = url.left(n);
        if (proto != "jabber")
            return NULL;
        url = url.mid(n + 1);
        while (url[0] == '/')
            url = url.mid(1);
        QString s = unquoteString(url);
        QString jid = getToken(s, '/');
        if (!jid.isEmpty()){
            Contact *contact;
            string resource;
            findContact(jid.utf8(), s.utf8(), true, contact, resource);
            Command cmd;
            cmd->id		 = MessageGeneric;
            cmd->menu_id = MenuMessage;
            cmd->param	 = (void*)(contact->id());
            Event eCmd(EventCommandExec, cmd);
            eCmd.process();
            return e->param();
        }
    }
    if (e->type() == EventTemplateExpanded){
        TemplateExpand *t = (TemplateExpand*)(e->param());
        setStatus((unsigned long)(t->param), quoteString(t->tmpl, quoteNOBR).utf8());
    }
    if (e->type() == EventContactChanged){
        Contact *contact = (Contact*)(e->param());
        QString grpName;
        QString name;
        name = contact->getName();
        Group *grp = NULL;
        if (contact->getGroup())
            grp = getContacts()->group(contact->getGroup());
        if (grp)
            grpName = grp->getName();
        ClientDataIterator it(contact->clientData, this);
        JabberUserData *data;
        while ((data = (JabberUserData*)(++it)) != NULL){
            if (grpName == data->Group.str()){
                listRequest(data, name.utf8(), grpName.utf8(), false);
                continue;
            }
            if (!data->Name.str().isEmpty()){
                if (name == data->Name.str())
                    listRequest(data, name.utf8(), grpName.utf8(), false);
                continue;
            }
            if (name == data->ID.str())
                listRequest(data, name.utf8(), grpName.utf8(), false);
        }
        return NULL;
    }
    if (e->type() == EventContactDeleted){
        Contact *contact = (Contact*)(e->param());
        ClientDataIterator it(contact->clientData, this);
        JabberUserData *data;
        while ((data = (JabberUserData*)(++it)) != NULL){
            listRequest(data, NULL, NULL, true);
        }
        return NULL;
    }
    if (e->type() == EventGroupChanged){
        Group *grp = (Group*)(e->param());
        QString grpName;
        grpName = grp->getName();
        ContactList::ContactIterator itc;
        Contact *contact;
        while ((contact = ++itc) != NULL){
            if (contact->getGroup() != grp->id())
                continue;
            ClientDataIterator it(contact->clientData, this);
            JabberUserData *data;
            while ((data = (JabberUserData*)(++it)) != NULL){
                if (grpName == data->Group.str())
                    listRequest(data, contact->getName().utf8(), grpName.utf8(), false);
            }
        }
    }
    if (e->type() == EventMessageCancel){
        Message *msg = (Message*)(e->param());
        for (list<Message*>::iterator it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
            if ((*it) == msg){
                m_waitMsg.erase(it);
                delete msg;
                return e->param();
            }
        }
        return NULL;
    }
    if (e->type() == EventMessageAccept){
        messageAccept *ma = (messageAccept*)(e->param());
        for (list<Message*>::iterator it = m_ackMsg.begin(); it != m_ackMsg.end(); ++it){
            if ((*it)->id() == ma->msg->id()){
                JabberFileMessage *msg = static_cast<JabberFileMessage*>(*it);
                m_ackMsg.erase(it);
                Contact *contact;
                string resource;
                JabberUserData *data = findContact(msg->getFrom(), NULL, false, contact, resource);
                if (data){
                    JabberFileTransfer *ft = new JabberFileTransfer(static_cast<FileMessage*>(msg), data, this);
                    ft->setDir(QFile::encodeName(ma->dir));
                    ft->setOverwrite(ma->overwrite);
                    Event e(EventMessageAcked, msg);
                    e.process();
                    ft->connect();
                }
                Event eDel(EventMessageDeleted, msg);
                eDel.process();
                if (data == NULL)
                    delete msg;
                return msg;
            }
        }
        return NULL;
    }
    if (e->type() == EventMessageDecline){
        messageDecline *md = (messageDecline*)(e->param());
        for (list<Message*>::iterator it = m_ackMsg.begin(); it != m_ackMsg.end(); ++it){
            if ((*it)->id() == md->msg->id()){
                JabberFileMessage *msg = static_cast<JabberFileMessage*>(*it);
                m_ackMsg.erase(it);
                QString reason = "File transfer declined";
                if (md->reason)
                    reason = md->reason;
                ServerRequest req(this, "error", NULL, msg->getFrom(), msg->getID());
                req.start_element("error");
                req.add_attribute("code", "403");
                req.add_text(reason);
                req.send();
                Event e(EventMessageDeleted, msg);
                e.process();
                delete msg;
                return msg;
            }
        }
        return NULL;
    }
    return NULL;
}

void JabberClient::setStatus(unsigned status)
{
    if (getInvisible() && (status != STATUS_OFFLINE)){
        if (m_status != status){
            m_status = status;
            Event e(EventClientChanged, static_cast<Client*>(this));
            e.process();
        }
        return;
    }
    ARRequest ar;
    ar.contact  = NULL;
    ar.status   = status;
    ar.receiver = this;
    ar.param	= (void*)(long)status;
    Event e(EventARRequest, &ar);
    e.process();
}

void JabberClient::setStatus(unsigned status, const char *ar)
{
    if (status  != m_status){
        time_t now;
        time(&now);
        data.owner.StatusTime.asULong() = now;
        if (m_status == STATUS_OFFLINE)
            data.owner.OnlineTime.asULong() = now;
        m_status = status;
        m_socket->writeBuffer.packetStart();
		QString priority = QString::number(getPriority());
        const char *show = NULL;
        const char *type = NULL;
        if (getInvisible()){
            type = "invisible";
        }else{
            switch (status){
            case STATUS_AWAY:
                show = "away";
                break;
            case STATUS_NA:
                show = "xa";
                break;
            case STATUS_DND:
                show = "dnd";
                break;
            case STATUS_FFC:
                show = "chat";
                break;
            case STATUS_OFFLINE:
                priority = "";
                type = "unavailable";
                break;
            }
        }
        m_socket->writeBuffer << "<presence";
        if (type)
            m_socket->writeBuffer << " type=\'" << type << "\'";
        m_socket->writeBuffer << ">\n";
        if (show && *show)
            m_socket->writeBuffer << "<show>" << show << "</show>\n";
        if (ar && *ar){
            m_socket->writeBuffer << "<status>" << ar << "</status>\n";
        }
        if (!priority.isEmpty())
            m_socket->writeBuffer << "<priority>" << (const char*)priority.utf8() << "</priority>\n";
        m_socket->writeBuffer << "</presence>";
        sendPacket();
        Event e(EventClientChanged, static_cast<Client*>(this));
        e.process();
    }
    if (status == STATUS_OFFLINE){
        if (m_socket){
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "</stream:stream>\n";
            sendPacket();
        }
        Contact *contact;
        ContactList::ContactIterator it;
        time_t now;
        time(&now);
        data.owner.StatusTime.asULong() = now;
        while ((contact = ++it) != NULL){
            JabberUserData *data;
            ClientDataIterator it(contact->clientData, this);
            while ((data = (JabberUserData*)(++it)) != NULL){
                if (data->Status.toULong() == STATUS_OFFLINE)
                    continue;
                data->StatusTime.asULong() = now;
                setOffline(data);
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
}

void JabberClient::disconnected()
{
    for (list<ServerRequest*>::iterator it = m_requests.begin(); it != m_requests.end(); ++it)
        delete *it;
    m_requests.clear();
    if (m_curRequest){
        delete m_curRequest;
        m_curRequest = NULL;
    }
    list<Message*>::iterator itm;
    for (itm = m_ackMsg.begin(); itm != m_ackMsg.end(); ++itm){
        Message *msg = *itm;
        Event e(EventMessageDeleted, msg);
        e.process();
        delete msg;
    }
    for (itm = m_waitMsg.begin(); itm != m_waitMsg.end(); itm = m_waitMsg.begin()){
        Message *msg = *itm;
        msg->setError(I18N_NOOP("Client go offline"));
        Event e(EventMessageSent, msg);
        e.process();
        delete msg;
    }
    m_ackMsg.clear();
    init();
}

void JabberClient::init()
{
    m_id = "";
    m_depth = 0;
    m_id_seed = 0xAAAA;
    m_bSSL = false;
}

void JabberClient::sendPacket()
{
    JabberPlugin *plugin = static_cast<JabberPlugin*>(protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->JabberPacket);
    m_socket->write();
}

string JabberClient::get_attr(const char *name, const char **attr)
{
    if (attr == NULL)
        return "";
    for (const char **p = attr; *p; ){
        string tag = to_lower(*(p++));
        if (tag == name){
            return *p;
        }
    }
    return "";
}

void JabberClient::element_start(const char *el, const char **attr)
{
    string element = to_lower(el);
    const char *id = NULL;
    if (m_depth){
        if (m_curRequest){
            m_curRequest->element_start(element.c_str(), attr);
        }else{
            if (element == "iq"){
                string id = get_attr("id", attr);
                string type = get_attr("type", attr);
                if (id.empty() || (type == "set") || (type == "get")){
                    m_curRequest = new IqRequest(this);
                    m_curRequest->element_start(element.c_str(), attr);
                }else{
                    list<ServerRequest*>::iterator it;
                    for (it = m_requests.begin(); it != m_requests.end(); ++it){
                        if ((*it)->m_id == id)
                            break;
                    }
                    if (it != m_requests.end()){
                        m_curRequest = *it;
                        m_requests.erase(it);
                        m_curRequest->element_start(element.c_str(), attr);
                    }else{
                        log(L_WARN, "Packet %s not found", id.c_str());
                    }
                }
            }else if (element == "presence"){
                m_curRequest = new PresenceRequest(this);
                m_curRequest->element_start(element.c_str(), attr);
            }else if (element == "message"){
                m_curRequest = new MessageRequest(this);
                m_curRequest->element_start(element.c_str(), attr);
            }else if (element != "a"){
                log(L_DEBUG, "Bad tag %s", element.c_str());
            }
        }
    }else{
        if (element == "stream:stream" && attr){
            for (const char **p = attr; *p; ){
                string tag = to_lower(*(p++));
                if (tag == "id"){
                    id = *p;
                    break;
                }
            }
        }
        log(L_DEBUG, "Handshake %s (%s)", id, element.c_str());
        handshake(id);
    }
    m_depth++;
}

void JabberClient::element_end(const char *el)
{
    m_depth--;
    if (m_curRequest){
        string element = to_lower(el);
        m_curRequest->element_end(element.c_str());
        if (m_depth == 1){
            delete m_curRequest;
            m_curRequest = NULL;
        }
    }
}

void JabberClient::char_data(const char *str, int len)
{
    if (m_curRequest)
        m_curRequest->char_data(str, len);
}

QString JabberClient::get_unique_id()
{
    QString s("a");
	s += QString::number(m_id_seed,16);
    m_id_seed += 0x10;
    return s;
}

JabberClient::ServerRequest::ServerRequest(JabberClient *client, const char *type,
        const char *from, const char *to, const char *id)
{
    m_client = client;
    if (type == NULL)
        return;
    if (id){
        m_id = id;
    }else{
        m_id  = m_client->get_unique_id().utf8();
    }
    if (m_client->m_socket == NULL)
        return;
    m_client->m_socket->writeBuffer.packetStart();
    m_client->m_socket->writeBuffer
    << "<iq type=\'" << type << "\' id=\'"
    << m_id.c_str()
    << "\'";;
    if (from)
        m_client->m_socket->writeBuffer << " from=\'" << from << "\'";
    if (to)
        m_client->m_socket->writeBuffer << " to=\'" << to << "\'";
    m_client->m_socket->writeBuffer << ">\n";
}

JabberClient::ServerRequest::~ServerRequest()
{
}

void JabberClient::ServerRequest::send()
{
    end_element(false);
    while (!m_els.empty()){
        end_element(false);
    }
    m_client->m_socket->writeBuffer
    << "</iq>\n";
    m_client->sendPacket();
}

void JabberClient::ServerRequest::element_start(const char*, const char**)
{
}

void JabberClient::ServerRequest::element_end(const char*)
{
}

void JabberClient::ServerRequest::char_data(const char*, int)
{
}

void JabberClient::ServerRequest::start_element(const QString &name)
{
    end_element(true);
    m_client->m_socket->writeBuffer
    << "<" << (const char*)name.utf8();
    m_element = name;
}

void JabberClient::ServerRequest::add_attribute(const QString &name, const QString &value)
{
    m_client->m_socket->writeBuffer
    << " " << (const char*)name.utf8()
	<< "=\'" << (const char*)JabberClient::encodeXML(value).utf8() << "\'";
}

void JabberClient::ServerRequest::end_element(bool bNewLevel)
{
    if (bNewLevel){
        if (m_element.length()){
            m_client->m_socket->writeBuffer << ">\n";
            m_els.push(m_element);
        }
    }else{
        if (m_element.length()){
            m_client->m_socket->writeBuffer << "/>\n";
        }else if (m_els.size()){
            m_element = m_els.top();
            m_els.pop();
            m_client->m_socket->writeBuffer << "</" << (const char*)m_element.utf8() << ">\n";
        }
    }
    m_element = "";
}

void JabberClient::ServerRequest::add_text(const QString &value)
{
    if (m_element.length()){
        m_client->m_socket->writeBuffer << ">";
        m_els.push(m_element);
        m_element = "";
    }
    m_client->m_socket->writeBuffer
    << (const char*)JabberClient::encodeXML(value).utf8();
}

void JabberClient::ServerRequest::text_tag(const QString &name, const QString &value)
{
    if ((value == NULL) || (*value == 0))
        return;
    end_element(true);
    m_client->m_socket->writeBuffer
    << "<" << (const char*)name.utf8() << ">"
    << (const char*)JabberClient::encodeXML(value).utf8()
    << "</" << (const char*)name.utf8() << ">\n";
}

void JabberClient::ServerRequest::add_condition(const QString &condition, bool bXData)
{
    QString cond = condition;
    while (cond.length()){
        QString item = getToken(cond, ';');
        if (item == "x:data"){
            bXData = true;
            start_element("x");
            add_attribute("xmlns", "jabber:x:data");
            add_attribute("type", "submit");
        }
        QString key = getToken(item, '=');
        if (bXData){
            start_element("field");
            add_attribute("var", key);
            text_tag("value", item);
            end_element();
        }else{
            text_tag(key, item);
        }
    }
}

const char *JabberClient::ServerRequest::_GET = "get";
const char *JabberClient::ServerRequest::_SET = "set";
const char *JabberClient::ServerRequest::_RESULT = "result";

void JabberClient::startHandshake()
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer
    << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    << "<stream:stream to=\'"
    << encodeXML(VHost()).ascii()
    << "\' xmlns=\'jabber:client\' xmlns:stream=\'http://etherx.jabber.org/streams\'>\n";
    sendPacket();
}

void JabberClient::handshake(const char *id)
{
    if (id == NULL){
        m_socket->error_state("Bad session ID");
        return;
    }
    m_id = id;
    if (getRegister()){
        auth_register();
    }else{
#ifdef USE_OPENSSL
        if (getUsePlain()){
            auth_plain();
        }else{
            auth_digest();
        }
#else
auth_plain();
#endif
    }
}

void JabberClient::auth_ok()
{
    if (getRegister()){
        setRegister(false);
        setClientStatus(STATUS_OFFLINE);
        TCPClient::setStatus(getManualStatus(), getCommonStatus());
        return;
    }
    setState(Connected);
    setPreviousPassword(NULL);
    rosters_request();
    if (getInfoUpdated()){
        setClientInfo(&data.owner);
    }else{
        info_request(NULL, false);
    }
    setStatus(m_logonStatus);
    QTimer::singleShot(PING_TIMEOUT * 1000, this, SLOT(ping()));
}

void JabberClient::auth_failed()
{
    m_reconnect = NO_RECONNECT;
    m_socket->error_state(I18N_NOOP("Login failed"), AuthError);
}

string JabberClient::to_lower(const char *s)
{
    string res;
    if (s == NULL)
        return res;
    for (; *s; s++)
        res += (char)tolower(*s);
    return res;
}

QString JabberClient::encodeXML(const QString &str)
{
    return quoteString(str, quoteNOBR);
}

JabberUserData *JabberClient::findContact(const char *_jid, const char *name, bool bCreate, Contact *&contact, string &resource, bool bJoin)
{
    resource = "";
    string jid = _jid;
    int n = jid.find('/');
    if (n >= 0){
        resource = jid.substr(n + 1);
        jid = jid.substr(0, n);
    }
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        JabberUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (JabberUserData*)(++it)) != NULL){
            if (QString::fromUtf8(jid.c_str()) != data->ID.str())
                continue;
            if (!resource.empty())
                data->Resource.str() = QString::fromUtf8(resource.c_str());
            if (name)
                data->Name.str() = QString::fromUtf8(name);
            return data;
        }
    }
    if (!bCreate)
        return NULL;
    it.reset();
    QString sname;
    if (name && *name){
        sname = QString::fromUtf8(name);
    }else{
        sname = QString::fromUtf8(jid.c_str());
        int pos = sname.find('@');
        if (pos > 0)
            sname = sname.left(pos);
    }
    if (bJoin){
        while ((contact = ++it) != NULL){
            if (contact->getName().lower() == sname.lower()){
                JabberUserData *data = (JabberUserData*)(contact->clientData.createData(this));
                data->ID.str() = QString::fromUtf8(jid.c_str());
                if (!resource.empty())
                    data->Resource.str() = QString::fromUtf8(resource.c_str());
                if (name)
                    data->Name.str() = QString::fromUtf8(name);
                info_request(data, false);
                Event e(EventContactChanged, contact);
                e.process();
                m_bJoin = true;
                return data;
            }
        }
    }
    contact = getContacts()->contact(0, true);
    JabberUserData *data = (JabberUserData*)(contact->clientData.createData(this));
    data->ID.str() = QString::fromUtf8(jid.c_str());
    if (!resource.empty())
        data->Resource.str() = QString::fromUtf8(resource.c_str());
    if (name)
        data->Name.str() = QString::fromUtf8(name);
    contact->setName(sname);
    info_request(data, false);
    Event e(EventContactChanged, contact);
    e.process();
    return data;
}

static void addIcon(QString *s, const QString &icon, const QString &statusIcon)
{
    if (s == NULL)
        return;
    if (statusIcon == icon)
        return;

    QStringList sl = QStringList::split(',', *s);
    if(sl.findIndex(icon))
        return;

    if (!s->isEmpty())
        *s += ',';
    *s += icon;
}

const char *JabberClient::get_icon(JabberUserData *data, unsigned status, bool invisible)
{
    const CommandDef *def = protocol()->statusList();
    for (; def->text; def++){
        if (def->id == status)
            break;
    }
    if ((def == NULL) || (def->text == NULL))
        return "Jabber_offline";
    const char *dicon = def->icon;
    if (invisible)
        dicon = "Jabber_invisible";
    if (getProtocolIcons()){
        string id = data->ID.str().utf8();
        const char *host = strchr(id.c_str(), '@');
        if (host){
            string h = host + 1;
            char *p = strchr((char*)(h.c_str()), '.');
            if (p)
                *p = 0;
            if (strcmp(h.c_str(), "icq") == 0){
                if (invisible){
                    dicon = "ICQ_invisible";
                }else{
                    switch (status){
                    case STATUS_ONLINE:
                        dicon = "ICQ_online";
                        break;
                    case STATUS_OFFLINE:
                        dicon = "ICQ_offline";
                        break;
                    case STATUS_AWAY:
                        dicon = "ICQ_away";
                        break;
                    case STATUS_NA:
                        dicon = "ICQ_na";
                        break;
                    case STATUS_DND:
                        dicon = "ICQ_dnd";
                        break;
                    case STATUS_FFC:
                        dicon = "ICQ_ffc";
                        break;
                    }
                }
            }else if (strcmp(h.c_str(), "aim") == 0){
                switch (status){
                case STATUS_ONLINE:
                    dicon = "AIM_online";
                    break;
                case STATUS_OFFLINE:
                    dicon = "AIM_offline";
                    break;
                case STATUS_AWAY:
                    dicon = "AIM_away";
                    break;
                }
            }else if (strcmp(h.c_str(), "msn") == 0){
                if (invisible){
                    dicon = "MSN_invisible";
                }else{
                    switch (status){
                    case STATUS_ONLINE:
                        dicon = "MSN_online";
                        break;
                    case STATUS_OFFLINE:
                        dicon = "MSN_offline";
                        break;
                    case STATUS_AWAY:
                        dicon = "MSN_away";
                        break;
                    case STATUS_NA:
                        dicon = "MSN_na";
                        break;
                    case STATUS_DND:
                        dicon = "MSN_dnd";
                        break;
                    }
                }
            }else if (strcmp(h.c_str(), "yahoo") == 0){
                switch (status){
                case STATUS_ONLINE:
                    dicon = "Yahoo!_online";
                    break;
                case STATUS_OFFLINE:
                    dicon = "Yahoo!_offline";
                    break;
                case STATUS_AWAY:
                    dicon = "Yahoo!_away";
                    break;
                case STATUS_NA:
                    dicon = "Yahoo!_na";
                    break;
                case STATUS_DND:
                    dicon = "Yahoo!_dnd";
                    break;
                case STATUS_FFC:
                    dicon = "Yahoo!_ffc";
                    break;
                }
            }
        }
    }
    return dicon;
}

void JabberClient::contactInfo(void *_data, unsigned long &curStatus, unsigned &style, QString &statusIcon, QString *icons)
{
    JabberUserData *data = (JabberUserData*)_data;
    const char *dicon = get_icon(data, data->Status.toULong(), data->invisible.toBool());
    if (data->Status.toULong() > curStatus){
        curStatus = data->Status.toULong();
        if (statusIcon && icons){
            QString iconSave = *icons;
            *icons = statusIcon;
            if (iconSave.length())
                addIcon(icons, iconSave, statusIcon);
        }
        statusIcon = dicon;
    }else{
        if (statusIcon){
            addIcon(icons, dicon, statusIcon);
        }else{
            statusIcon = dicon;
        }
    }
    for (unsigned i = 1; i <= data->nResources.toULong(); i++){
        const char *dicon = get_icon(data, atol(get_str(data->ResourceStatus, i)), false);
        addIcon(icons, dicon, statusIcon);
    }
    if (((data->Subscribe.toULong() & SUBSCRIBE_TO) == 0) && !isAgent(data->ID.str().utf8()))
        style |= CONTACT_UNDERLINE;
    if (icons && !data->TypingId.str().isEmpty())
        addIcon(icons, "typing", statusIcon);
}

QString JabberClient::buildId(JabberUserData *data)
{
    QString res = data->ID.str();
    int n = res.find('@');
    if (n < 0){
        res += "@";
        QString server;
        if (getUseVHost())
            server = getVHost();
        if (server.isEmpty())
            server = getServer();
        res += server;
    }
    return res;
}

QWidget *JabberClient::searchWindow(QWidget *parent)
{
    if (getState() != Connected)
        return NULL;
    return new JabberAdd(this, parent);
}

void JabberClient::ping()
{
    if (getState() != Connected)
        return;
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << "\n";
    sendPacket();
    QTimer::singleShot(PING_TIMEOUT * 1000, this, SLOT(ping()));
}

QString JabberClient::contactName(void *clientData)
{
    QString res = Client::contactName(clientData);
    res += ": ";
    JabberUserData *data = (JabberUserData*)clientData;
    QString name = data->ID.str();
    if (!data->Nick.str().isEmpty()){
        res += data->Nick.str();
        res += " (";
        res += name;
        res += ")";
    }else{
        res += name;
    }
    return res;
}


QString JabberClient::contactTip(void *_data)
{
    JabberUserData *data = (JabberUserData*)_data;
    QString res;
    if (data->nResources.toULong() == 0){
        res = "<img src=\"icon:";
        res += get_icon(data, STATUS_OFFLINE, data->invisible.toBool());
        res += "\">";
        res += i18n("Offline");
        res += "<br/>";
        res += "ID: <b>";
        res += data->ID.str();
        res += "</b>";
        if (!data->Resource.str().isEmpty()){
            res += "<br/>";
            res += data->Resource.str();
        }
        if (data->StatusTime.toULong()){
            res += "<br/><font size=-1>";
            res += i18n("Last online");
            res += ": </font>";
            res += formatDateTime(data->StatusTime.toULong());
        }
        QString &reply = data->AutoReply.str();
        if (!reply.isEmpty()){
            res += "<br/>";
            res += reply.replace(QRegExp("\n"), "<br/>");
        }
    }else{
        for (unsigned i = 1; i <= data->nResources.toULong(); i++){
            unsigned status = atol(get_str(data->ResourceStatus, i));
            res += "<img src=\"icon:";
            res += get_icon(data, status, false);
            res += "\">";
            QString statusText;
            for (const CommandDef *cmd = protocol()->statusList(); !cmd->text.isEmpty(); cmd++){
                if (cmd->id == status){
                    statusText = i18n(cmd->text);
                    res += statusText;
                    break;
                }
            }
            res += "<br/>ID: <b>";
            res += data->ID.str();
            res += "</b><br/>";
            res += QString::fromUtf8(get_str(data->Resources, i));
            res += "<br/>";
            unsigned onlineTime = atol(get_str(data->ResourceOnlineTime, i));
            unsigned statusTime = atol(get_str(data->ResourceStatusTime, i));
            if (onlineTime){
                res += "<br/><font size=-1>";
                res += i18n("Online");
                res += ": </font>";
                res += formatDateTime(onlineTime);
            }
            if (statusTime != onlineTime){
                res += "<br/><font size=-1>";
                res += statusText;
                res += ": </font>";
                res += formatDateTime(statusTime);
            }
            const char *reply = get_str(data->ResourceReply, i);
            if (reply && *reply){
                res += "<br/>";
                QString r = QString::fromUtf8(reply);
                r = r.replace(QRegExp("\n"), "<br/>");
                res += r;
            }
            if (i < data->nResources.toULong())
                res += "<br>_________<br>";
        }
    }

    if (data->LogoWidth.toULong() && data->LogoHeight.toULong()){
        QImage img(logoFile(data));
        if (!img.isNull()){
            QPixmap pict;
            pict.convertFromImage(img);
            int w = pict.width();
            int h = pict.height();
            if (h > w){
                if (h > 60){
                    w = w * 60 / h;
                    h = 60;
                }
            }else{
                if (w > 60){
                    h = h * 60 / w;
                    w = 60;
                }
            }
            QMimeSourceFactory::defaultFactory()->setPixmap("pict://jabber.logo", pict);
            res += "<br/><img src=\"pict://jabber.logo\" width=\"";
			res += QString::number(w);
            res += "\" height=\"";
            res += QString::number(h);
            res += "\">";
        }
    }
    if (data->PhotoWidth.toULong() && data->PhotoHeight.toULong()){
        QImage img(photoFile(data));
        if (!img.isNull()){
            QPixmap pict;
            pict.convertFromImage(img);
            int w = pict.width();
            int h = pict.height();
            if (h > w){
                if (h > 60){
                    w = w * 60 / h;
                    h = 60;
                }
            }else{
                if (w > 60){
                    h = h * 60 / w;
                    w = 60;
                }
            }
            QMimeSourceFactory::defaultFactory()->setPixmap("pict://jabber.photo", pict);
            res += "<br/><img src=\"pict://jabber.photo\" width=\"";
            res += QString::number(w);
            res += "\" height=\"";
            res += QString::number(h);
            res += "\">";
        }
    }
    return res;
}

void JabberClient::setOffline(JabberUserData *data)
{
    data->Status.asULong()    = STATUS_OFFLINE;
    data->composeId.asULong() = 0;
    data->Resources.clear();
    data->ResourceReply.clear();
    data->ResourceStatus.clear();
    data->ResourceStatusTime.clear();
    data->ResourceOnlineTime.clear();
    data->nResources.asULong() = 0;
    if (!data->TypingId.str().isEmpty()){
        data->TypingId.clear();
        Contact *contact;
        string resource;
        if (findContact(data->ID.str().utf8(), NULL, false, contact, resource)){
            Event e(EventContactStatus, contact);
            e.process();
        }
    }
}

const unsigned MAIN_INFO  = 1;
const unsigned HOME_INFO  = 2;
const unsigned WORK_INFO  = 3;
const unsigned ABOUT_INFO = 4;
const unsigned PHOTO_INFO = 5;
const unsigned LOGO_INFO  = 6;
const unsigned NETWORK	  = 7;

static CommandDef jabberWnd[] =
    {
        CommandDef (
            MAIN_INFO,
            " ",
            "Jabber_online",
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
            HOME_INFO,
            I18N_NOOP("Home info"),
            "home",
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
            WORK_INFO,
            I18N_NOOP("Work info"),
            "work",
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
            ABOUT_INFO,
            I18N_NOOP("About info"),
            "info",
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
            PHOTO_INFO,
            I18N_NOOP("Photo"),
            "pict",
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
            LOGO_INFO,
            I18N_NOOP("Logo"),
            "pict",
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

static CommandDef cfgJabberWnd[] =
    {
        CommandDef (
            MAIN_INFO,
            " ",
            "Jabber_online",
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
            HOME_INFO,
            I18N_NOOP("Home info"),
            "home",
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
            WORK_INFO,
            I18N_NOOP("Work info"),
            "work",
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
            ABOUT_INFO,
            I18N_NOOP("About info"),
            "info",
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
            PHOTO_INFO,
            I18N_NOOP("Photo"),
            "pict",
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
            LOGO_INFO,
            I18N_NOOP("Logo"),
            "pict",
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

CommandDef *JabberClient::infoWindows(Contact*, void *_data)
{
    JabberUserData *data = (JabberUserData*)_data;
    QString name = i18n(protocol()->description()->text);
    name += " ";
    name += data->ID.str();
    jabberWnd[0].text_wrk = name;
    return jabberWnd;
}

CommandDef *JabberClient::configWindows()
{
    QString title = name();
    int n = title.find(".");
    if (n > 0)
        title = title.left(n) + " " + title.mid(n + 1);
    cfgJabberWnd[0].text_wrk = title;
    return cfgJabberWnd;
}

QWidget *JabberClient::infoWindow(QWidget *parent, Contact*, void *_data, unsigned id)
{
    JabberUserData *data = (JabberUserData*)_data;
    switch (id){
    case MAIN_INFO:
        return new JabberInfo(parent, data, this);
    case HOME_INFO:
        return new InfoProxy(parent, new JabberHomeInfo(parent, data, this), i18n("Home info"));
    case WORK_INFO:
        return new InfoProxy(parent, new JabberWorkInfo(parent, data, this), i18n("Work info"));
    case ABOUT_INFO:
        return new InfoProxy(parent, new JabberAboutInfo(parent, data, this), i18n("About info"));
    case PHOTO_INFO:
        return new JabberPicture(parent, data, this, true);
    case LOGO_INFO:
        return new JabberPicture(parent, data, this, false);
    }
    return NULL;
}

QWidget *JabberClient::configWindow(QWidget *parent, unsigned id)
{
    switch (id){
    case MAIN_INFO:
        return new JabberInfo(parent, NULL, this);
    case HOME_INFO:
        return new InfoProxy(parent, new JabberHomeInfo(parent, NULL, this), i18n("Home info"));
    case WORK_INFO:
        return new InfoProxy(parent, new JabberWorkInfo(parent, NULL, this), i18n("Work info"));
    case ABOUT_INFO:
        return new InfoProxy(parent, new JabberAboutInfo(parent, NULL, this), i18n("About info"));
    case PHOTO_INFO:
        return new JabberPicture(parent, NULL, this, true);
    case LOGO_INFO:
        return new JabberPicture(parent, NULL, this, false);
    case NETWORK:
        return new JabberConfig(parent, this, true);
    }
    return NULL;
}

void JabberClient::updateInfo(Contact *contact, void *data)
{
    if (getState() != Connected){
        Client::updateInfo(contact, data);
        return;
    }
    if (data == NULL)
        data = &this->data.owner;
    info_request((JabberUserData*)data, false);
}

QString JabberClient::resources(void *_data)
{
    JabberUserData *data = (JabberUserData*)_data;
    QString resource;
    if (data->nResources.toULong() > 1){
        for (unsigned i = 1; i <= data->nResources.toULong(); i++){
            if (!resource.isEmpty())
                resource += ";";
            const char *dicon = get_icon(data, atol(get_str(data->ResourceStatus, i)), false);
            resource += QString::number((unsigned long)dicon);
            resource += ",";
            resource += quoteChars(get_str(data->Resources, i), ";");
        }
    }
    return resource;
}

bool JabberClient::canSend(unsigned type, void *_data)
{
    if ((_data == NULL) || (((clientData*)_data)->Sign.toULong() != JABBER_SIGN))
        return false;
    if (getState() != Connected)
        return false;
    JabberUserData *data = (JabberUserData*)_data;
    switch (type){
    case MessageGeneric:
    case MessageFile:
    case MessageContacts:
    case MessageUrl:
        return true;
    case MessageAuthRequest:
        return ((data->Subscribe.toULong() & SUBSCRIBE_TO) == 0) && !isAgent(data->ID.str().utf8());
    case MessageAuthGranted:
        return ((data->Subscribe.toULong() & SUBSCRIBE_FROM) == 0) && !isAgent(data->ID.str().utf8());
    case MessageAuthRefused:
        return (data->Subscribe.toULong() & SUBSCRIBE_FROM) && !isAgent(data->ID.str().utf8());
    case MessageJabberOnline:
        return isAgent(data->ID.str().utf8()) && (data->Status.toULong() == STATUS_OFFLINE);
    case MessageJabberOffline:
        return isAgent(data->ID.str().utf8()) && (data->Status.toULong() != STATUS_OFFLINE);
    }
    return false;
}

class JabberImageParser : public HTMLParser
{
public:
    JabberImageParser(unsigned bgColor);
    QString parse(const QString &text);
protected:
    virtual void text(const QString &text);
    virtual void tag_start(const QString &tag, const list<QString> &attrs);
    virtual void tag_end(const QString &tag);
    void startBody(const list<QString> &attrs);
    void endBody();
    QString res;
    bool		m_bPara;
    bool		m_bBody;
    unsigned	m_bgColor;
};

JabberImageParser::JabberImageParser(unsigned bgColor)
{
    m_bPara    = false;
    m_bBody    = true;
    m_bgColor  = bgColor;
}

QString JabberImageParser::parse(const QString &text)
{
    list<QString> attrs;
    startBody(attrs);
    HTMLParser::parse(text);
    endBody();
    return res;
}

void JabberImageParser::text(const QString &text)
{
    if (m_bBody)
        res += quoteString(text);
}

static const char *_tags[] =
    {
        "abbr",
        "acronym",
        "address",
        "blockquote",
        "cite",
        "code",
        "dfn",
        "div",
        "em",
        "h1",
        "h2",
        "h3",
        "h4",
        "h5",
        "h6",
        "kbd",
        "p",
        "pre",
        "q",
        "samp",
        "span",
        "strong",
        "var",
        "a",
        "dl",
        "dt",
        "dd",
        "ol",
        "ul",
        "li",
        NULL
    };

static const char *_styles[] =
    {
        "color",
        "background-color",
        "font-family",
        "font-size",
        "font-style",
        "font-weight",
        "text-align",
        "text-decoration",
        NULL
    };

void JabberImageParser::startBody(const list<QString> &attrs)
{
    m_bBody = true;
    res = "";
    list<QString> newStyles;
    list<QString>::const_iterator it;
    for (it = attrs.begin(); it != attrs.end(); ++it){
        QString name = *it;
        ++it;
        QString value = *it;
        if (name == "style"){
            list<QString> styles = parseStyle(value);
            for (list<QString>::iterator it = styles.begin(); it != styles.end(); ++it){
                QString name = *it;
                ++it;
                QString value = *it;
                for (const char **s = _styles; *s; s++){
                    if (name == *s){
                        newStyles.push_back(name);
                        newStyles.push_back(value);
                        break;
                    }
                }
            }
        }
    }
    for (it = newStyles.begin(); it != newStyles.end(); ++it){
        QString name = *it;
        ++it;
        if (name == "background-color")
            break;
    }
    if (it == newStyles.end()){
        char b[15];
        sprintf(b, "#%06X", m_bgColor & 0xFFFFFF);
        newStyles.push_back("background-color");
        newStyles.push_back(b);
    }
    res += "<span style=\"";
    res += makeStyle(newStyles);
    res += "\">";
}

void JabberImageParser::endBody()
{
    if (m_bBody){
        res += "</span>";
        m_bBody = false;
    }
}

void JabberImageParser::tag_start(const QString &tag, const list<QString> &attrs)
{
    if (tag == "html"){
        m_bBody = false;
        res = "";
        return;
    }
    if (tag == "body"){
        startBody(attrs);
        return;
    }
    if (!m_bBody)
        return;
    if (tag == "img"){
        QString src;
        QString alt;
        for (list<QString>::const_iterator it = attrs.begin(); it != attrs.end(); ++it){
            QString name = *it;
            ++it;
            QString value = *it;
            if (name == "src")
                src = value;
            if (name == "alt")
                alt = value;
        }
        if (!alt.isEmpty()){
            res += unquoteString(alt);
            return;
        }
        if (src.left(5) == "icon:"){
            QStringList smiles = getIcons()->getSmile(src.mid(5).ascii());
            if (!smiles.empty()){
                res += smiles.front();
                return;
            }
        }
        text(alt);
        return;
    }
    if (tag == "p"){
        if (m_bPara){
            res += "<br/>";
            m_bPara = false;
        }
        return;
    }
    if (tag == "br"){
        res += "<br/>";
        return;
    }
    for (const char **t = _tags; *t; t++){
        if (tag == *t){
            res += "<";
            res += tag;
            for (list<QString>::const_iterator it = attrs.begin(); it != attrs.end(); ++it){
                QString name = *it;
                ++it;
                QString value = *it;
                if (name == "style"){
                    list<QString> styles = parseStyle(value);
                    list<QString> newStyles;
                    for (list<QString>::iterator it = styles.begin(); it != styles.end(); ++it){
                        QString name = *it;
                        ++it;
                        QString value = *it;
                        for (const char **s = _styles; *s; s++){
                            if (name == *s){
                                newStyles.push_back(name);
                                newStyles.push_back(value);
                                break;
                            }
                        }
                    }
                    value = makeStyle(newStyles);
                }
                if ((name != "style") && (name != "href"))
                    continue;
                res += " ";
                res += name;
                if (!value.isEmpty()){
                    res += "=\'";
                    res += quoteString(value);
                    res += "\'";
                }
            }
            res += ">";
            return;
        }
    }
    if (tag == "b"){
        res += "<span style=\'font-weight:bold\'>";
        return;
    }
    if (tag == "i"){
        res += "<span style=\'font-style:italic\'>";
        return;
    }
    if (tag == "u"){
        res += "<span style=\'text-decoration:underline\'>";
        return;
    }
    if (tag == "font"){
        res += "<span";
        string style;
        for (list<QString>::const_iterator it = attrs.begin(); it != attrs.end(); ++it){
            QString name = *it;
            ++it;
            QString value = *it;
            if (name == "color"){
                if (!style.empty())
                    style += ";";
                style += "color: ";
                style += value.utf8();
                continue;
            }
        }
        if (!style.empty()){
            res += " style=\'";
            res += style.c_str();
            res += "\'";
        }
        res += ">";
        return;
    }
    return;
}

void JabberImageParser::tag_end(const QString &tag)
{
    if (tag == "body"){
        endBody();
        return;
    }
    if (!m_bBody)
        return;
    if (tag == "p"){
        m_bPara = true;
        return;
    }
    for (const char **t = _tags; *t; t++){
        if (tag == *t){
            res += "</";
            res += tag;
            res += ">";
            return;
        }
    }
    if ((tag == "b") || (tag == "i") || (tag == "u") || (tag == "font")){
        res += "</span>";
        return;
    }
}

static QString removeImages(const QString &text, unsigned bgColor)
{
    JabberImageParser p(bgColor);
    return p.parse(text);
}

bool JabberClient::send(Message *msg, void *_data)
{
    if ((getState() != Connected) || (_data == NULL))
        return false;
    JabberUserData *data = (JabberUserData*)_data;
    switch (msg->type()){
    case MessageAuthRefused:{
            QString grp;
            Group *group = NULL;
            Contact *contact = getContacts()->contact(msg->contact());
            if (contact && contact->getGroup())
                group = getContacts()->group(contact->getGroup());
            if (group)
                grp = group->getName();
            listRequest(data, data->Name.str(), grp, false);
            if (data->Subscribe.toULong() & SUBSCRIBE_FROM){
                m_socket->writeBuffer.packetStart();
                m_socket->writeBuffer
                << "<presence to=\'"
                << data->ID.str().utf8();
                m_socket->writeBuffer
                << "\' type=\'unsubscribed\'><status>"
                << (const char*)(quoteString(msg->getPlainText(), quoteNOBR).utf8())
                << "</status></presence>";
                sendPacket();
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
        }
    case MessageGeneric:{
            Contact *contact = getContacts()->contact(msg->contact());
            if ((contact == NULL) || (data == NULL))
                return false;
            QString text;
            text = msg->getPlainText();
            messageSend ms;
            ms.msg  = msg;
            ms.text = &text;
            Event eSend(EventSend, &ms);
            eSend.process();
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<message type=\'chat\' to=\'"
            << data->ID.str().utf8();
            if (!msg->getResource().isEmpty()){
                m_socket->writeBuffer
                << "/"
                << msg->getResource();
            }
            m_socket->writeBuffer
            << "\'><body>"
            << (const char*)(quote_nbsp(quoteString(text, quoteNOBR))).utf8()
            << "</body>";
            if (data->richText.toBool() && getRichText() && (msg->getFlags() & MESSAGE_RICHTEXT)){
                m_socket->writeBuffer
                << "<html xmlns='http://jabber.org/protocol/xhtml-im'><body>"
                << (const char*)quote_nbsp(removeImages(msg->getRichText(), msg->getBackground())).utf8()
                << "</body></html>";
            }
            m_socket->writeBuffer
            << "</message>";
            sendPacket();
            if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                if (data->richText.toBool()){
                    msg->setClient(dataName(data));
                    Event e(EventSent, msg);
                    e.process();
                }else{
                    Message m(MessageGeneric);
                    m.setContact(msg->contact());
                    m.setClient(dataName(data));
                    m.setText(msg->getPlainText());
                    Event e(EventSent, msg);
                    e.process();
                }
            }
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            return true;
        }
    case MessageUrl:{
            Contact *contact = getContacts()->contact(msg->contact());
            if ((contact == NULL) || (data == NULL))
                return false;
            UrlMessage *m = static_cast<UrlMessage*>(msg);
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<message type=\'chat\' to=\'"
            << data->ID.str().utf8();
            if (!msg->getResource().isEmpty()){
                m_socket->writeBuffer
                << "/"
                << msg->getResource();
            }
            m_socket->writeBuffer
            << "\'><body>"
            << (const char*)(quoteString(m->getUrl(), quoteNOBR).utf8());
            QString t = m->getPlainText();
            if (!t.isEmpty()){
                m_socket->writeBuffer
                << "\n"
                << (const char*)(quoteString(m->getPlainText(), quoteNOBR).utf8());
            }
            m_socket->writeBuffer
            << "</body>";
            if (data->richText.toBool() && getRichText()){
                m_socket->writeBuffer
                << "<html xmlns='http://jabber.org/protocol/xhtml-im'><body>"
                << "<a href=\'"
                << (const char*)(quoteString(m->getUrl(), quoteNOBR).utf8())
                << "\'>"
                << (const char*)(quoteString(m->getUrl(), quoteNOBR).utf8())
                << "</a>";
                if (!t.isEmpty()){
                    m_socket->writeBuffer
                    << "<br/>"
                    << removeImages(msg->getRichText(), msg->getBackground());
                }
                m_socket->writeBuffer
                << "</body></html>";
            }
            m_socket->writeBuffer
            << "</message>";
            sendPacket();
            if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                if (data->richText.toBool()){
                    msg->setClient(dataName(data));
                    Event e(EventSent, msg);
                    e.process();
                }else{
                    Message m(MessageGeneric);
                    m.setContact(msg->contact());
                    m.setClient(dataName(data));
                    m.setText(msg->getPlainText());
                    Event e(EventSent, msg);
                    e.process();
                }
            }
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            return true;
        }
    case MessageContacts:{
            Contact *contact = getContacts()->contact(msg->contact());
            if ((contact == NULL) || (data == NULL))
                return false;
            ContactsMessage *m = static_cast<ContactsMessage*>(msg);
            list<string> jids;
            list<string> names;
            QString contacts = m->getContacts();
            QString nc;
            while (!contacts.isEmpty()){
                QString item = getToken(contacts, ';');
                QString url = getToken(item, ',');
                QString proto = getToken(url, ':');
                if (proto == "sim"){
                    Contact *contact = getContacts()->contact(atol(url.latin1()));
                    if (contact){
                        clientData *data;
                        ClientDataIterator it(contact->clientData);
                        while ((data = ++it) != NULL){
                            Contact *c = contact;
                            if (!isMyData(data, c))
                                continue;
                            JabberUserData *d = (JabberUserData*)data;
                            string s = d->ID.str().utf8();
                            jids.push_back(s);
                            string n;
                            n = c->getName().utf8();
                            names.push_back(n);
                            if (!nc.isEmpty())
                                nc += ";";
                            nc += "jabber:";
                            nc += d->ID.str().utf8();
                            nc += ",";
                            if (c->getName() == d->ID.str()){
                                nc += d->ID.str();
                            }else{
                                nc += c->getName();
                                nc += " (";
                                nc += d->ID.str();
                                nc += ")";
                            }
                        }
                    }
                }
            }
            if (jids.empty()){
                msg->setError(I18N_NOOP("No contacts for send"));
                Event e(EventMessageSent, msg);
                e.process();
                delete msg;
                return true;
            }
            m->setContacts(nc);
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<message type=\'chat\' to=\'"
            << data->ID.str().utf8();
            if (!msg->getResource().isEmpty()){
                m_socket->writeBuffer
                << "/"
                << msg->getResource();
            }
            m_socket->writeBuffer
            << "\'><x xmlns='jabber:x:roster'>";
            list<string>::iterator iti = jids.begin();
            list<string>::iterator itn = names.begin();
            for (; iti != jids.end(); ++iti, ++itn){
                m_socket->writeBuffer
                << "<item name=\'"
                << (const char*)(quoteString(QString::fromUtf8((*itn).c_str()), quoteNOBR).utf8())
                << "\' jid=\'"
                << (const char*)(quoteString(QString::fromUtf8((*iti).c_str()), quoteNOBR).utf8())
                << "\'><group/></item>";
            }
            m_socket->writeBuffer
            << "</x><body>";
            iti = jids.begin();
            for (; iti != jids.end(); ++iti, ++itn){
                m_socket->writeBuffer
                << (const char*)(quoteString(QString::fromUtf8((*iti).c_str()), quoteNOBR).utf8())
                << "\n";
            }
            m_socket->writeBuffer
            << "</body>";
            if (data->richText.toBool() && getRichText()){
                m_socket->writeBuffer
                << "<html xmlns='http://jabber.org/protocol/xhtml-im'><body>";
                iti = jids.begin();
                for (; iti != jids.end(); ++iti, ++itn){
                    m_socket->writeBuffer
                    << (const char*)(quoteString(QString::fromUtf8((*iti).c_str()), quoteNOBR).utf8())
                    << "<br/>\n";
                }
                m_socket->writeBuffer
                << "</body></html>";
            }
            m_socket->writeBuffer
            << "</message>";
            sendPacket();
            if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                if (data->richText.toBool()){
                    msg->setClient(dataName(data));
                    Event e(EventSent, msg);
                    e.process();
                }else{
                    Message m(MessageGeneric);
                    m.setContact(msg->contact());
                    m.setClient(dataName(data));
                    m.setText(msg->getPlainText());
                    Event e(EventSent, msg);
                    e.process();
                }
            }
            Event e(EventMessageSent, msg);
            e.process();
            delete msg;
            return true;
        }
    case MessageFile:{
            m_waitMsg.push_back(msg);
            JabberFileTransfer *ft = static_cast<JabberFileTransfer*>(static_cast<FileMessage*>(msg)->m_transfer);
            if (ft == NULL)
                ft = new JabberFileTransfer(static_cast<FileMessage*>(msg), data, this);
            ft->listen();
            return true;
        }
    case MessageAuthRequest:{
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<presence to=\'"
            << data->ID.str().utf8();
            m_socket->writeBuffer
            << "\' type=\'subscribe\'><status>"
            << (const char*)(quoteString(msg->getPlainText(), quoteNOBR).utf8())
            << "</status></presence>";
            sendPacket();
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
    case MessageAuthGranted:{
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<presence to=\'"
            << data->ID.str().utf8();
            m_socket->writeBuffer
            << "\' type=\'subscribed\'></presence>";
            sendPacket();
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
    case MessageJabberOnline:
        if (isAgent(data->ID.str().utf8()) && (data->Status.toULong() == STATUS_OFFLINE)){
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<presence to=\'"
            << data->ID.str().utf8()
            << "\'></presence>";
            sendPacket();
            delete msg;
            return true;
        }
        break;
    case MessageJabberOffline:
        if (isAgent(data->ID.str().utf8()) && (data->Status.toULong() != STATUS_OFFLINE)){
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<presence to=\'"
            << data->ID.str().utf8()
            << "\' type=\'unavailable\'></presence>";
            sendPacket();
            delete msg;
            return true;
        }
        break;
    case MessageTypingStart:
        if (getTyping()){
            data->composeId.asULong() = ++m_msg_id;
            QString msg_id = "msg";
            msg_id += QString::number(data->composeId.toULong());
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<message to=\'"
            << data->ID.str().utf8()
            << "\'><x xmlns='jabber:x:event'><composing/><id>"
            << (const char*)msg_id.utf8()
            << "</id></x></message>";
            sendPacket();
            delete msg;
            return true;
        }
        break;
    case MessageTypingStop:
        if (getTyping()){
            if (data->composeId.toULong() == 0)
                return false;
            QString msg_id = "msg";
            msg_id += number(data->composeId.toULong());
            m_socket->writeBuffer.packetStart();
            m_socket->writeBuffer
            << "<message to=\'"
            << data->ID.str().utf8()
            << "\'><x xmlns='jabber:x:event'><id>"
            << (const char*)msg_id.utf8()
            << "</id></x></message>";
            sendPacket();
            delete msg;
            return true;
        }
        break;
    }
    return false;
}

QString JabberClient::dataName(void *_data)
{
    QString res = name();
    JabberUserData *data = (JabberUserData*)_data;
    res += "+";
    res += data->ID.str();
    res = res.replace(QRegExp("/"), "_");
    return res;
}

void JabberClient::listRequest(JabberUserData *data, const char *name, const char *grp, bool bDelete)
{
    QString jid = data->ID.str();
    list<JabberListRequest>::iterator it;
    for (it = m_listRequests.begin(); it != m_listRequests.end(); ++it){
        if (jid == (*it).jid){
            m_listRequests.erase(it);
            break;
        }
    }
    JabberListRequest lr;
    lr.jid = jid;
    if (name)
        lr.name = name;
    if (grp)
        lr.grp = grp;
    lr.bDelete = bDelete;
    m_listRequests.push_back(lr);
    processList();
}

JabberListRequest *JabberClient::findRequest(const char *jid, bool bRemove)
{
    list<JabberListRequest>::iterator it;
    for (it = m_listRequests.begin(); it != m_listRequests.end(); ++it){
        if ((*it).jid == QString::fromUtf8(jid)){
            if (bRemove){
                m_listRequests.erase(it);
                return NULL;
            }
            return &(*it);
        }
    }
    return NULL;
}


bool JabberClient::isAgent(const char *jid)
{
    const char *p = strrchr(jid, '/');
    if (p && !strcmp(p + 1, "registered"))
        return true;
    return false;
}


void JabberClient::auth_request(const char *jid, unsigned type, const char *text, bool bCreate)
{
    Contact *contact;
    string resource;
    JabberUserData *data = findContact(jid, NULL, false, contact, resource);
    if (isAgent(jid) || ((type == MessageAuthRequest) && getAutoAccept())){
        switch (type){
        case MessageAuthRequest:{
                if (data == NULL)
                    data = findContact(jid, NULL, true, contact, resource);
                m_socket->writeBuffer.packetStart();
                m_socket->writeBuffer
                << "<presence to=\'"
                << data->ID.str().utf8()
                << "\' type=\'subscribed\'></presence>";
                sendPacket();
                m_socket->writeBuffer.packetStart();
                m_socket->writeBuffer
                << "<presence to=\'"
                << data->ID.str().utf8()
                << "\' type=\'subscribe\'><status>"
                << "</status></presence>";
                sendPacket();
                Event e(EventContactChanged, contact);
                e.process();
                return;
            }
        case MessageAuthGranted:{
                if (data == NULL)
                    data = findContact(jid, NULL, true, contact, resource);
                Event e(EventContactChanged, contact);
                e.process();
                return;
            }

        }
    }
    if ((data == NULL) && bCreate){
        data = findContact(jid, NULL, true, contact, resource);
        contact->setFlags(CONTACT_TEMP);
    }
    if (data == NULL)
        return;
    if (((type == MessageAuthGranted) || (type ==MessageAuthRefused)) &&
            (contact->getFlags() & CONTACT_TEMP)){
        contact->setFlags(contact->getFlags() & ~CONTACT_TEMP);
        Event e(EventContactChanged, contact);
        e.process();
        return;
    }
    AuthMessage msg(type);
    msg.setContact(contact->id());
    msg.setClient(dataName(data));
    msg.setFlags(MESSAGE_RECEIVED);
    if (text)
        msg.setText(unquoteString(QString::fromUtf8(text)));
    Event e(EventMessageReceived, &msg);
    e.process();
}

void JabberClient::setInvisible(bool bState)
{
    if (getInvisible() == bState)
        return;
    TCPClient::setInvisible(bState);
    if (getStatus() == STATUS_OFFLINE)
        return;
    unsigned status = getStatus();
    m_status = STATUS_OFFLINE;
    if (getInvisible()){
        setStatus(status, NULL);
        return;
    }
    setStatus(status);
}

QString JabberClient::VHost()
{
    if (data.UseVHost.toBool() && !data.VHost.str().isEmpty())
        return data.VHost.str();
    return data.Server.str();
}

JabberFileTransfer::JabberFileTransfer(FileMessage *msg, JabberUserData *data, JabberClient *client)
        : FileTransfer(msg)
{
    m_data   = data;
    m_client = client;
    m_state  = None;
    m_socket = new ClientSocket(this);
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
}

JabberFileTransfer::~JabberFileTransfer()
{
    for (list<Message*>::iterator it = m_client->m_waitMsg.begin(); it != m_client->m_waitMsg.end(); ++it){
        if ((*it) == m_msg){
            m_client->m_waitMsg.erase(it);
            break;
        }
    }
    if (m_socket)
        delete m_socket;
}

void JabberFileTransfer::listen()
{
    if (m_file == NULL){
        for (;;){
            if (!openFile()){
                if (FileTransfer::m_state == FileTransfer::Done)
                    m_socket->error_state("");
                return;
            }
            if (!isDirectory())
                break;
        }
    }
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void JabberFileTransfer::startReceive(unsigned pos)
{
    m_startPos = pos;
    JabberFileMessage *msg = static_cast<JabberFileMessage*>(m_msg);
    m_socket->connect(msg->getHost(), msg->getPort(), m_client);
    m_state = Connect;
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
}

void JabberFileTransfer::bind_ready(unsigned short port)
{
    if (m_state == None){
        m_state = Listen;
    }else{
        m_state = ListenWait;
        FileTransfer::m_state = FileTransfer::Listen;
        if (m_notify)
            m_notify->process();
    }
    QString fname = m_file->name();
    fname = fname.replace(QRegExp("\\\\"), "/");
    int n = fname.findRev('/');
    if (n >= 0)
        fname = fname.mid(n + 1);
    m_url = fname.utf8();
    m_client->sendFileRequest(m_msg, port, m_data, m_url.c_str(), m_fileSize);
}

bool JabberFileTransfer::error(const char *err)
{
    error_state(err, 0);
    return true;
}

bool JabberFileTransfer::accept(Socket *s, unsigned long)
{
    if (m_state == Listen){
        Event e(EventMessageAcked, m_msg);
        e.process();
        m_state = ListenWait;
    }
    log(L_DEBUG, "Accept connection");
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
    m_socket->setSocket(s);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    m_answer = 400;
    return true;
}

bool JabberFileTransfer::error_state(const char *err, unsigned)
{
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

void JabberFileTransfer::packet_ready()
{
    if (m_socket->readBuffer.writePos() == 0)
        return;
    if (m_state != Receive){
        JabberPlugin *plugin = static_cast<JabberPlugin*>(m_client->protocol()->plugin());
        log_packet(m_socket->readBuffer, false, plugin->JabberPacket);
        for (;;){
            QCString s;
            if (!m_socket->readBuffer.scan("\n", s))
                break;
            if (!s.isEmpty() && (s[(int)s.length() - 1] == '\r'))
                s = s.left(s.length() - 1);
            if (!get_line(s))
                break;
        }
    }
    if (m_state == Receive){
        if (m_file == NULL){
            m_socket->error_state("", 0);
            return;
        }
        unsigned size = m_socket->readBuffer.size() - m_socket->readBuffer.readPos();
        if (size > m_endPos - m_startPos)
            size = m_endPos - m_startPos;
        if (size){
            m_file->writeBlock(m_socket->readBuffer.data(m_socket->readBuffer.readPos()), size);
            m_bytes += size;
            m_totalBytes += size;
            m_startPos += size;
            m_transferBytes += size;
            if (m_startPos == m_endPos){
                FileTransfer::m_state = FileTransfer::Done;
                if (m_notify){
                    m_notify->transfer(false);
                    m_notify->process();
                }
                m_socket->error_state("");
            }
            if (m_notify)
                m_notify->process();
        }
    }
    if (m_socket->readBuffer.readPos() == m_socket->readBuffer.writePos())
        m_socket->readBuffer.init(0);
}

void JabberFileTransfer::connect_ready()
{
    JabberFileMessage *msg = static_cast<JabberFileMessage*>(m_msg);
    QString line;
    line = "GET /";
    line += msg->getDescription().utf8();
    line += " HTTP/1.1\r\n"
            "Host :";
    line += msg->getHost();
    line += "\r\n";
    if (m_startPos){
        line += "Range: ";
		line += QString::number(m_startPos);
        line += "-\r\n";
    }
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
    send_line(line);
    FileTransfer::m_state = FileTransfer::Negotiation;
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
}

void JabberFileTransfer::write_ready()
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
    if (m_startPos >= m_endPos){
        if (m_notify)
            m_notify->transfer(false);
        m_bytes += m_file->size() - m_endPos;
        m_totalBytes += m_file->size() - m_endPos;
        for (;;){
            if (!openFile()){
                m_state = None;
                if (FileTransfer::m_state == FileTransfer::Done)
                    m_socket->error_state("");
                break;
            }
            if (isDirectory())
                continue;
            m_state = Wait;
            FileTransfer::m_state = FileTransfer::Wait;
            if (!((Client*)m_client)->send(m_msg, m_data))
                error_state(I18N_NOOP("File transfer failed"), 0);
            break;
        }
        if (m_notify)
            m_notify->process();
        m_socket->close();
        return;
    }
    time_t now;
    time(&now);
    if ((unsigned)now != m_sendTime){
        m_sendTime = now;
        m_sendSize = 0;
    }
    if (m_sendSize > (m_speed << 18)){
        m_socket->pause(1);
        return;
    }
    char buf[2048];
    unsigned tail = sizeof(buf);
    if (tail > m_endPos - m_startPos)
        tail = m_endPos - m_startPos;
    int readn = m_file->readBlock(buf, tail);
    if (readn <= 0){
        m_socket->error_state("Read file error");
        return;
    }
    m_startPos   += readn;
    m_transfer    = readn;
    m_bytes      += readn;
    m_totalBytes += readn;
    m_sendSize   += readn;
    m_socket->writeBuffer.pack(buf, readn);
    m_socket->write();
}

bool JabberFileTransfer::get_line(const char *str)
{
    string line = str;
    if (line.empty()){
        if (m_state == Connect){
            m_socket->error_state(I18N_NOOP("File transfer failed"));
            return true;
        }
        if (m_state == ReadHeader){
            if (m_endPos < m_startPos)
                m_endPos = m_startPos;
            if (m_file)
                m_file->at(m_startPos);
            m_state = Receive;
            FileTransfer::m_state = FileTransfer::Read;
            m_bytes += m_startPos;
            m_totalBytes += m_startPos;
            m_fileSize = m_endPos;
            m_totalSize = m_endPos;
            if (m_notify){
                m_notify->process();
                m_notify->transfer(true);
            }
            return true;
        }
        if (m_file->size() < m_endPos)
            m_endPos = m_file->size();
        if (m_startPos > m_endPos)
            m_startPos = m_endPos;
        if ((m_answer == 200) && (m_startPos == m_endPos))
            m_answer = 204;
        if ((m_answer == 200) && ((m_startPos != 0) || (m_endPos < m_file->size())))
            m_answer = 206;
        QString s;
        s = "HTTP/1.0 ";
        s += QString::number(m_answer);
        switch (m_answer){
        case 200:
            s += " OK";
            break;
        case 204:
            s += " No content";
            break;
        case 206:
            s += " Partial content";
            break;
        case 400:
            s += " Bad request";
            break;
        case 404:
            s += " Not found";
            break;
        default:
            s += " Error";
        }
        send_line(s);
        if ((m_answer == 200) || (m_answer == 206)){
            send_line("Content-Type: application/data");
            s = "Content-Length: ";
            s += QString::number(m_endPos - m_startPos);
            send_line(s);
        }
        if (m_answer == 206){
            s = "Range: ";
            s += QString::number(m_startPos);
            s += "-";
            s += QString::number(m_endPos);
            send_line(s);
        }
        send_line("");
        if (m_answer < 300){
            m_file->at(m_startPos);
            FileTransfer::m_state = FileTransfer::Write;
            m_state = Send;
            m_bytes = m_startPos;
            m_totalBytes += m_startPos;
            if (m_notify){
                m_notify->process();
                m_notify->transfer(true);
            }
            write_ready();
        }else{
            m_socket->error_state("Bad request");
        }
        return false;
    }
    if (m_state == ListenWait){
        string t = getToken(line, ' ');
        if (t == "GET"){
            m_answer = 404;
            t = getToken(line, ' ');
            if (t[0] == '/'){
                if (m_url == (t.c_str() + 1))
                    m_answer = 200;
            }
        }
        m_state = Header;
        return true;
    }
    if (m_state == Connect){
        string t = getToken(line, ' ');
        t = getToken(t, '/');
        if (t != "HTTP"){
            m_socket->error_state(I18N_NOOP("File transfer fail"));
            return true;
        }
        unsigned code = atol(getToken(line, ' ').c_str());
        switch (code){
        case 200:
        case 206:
            m_startPos = 0;
            m_endPos   = 0xFFFFFFFF;
            break;
        case 204:
            m_startPos = 0;
            m_endPos   = 0;
            break;
        }
        m_state = ReadHeader;
        return true;
    }
    if (m_state == ReadHeader){
        string t = getToken(line, ':');
        if (t == "Content-Length"){
            const char *p;
            for (p = line.c_str(); *p; p++)
                if ((*p > '0') && (*p < '9'))
                    break;
            m_endPos = m_startPos + strtoul(p, NULL, 10);
        }
        if (t == "Range"){
            const char *p;
            for (p = line.c_str(); *p; p++)
                if ((*p > '0') && (*p < '9'))
                    break;
            m_startPos = strtoul(p, NULL, 10);
            for (; *p; p++)
                if (*p == '-'){
                    ++p;
                    break;
                }
            if ((*p > '0') && (*p < '9'))
                m_endPos = m_startPos + strtoul(p, NULL, 10);
        }
        return true;
    }
    string t = getToken(line, ':');
    if (t == "Range"){
        const char *p = line.c_str();
        for (; *p; p++)
            if (*p != ' ')
                break;
        m_startPos = strtoul(p, NULL, 10);
        for (; *p; p++)
            if (*p == '-'){
                p++;
                break;
            }
        if ((*p >= '0') && (*p <= '9'))
            m_endPos = strtoul(p, NULL, 10);
    }
    return true;
}

void JabberFileTransfer::send_line(const char *line)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << line;
    m_socket->writeBuffer << "\r\n";
    JabberPlugin *plugin = static_cast<JabberPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->JabberPacket);
    m_socket->write();
}

void JabberFileTransfer::connect()
{
    m_nFiles = 1;
    if (static_cast<JabberFileMessage*>(m_msg)->getPort() == 0)
        m_client->sendFileAccept(m_msg, m_data);
    if (m_notify)
        m_notify->createFile(m_msg->getDescription(), 0xFFFFFFFF, false);
}

#ifdef WIN32
static char PICT_PATH[] = "pictures\\";
#else
static char PICT_PATH[] = "pictures/";
#endif

QString JabberClient::photoFile(JabberUserData *data)
{
    QString f = PICT_PATH;
    f += "photo.";
    f += data->ID.str();
    f = user_file(f);
    return f;
}

QString JabberClient::logoFile(JabberUserData *data)
{
    QString f = PICT_PATH;
    f += "logo.";
    f += data->ID.str();
    f = user_file(f);
    return f;
}

#ifndef _MSC_VER
#include "jabberclient.moc"
#endif

