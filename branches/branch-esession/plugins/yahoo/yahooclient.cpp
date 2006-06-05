/***************************************************************************
                          yahooclient.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 * Based on libyahoo2
 *
 * Some code copyright (C) 2002-2004, Philip S Tellis <philip.tellis AT gmx.net>
 *
 * Yahoo Search copyright (C) 2003, Konstantin Klyagin <konst AT konst.org.ua>
 *
 * Much of this code was taken and adapted from the yahoo module for
 * gaim released under the GNU GPL.  This code is also released under the 
 * GNU GPL.
 *
 * This code is derivitive of Gaim <http://gaim.sourceforge.net>
 * copyright (C) 1998-1999, Mark Spencer <markster@marko.net>
 *             1998-1999, Adam Fritzler <afritz@marko.net>
 *             1998-2002, Rob Flynn <rob@marko.net>
 *             2000-2002, Eric Warmenhoven <eric@warmenhoven.org>
 *             2001-2002, Brian Macke <macke@strangelove.net>
 *                  2001, Anand Biligiri S <abiligiri@users.sf.net>
 *                  2001, Valdis Kletnieks
 *                  2002, Sean Egan <bj91704@binghamton.edu>
 *                  2002, Toby Gray <toby.gray@ntlworld.com>
 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "yahoo.h"
#include "yahooclient.h"
#include "yahoocfg.h"
#include "yahooinfo.h"
#include "yahoosearch.h"

#include "html.h"
#include "core.h"
#include "icons.h"

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <time.h>

#include <qtimer.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qfile.h>

using namespace std;
using namespace SIM;

const unsigned long MessageYahooFile	= 0x700;

static char YAHOO_PACKET_SIGN[] = "YMSG";

const unsigned PING_TIMEOUT = 60;

const unsigned	YAHOO_LOGIN_OK		= 0;
const unsigned	YAHOO_LOGIN_PASSWD	= 13;
const unsigned	YAHOO_LOGIN_LOCK	= 14;
const unsigned	YAHOO_LOGIN_DUPL	= 99;

static DataDef yahooUserData[] =
    {
        { "", DATA_ULONG, 1, DATA(9) },		// Sign
        { "LastSend", DATA_ULONG, 1, 0 },
        { "Login", DATA_UTF, 1, 0 },
        { "Nick", DATA_UTF, 1, 0 },
        { "First", DATA_UTF, 1, 0 },
        { "Last", DATA_UTF, 1, 0 },
        { "EMail", DATA_UTF, 1, 0 },
        { "", DATA_ULONG, 1, DATA(-1) },	// Status
        { "", DATA_BOOL, 1, 0 },			// bAway
        { "", DATA_UTF, 1, 0 },				// AwayMessage
        { "StatusTime", DATA_ULONG, 1, 0 },
        { "OnlineTime", DATA_ULONG, 1, 0 },
        { "Group", DATA_STRING, 1, 0 },
        { "", DATA_BOOL, 1, 0 },			// bChecked
        { "", DATA_BOOL, 1, 0 },			// bTyping
        { NULL, 0, 0, 0 }
    };

static DataDef yahooClientData[] =
    {
        { "Server", DATA_STRING, 1, "scs.msg.yahoo.com" },
        { "Port", DATA_ULONG, 1, DATA(5050) },
        { "MinPort", DATA_ULONG, 1, DATA(1024) },
        { "MaxPort", DATA_ULONG, 1, DATA(0xFFFE) },
        { "UseHTTP", DATA_BOOL, 1, 0 },
        { "AutoHTTP", DATA_BOOL, 1, DATA(1) },
        { "ListRequests", DATA_STRING, 1, 0 },
        { "", DATA_STRUCT, sizeof(YahooUserData) / sizeof(Data), DATA(yahooUserData) },
        { NULL, 0, 0, 0 }
    };

const DataDef *YahooProtocol::userDataDef()
{
    return yahooUserData;
}

YahooClient::YahooClient(Protocol *protocol, Buffer *cfg)
        : TCPClient(protocol, cfg)
{
    load_data(yahooClientData, &data, cfg);
    m_status = STATUS_OFFLINE;
    m_bFirstTry = false;
    m_ft_id = 0;
    string requests = getListRequests();
    while (!requests.empty()){
        string request = getToken(requests, ';');
        ListRequest lr;
        lr.type = atol(getToken(request, ',').c_str());
        lr.name = request;
        m_requests.push_back(lr);
    }
    setListRequests(NULL);
}

YahooClient::~YahooClient()
{
    TCPClient::setStatus(STATUS_OFFLINE, false);
    free_data(yahooClientData, &data);
}

string YahooClient::getConfig()
{
    string res = TCPClient::getConfig();
    if (res.length())
        res += "\n";
    string requests;
    for (list<ListRequest>::iterator it = m_requests.begin(); it != m_requests.end(); ++it){
        if (!requests.empty())
            requests += ";";
        requests += number((*it).type);
        requests += (*it).name;
    }
    setListRequests(requests.c_str());
    res += save_data(yahooClientData, &data);
    return res;
}

bool YahooClient::send(Message *msg, void *_data)
{
    if ((getState() != Connected) || (_data == NULL))
        return false;
    YahooUserData *data = (YahooUserData*)_data;
    Message_ID msg_id;
    switch (msg->type()){
    case MessageTypingStart:
        sendTyping(data, true);
        return true;
    case MessageTypingStop:
        sendTyping(data, false);
        return true;
    case MessageGeneric:
        sendMessage(msg->getRichText(), msg, data);
        return true;
    case MessageUrl:{
            QString msgText = static_cast<UrlMessage*>(msg)->getUrl();
            if (!msg->getPlainText().isEmpty()){
                msgText += "<br>";
                msgText += msg->getRichText();
            }
            sendMessage(msgText, msg, data);
            return true;
        }
    case MessageFile:{
            msg_id.id  = 0;
            msg_id.msg = msg;
            m_waitMsg.push_back(msg_id);
            YahooFileTransfer *ft = static_cast<YahooFileTransfer*>(static_cast<FileMessage*>(msg)->m_transfer);
            if (ft == NULL)
                ft = new YahooFileTransfer(static_cast<FileMessage*>(msg), data, this);
            ft->listen();
            return true;
        }
    }
    return false;
}

bool YahooClient::canSend(unsigned type, void *_data)
{
    if ((_data == NULL) || (((clientData*)_data)->Sign.value != YAHOO_SIGN))
        return false;
    if (getState() != Connected)
        return false;
    switch (type){
    case MessageGeneric:
    case MessageUrl:
    case MessageFile:
        return true;
    }
    return false;
}

void YahooClient::packet_ready()
{
    log_packet(m_socket->readBuffer, false, YahooPlugin::YahooPacket);
    if (m_bHeader){
        char header[4];
        m_socket->readBuffer.unpack(header, 4);
        if (memcmp(header, YAHOO_PACKET_SIGN, 4)){
            m_socket->error_state("Bad packet sign");
            return;
        }
        m_socket->readBuffer.incReadPos(4);
        m_socket->readBuffer >> m_data_size >> m_service;
        unsigned long session_id;
        m_socket->readBuffer >> m_pkt_status >> session_id;
        if (m_data_size){
            m_socket->readBuffer.add(m_data_size);
            m_bHeader = false;
            return;
        }
    }
    log_packet(m_socket->readBuffer, false, YahooPlugin::YahooPacket);
    scan_packet();
    m_socket->readBuffer.init(20);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
}

void YahooClient::sendPacket(unsigned short service, unsigned long status)
{
    if (m_bHTTP && !m_session_id.empty()){
        addParam(0, getLogin().utf8());
        addParam(24, m_session_id.c_str());
    }
    unsigned short size = 0;
    if (!m_values.empty()){
        for (list<PARAM>::iterator it = m_values.begin(); it != m_values.end(); ++it){
            size += 4;
            size += (*it).second.size();
            size += number((*it).first).length();
        }
    }
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack(YAHOO_PACKET_SIGN, 4);
    m_socket->writeBuffer << 0x000B0000L << size << service << status << m_session;
    if (size){
        for (list<PARAM>::iterator it = m_values.begin(); it != m_values.end(); ++it){
            m_socket->writeBuffer
            << number((*it).first).c_str()
            << (unsigned short)0xC080
            << (*it).second.c_str()
            << (unsigned short)0xC080;
        }
    }
    m_values.clear();
    log_packet(m_socket->writeBuffer, true, YahooPlugin::YahooPacket);
    m_socket->write();
}

void YahooClient::addParam(unsigned key, const char *value)
{
    if (value == NULL)
        value = "";
    m_values.push_back(PARAM(key, string(value)));
}

void YahooClient::connect_ready()
{
    m_bFirstTry = false;
    m_socket->readBuffer.init(20);
    m_socket->readBuffer.packetStart();
    m_session = rand();
    m_bHeader = true;
    log(L_DEBUG, "Connect ready");
    TCPClient::connect_ready();
    if (m_bHTTP){
        addParam(1, getLogin().utf8());
        sendPacket(YAHOO_SERVICE_AUTH);
    }else{
        sendPacket(YAHOO_SERVICE_VERIFY);
    }
}

const char *Params::operator [](unsigned id)
{
    for (iterator it = begin(); it != end(); ++it){
        if ((*it).first == id)
            return (*it).second.c_str();
    }
    return NULL;
}

void YahooClient::scan_packet()
{
    Params params;
    int param7_cnt = 0;

    for (;;){
        string key;
        string value;
        if (!(m_socket->readBuffer.scan("\xC0\x80", key) &&
                m_socket->readBuffer.scan("\xC0\x80", value)))
            break;
        unsigned key_id = atol(key.c_str());
        log(L_DEBUG, "Param: %u %s", key_id, value.c_str());
        /* There can be multiple buddies in an YAHOO_SERVICE_IDDEACT and
           YAHOO_SERVICE_LOGON paket ... */
        if ((key_id == 7) && ((m_service == YAHOO_SERVICE_IDDEACT) ||
                              (m_service == YAHOO_SERVICE_LOGON))) {
            if (param7_cnt) {
                /* process the last buddie */
                process_packet(params);
                params.clear();
                param7_cnt = 0;
            } else {
                param7_cnt = 1;
            }
        }
        params.push_back(PARAM(key_id, value));
    }
    process_packet(params);
}

void YahooClient::process_packet(Params &params)
{
    log(L_DEBUG,"Service type: %02X",m_service);
    switch (m_service){
    case YAHOO_SERVICE_VERIFY:
        if (m_pkt_status != 1){
            m_reconnect = NO_RECONNECT;
            m_socket->error_state(I18N_NOOP("Yahoo! login lock"));
            return;
        }
        addParam(1, getLogin().utf8());
        sendPacket(YAHOO_SERVICE_AUTH);
        break;
    case YAHOO_SERVICE_AUTH:
        process_auth(params[13], params[94], params[1]);
        break;
    case YAHOO_SERVICE_AUTHRESP:
        m_pkt_status = 0;
        if (params[66])
            m_pkt_status = atol(params[66]);
        switch (m_pkt_status){
        case YAHOO_LOGIN_OK:
            authOk();
            return;
        case YAHOO_LOGIN_PASSWD:
            m_reconnect = NO_RECONNECT;
            m_socket->error_state(I18N_NOOP("Login failed"), AuthError);
            return;
        case YAHOO_LOGIN_LOCK:
            m_reconnect = NO_RECONNECT;
            m_socket->error_state(I18N_NOOP("Your account has been locked"), AuthError);
            return;
        case YAHOO_LOGIN_DUPL:
            m_reconnect = NO_RECONNECT;
            m_socket->error_state(I18N_NOOP("Your account is being used from another location"));
            return;
        default:
            m_socket->error_state(I18N_NOOP("Login failed"));
        }
        break;
    case YAHOO_SERVICE_LIST:
        authOk();
        loadList(params[87]);
        break;
    case YAHOO_SERVICE_LOGOFF:
        if (m_pkt_status == (unsigned long)(-1)){
            m_reconnect = NO_RECONNECT;
            m_socket->error_state(I18N_NOOP("Your account is being used from another location"));
            return;
        }
    case YAHOO_SERVICE_LOGON:
        if (params[1]){
            if (params[24])
                m_session_id = params[24];
            authOk();
        }
    case YAHOO_SERVICE_USERSTAT:
    case YAHOO_SERVICE_ISAWAY:
    case YAHOO_SERVICE_ISBACK:
    case YAHOO_SERVICE_GAMELOGON:
    case YAHOO_SERVICE_GAMELOGOFF:
    case YAHOO_SERVICE_IDACT:
    case YAHOO_SERVICE_IDDEACT:
        if (params[7] && params[13])
            processStatus(m_service, params[7], params[10], params[19], params[47], params[137]);
        break;
    case YAHOO_SERVICE_IDLE:
    case YAHOO_SERVICE_MAILSTAT:
    case YAHOO_SERVICE_CHATINVITE:
    case YAHOO_SERVICE_CALENDAR:
    case YAHOO_SERVICE_NEWPERSONALMAIL:
    case YAHOO_SERVICE_ADDIDENT:
    case YAHOO_SERVICE_ADDIGNORE:
    case YAHOO_SERVICE_PING:
    case YAHOO_SERVICE_GOTGROUPRENAME:
    case YAHOO_SERVICE_GROUPRENAME:
    case YAHOO_SERVICE_PASSTHROUGH2:
    case YAHOO_SERVICE_CHATLOGON:
    case YAHOO_SERVICE_CHATLOGOFF:
    case YAHOO_SERVICE_CHATMSG:
    case YAHOO_SERVICE_REJECTCONTACT:
    case YAHOO_SERVICE_PEERTOPEER:
        break;
    case YAHOO_SERVICE_MESSAGE:
        if (params[4] && params[14])
            process_message(params[4], params[14], params[97]);
        break;
    case YAHOO_SERVICE_NOTIFY:
        if (params[4] && params[49])
            notify(params[4], params[49], params[13]);
        break;
    case YAHOO_SERVICE_NEWCONTACT:
        if (params[1]){
            contact_added(params[3], params[14]);
            return;
        }
        if (params[7]){
            processStatus(m_service, params[7], params[10], params[14], params[47], params[137]);
            return;
        }
        if (m_pkt_status == 7)
            contact_rejected(params[3], params[14]);
        break;
    case YAHOO_SERVICE_P2PFILEXFER:
        if ((params[49] == NULL) || strcmp(params[49], "FILEXFER")){
            log(L_WARN, "Unhandled p2p type %s", params[49]);
            break;
        }
        if ((params[28] == NULL) && params[11]){
            unsigned id =atol(params[11]);
            for (list<Message_ID>::iterator it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
                if ((*it).id == id){
                    FileMessage *msg = static_cast<FileMessage*>((*it).msg);
                    m_waitMsg.erase(it);
                    if (msg->m_transfer){
                        static_cast<YahooFileTransfer*>(msg->m_transfer)->error_state(I18N_NOOP("Message declined"), 0);
                        break;
                    }
                    msg->setError(I18N_NOOP("Message declined"));
                    Event e(EventMessageSent, msg);
                    e.process();
                    delete msg;
                    break;
                }
            }
            break;
        }
    case YAHOO_SERVICE_FILETRANSFER:
        /*

        	params[14] - can be empty when no message is send with the file...
            params[20] - url - just for filetransfer through website
        */
        if (params[4] && params[27] && params[28])
            process_file(params[4], params[27], params[28], params[14], params[20], params[11]);
        else
            /* file-url-message */
            process_fileurl(params[4],params[14],params[20]);
        break;
    case YAHOO_SERVICE_ADDBUDDY:
        if (params[1] && params[7] && params[65])
            log(L_DEBUG,"%s added %s to group %s",params[1],params[7],params[65]);
        else
            log(L_DEBUG,"Please send paket to developer!");
        break;
    case YAHOO_SERVICE_REMBUDDY:
        if (params[1] && params[7] && params[65])
            log(L_DEBUG,"%s removed %s from group %s",params[1],params[7],params[65]);
        else
            log(L_DEBUG,"Please send paket to developer!");
        break;
    case YAHOO_SERVICE_CONFINVITE:
        log(L_WARN, "Conferencing currently not implemented!");
    default:
        log(L_WARN, "Unknown service %02X", m_service);
    }
}

class TextParser
{
public:
    TextParser(YahooClient *client, Contact *contact);
    QString parse(const char *msg);

    class Tag
    {
    public:
        Tag(const QString &str);
        bool operator == (const Tag &t) const;
        QString open_tag() const;
        QString close_tag() const;
    protected:
        QString	m_tag;
    };

class FaceSizeParser : public HTMLParser
    {
    public:
        FaceSizeParser(const QString&);
        QString face;
        QString size;
    protected:
        virtual void text(const QString &text);
        virtual void tag_start(const QString &tag, const list<QString> &options);
        virtual void tag_end(const QString &tag);
    };

protected:
    void addText(const char *str, unsigned size);
    unsigned m_state;
    Contact *m_contact;
    QString color;
    QString face;
    QString size;
    bool m_bChanged;
    stack<Tag> m_tags;
    void setState(unsigned code, bool bSet);
    void clearState(unsigned code);
    void put_color(unsigned color);
    void put_style();
    void push_tag(const QString &tag);
    void pop_tag(const QString &tag);
    YahooClient   *m_client;
    QString m_text;
};

TextParser::FaceSizeParser::FaceSizeParser(const QString &str)
{
    parse(str);
}

void TextParser::FaceSizeParser::text(const QString&)
{
}

void TextParser::FaceSizeParser::tag_start(const QString &tag, const list<QString> &options)
{
    if (tag != "font")
        return;
    for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
        QString key = *it;
        ++it;
        if (key == "face")
            face = QString("font-family:") + *it;
        if (key == "size")
            size = QString("font-size:") + *it + "pt";
    }
}

void TextParser::FaceSizeParser::tag_end(const QString&)
{
}

TextParser::Tag::Tag(const QString &tag)
{
    m_tag	= tag;
}

bool TextParser::Tag::operator == (const Tag &t) const
{
    return close_tag() == t.close_tag();
}

QString TextParser::Tag::open_tag() const
{
    QString res;
    res += "<";
    res += m_tag;
    res += ">";
    return res;
}

QString TextParser::Tag::close_tag() const
{
    int n = m_tag.find(" ");
    QString res;
    res += "</";
    if (n >= 0){
        res += m_tag.left(n);
    }else{
        res += m_tag;
    }
    res += ">";
    return res;
}

TextParser::TextParser(YahooClient *client, Contact *contact)
{
    m_contact  = contact;
    m_client   = client;
    m_bChanged = false;
    m_state    = 0;
}

static unsigned esc_colors[] =
    {
        0x000000,
        0x0000FF,
        0x008080,
        0x808080,
        0x008000,
        0xFF0080,
        0x800080,
        0xFF8000,
        0xFF0000,
        0x808000
    };

QString TextParser::parse(const char *msg)
{
    Buffer b;
    b.pack(msg, strlen(msg));
    for (;;){
        string part;
        if (!b.scan("\x1B\x5B", part))
            break;
        addText(part.c_str(), part.length());

        if (!b.scan("m", part))
            break;
        if (part.empty())
            continue;
        if (part[0] == 'x'){
            unsigned code = atol(part.c_str() + 1);
            switch (code){
            case 1:
            case 2:
            case 4:
                setState(code, false);
                break;
            }
            continue;
        }
        if (part[0] == '#'){
            put_color(strtoul(part.c_str() + 1, NULL, 16));
            continue;
        }
        unsigned code = atol(part.c_str());
        switch (code){
        case 1:
        case 2:
        case 4:
            setState(code, true);
            break;
        default:
            if ((code >= 30) && (code < 40))
                put_color(esc_colors[code - 30]);
        }
    }
    addText(b.data(b.readPos()), b.writePos() - b.readPos());
    while (!m_tags.empty()){
        m_text += m_tags.top().close_tag();
        m_tags.pop();
    }
    return m_text;
}

void TextParser::setState(unsigned code, bool bSet)
{
    if (bSet){
        if ((m_state & code) == code)
            return;
        m_state |= code;
    }else{
        if ((m_state & code) == 0)
            return;
        m_state &= ~code;
    }
    QString tag;
    switch (code){
    case 1:
        tag = "b";
        break;
    case 2:
        tag = "i";
        break;
    case 4:
        tag = "u";
        break;
    default:
        return;
    }
    if (bSet){
        push_tag(tag);
    }else{
        pop_tag(tag);
    }
}

void TextParser::put_color(unsigned _color)
{
    color.sprintf("color:#%06X", _color & 0xFFFFFF);
    m_bChanged = true;
}

void TextParser::put_style()
{
    if (!m_bChanged)
        return;
    m_bChanged = false;
    QString style;
    if (!color.isEmpty())
        style = color;
    if (!face.isEmpty()){
        if (!style.isEmpty())
            style += ";";
        style += face;
    }
    if (!size.isEmpty()){
        if (!style.isEmpty())
            style += ";";
        style += size;
    }
    QString tag("span style=\"");
    tag += style;
    tag += "\"";
    pop_tag(tag);
    push_tag(tag);
}

void TextParser::push_tag(const QString &tag)
{
    Tag t(tag);
    m_tags.push(t);
    m_text += t.open_tag();
}

void TextParser::pop_tag(const QString &tag)
{
    Tag t(tag);
    stack<Tag> tags;
    bool bFound = false;
    QString text;
    while (!m_tags.empty()){
        Tag top = m_tags.top();
        m_tags.pop();
        text += top.close_tag();
        if (top == t){
            bFound = true;
            break;
        }
        tags.push(top);
    }
    if (bFound)
        m_text += text;
    while (!tags.empty()){
        Tag top = tags.top();
        tags.pop();
        if (bFound)
            m_text += top.open_tag();
        m_tags.push(top);
    }
}

void TextParser::addText(const char *str, unsigned s)
{
    if (s == 0)
        return;
    QString text;
    if (m_contact){
        text = getContacts()->toUnicode(m_contact, str, s);
    }else{
        text = QString::fromUtf8(str, s);
    }
    while (!text.isEmpty()){
        bool bFace = false;
        int n1 = text.find("<font size=\"");
        int n2 = text.find("<font face=\"");
        int n = -1;
        if (n1 >= 0)
            n = n1;
        if ((n2 >= 0) && ((n == -1) || (n2 < n1))){
            n = n2;
            bFace = true;
        }
        if (n < 0){
            if (!text.isEmpty())
                put_style();
            m_text += quoteString(text);
            break;
        }
        if (n)
            put_style();
        m_text += quoteString(text.left(n));
        text = text.mid(n);
        n = text.find(">");
        if (n < 0)
            break;
        FaceSizeParser p(text.left(n + 1));
        text = text.mid(n + 1);
        if (!p.face.isEmpty()){
            face = p.face;
            m_bChanged = true;
        }
        if (!p.size.isEmpty()){
            size = p.size;
            m_bChanged = true;
        }
    }
}

void YahooClient::process_message(const char *id, const char *msg, const char *utf)
{
    bool bUtf = false;
    if (utf && atol(utf))
        bUtf = true;
    Contact *contact = NULL;
    if (utf == NULL){
        if (findContact(id, NULL, contact) == NULL)
            contact = getContacts()->owner();
    }
    Message *m = new Message(MessageGeneric);
    m->setFlags(MESSAGE_RICHTEXT);
    TextParser parser(this, contact);
    m->setText(parser.parse(msg));
    messageReceived(m, id);
}

void YahooClient::notify(const char *id, const char *msg, const char *state)
{
    Contact *contact;
    YahooUserData *data = findContact(id, NULL, contact);
    if (data == NULL)
        return;
    bool bState = false;
    if (state && atol(state))
        bState = true;
    if (!strcasecmp(msg, "TYPING")){
        if (data->bTyping.bValue != bState){
            data->bTyping.bValue = bState;
            Event e(EventContactStatus, contact);
            e.process();
        }
    }
}

void YahooClient::contact_added(const char *id, const char *message)
{
    Message *msg = new AuthMessage(MessageAdded);
    if (message)
        msg->setText(QString::fromUtf8(message));
    messageReceived(msg, id);
}

void YahooClient::contact_rejected(const char *id, const char *message)
{
    Message *msg = new AuthMessage(MessageRemoved);
    if (message)
        msg->setText(QString::fromUtf8(message));
    messageReceived(msg, id);
}

static bool equal(const char *s1, const char *s2)
{
    if (s1 == NULL)
        s1 = "";
    if (s2 == NULL)
        s2 = "";
    return strcmp(s1, s2) != 0;
}

void YahooClient::processStatus(unsigned short service, const char *id,
                                const char *_state, const char *_msg,
                                const char *_away, const char *_idle)
{
    Contact *contact;
    YahooUserData *data = findContact(id, NULL, contact);
    if (data == NULL)
        return;
    unsigned state = 0;
    unsigned away  = 0;
    unsigned idle  = 0;
    if (_state)
        state = atol(_state);
    if (_away)
        away  = atol(_away);
    if (_idle)
        idle  = atol(_idle);
    if (service == YAHOO_SERVICE_LOGOFF)
        state = YAHOO_STATUS_OFFLINE;
    if ((state != data->Status.value) ||
            ((state == YAHOO_STATUS_CUSTOM) &&
             (((away != 0) != data->bAway.bValue) || equal(_msg, data->AwayMessage.ptr)))){

        unsigned long old_status = STATUS_UNKNOWN;
        unsigned style  = 0;
        const char *statusIcon = NULL;
        contactInfo(data, old_status, style, statusIcon);

        time_t now;
        time(&now);
        now -= idle;
        if (data->Status.value == YAHOO_STATUS_OFFLINE)
            data->OnlineTime.value = now;
        data->Status.value = state;
        data->bAway.bValue = (away != 0);
        data->StatusTime.value = now;

        unsigned long new_status = STATUS_UNKNOWN;
        contactInfo(data, new_status, style, statusIcon);

        if (old_status != new_status){
            StatusMessage m;
            m.setContact(contact->id());
            m.setClient(dataName(data).c_str());
            m.setFlags(MESSAGE_RECEIVED);
            m.setStatus(STATUS_OFFLINE);
            Event e(EventMessageReceived, &m);
            e.process();
            if ((new_status == STATUS_ONLINE) && !contact->getIgnore() && (getState() == Connected) &&
                    (data->StatusTime.value > this->data.owner.OnlineTime.value + 30)){
                Event e(EventContactOnline, contact);
                e.process();
            }
        }else{
            Event e(EventContactStatus, contact);
            e.process();
        }
    }
}

string YahooClient::name()
{
    string res = "Yahoo.";
    if (data.owner.Login.ptr)
        res += data.owner.Login.ptr;
    return res;
}

string YahooClient::dataName(void *_data)
{
    string res = name();
    YahooUserData *data = (YahooUserData*)_data;
    res += "+";
    res += data->Login.ptr;
    return res;
}

void YahooClient::setStatus(unsigned status)
{
    if (status  == m_status)
        return;
    time_t now;
    time(&now);
    if (m_status == STATUS_OFFLINE)
        data.owner.OnlineTime.value = now;
    data.owner.StatusTime.value = now;
    m_status = status;
    data.owner.Status.value = m_status;
    Event e(EventClientChanged, static_cast<Client*>(this));
    e.process();
    if (status == STATUS_OFFLINE){
        if (m_status != STATUS_OFFLINE){
            m_status = status;
            data.owner.Status.value = status;
            time_t now;
            time(&now);
            data.owner.StatusTime.value = now;
        }
        return;
    }
    unsigned long yahoo_status = YAHOO_STATUS_OFFLINE;
    switch (status){
    case STATUS_ONLINE:
        yahoo_status = YAHOO_STATUS_AVAILABLE;
        break;
    case STATUS_DND:
        yahoo_status = YAHOO_STATUS_BUSY;
        break;
    }
    if (yahoo_status != YAHOO_STATUS_OFFLINE){
        m_status = status;
        sendStatus(yahoo_status);
        return;
    }
    ARRequest ar;
    ar.contact  = NULL;
    ar.status   = status;
    ar.receiver = this;
    ar.param	= (void*)(unsigned long)status;
    Event eAR(EventARRequest, &ar);
    eAR.process();
}

void YahooClient::process_file(const char *id, const char *fileName, const char *fileSize, const char *msg, const char *url, const char *msgid)
{
    YahooFileMessage *m = new YahooFileMessage;
    m->setDescription(getContacts()->toUnicode(NULL, fileName));
    m->setSize(atol(fileSize));
    if (url)
        m->setUrl(url);
    if (msg)
        m->setServerText(msg);
    if (msgid)
        m->setMsgID(atol(msgid));
    messageReceived(m, id);
}

void YahooClient::process_fileurl(const char *id, const char *msg, const char *url)
{
    UrlMessage *m = new UrlMessage(MessageUrl);

    if (msg)
        m->setServerText(msg);
    m->setUrl(url);
    messageReceived(m, id);
}

void YahooClient::disconnected()
{
    m_values.clear();
    m_session_id = "";
    Contact *contact;
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        YahooUserData *data;
        ClientDataIterator it(contact->clientData, this);
        while ((data = (YahooUserData*)(++it)) != NULL){
            if (data->Status.value != YAHOO_STATUS_OFFLINE){
                data->Status.value = YAHOO_STATUS_OFFLINE;
                StatusMessage m;
                m.setContact(contact->id());
                m.setClient(dataName(data).c_str());
                m.setStatus(STATUS_OFFLINE);
                m.setFlags(MESSAGE_RECEIVED);
                Event e(EventMessageReceived, &m);
                e.process();
            }
        }
    }
    list<Message*>::iterator itm;
    for (itm = m_ackMsg.begin(); itm != m_ackMsg.end(); ++itm){
        Message *msg = *itm;
        Event e(EventMessageDeleted, msg);
        e.process();
        delete msg;
    }
    list<Message_ID>::iterator itw;
    for (itw = m_waitMsg.begin(); itw != m_waitMsg.end(); itw = m_waitMsg.begin()){
        Message *msg = (*itw).msg;
        msg->setError(I18N_NOOP("Client go offline"));
        Event e(EventMessageSent, msg);
        e.process();
        delete msg;
    }
}

bool YahooClient::isMyData(clientData *&_data, Contact*&contact)
{
    if (_data->Sign.value != YAHOO_SIGN)
        return false;
    YahooUserData *data = (YahooUserData*)_data;
    YahooUserData *my_data = findContact(data->Login.ptr, NULL, contact);
    if (!my_data){
        contact = NULL;
    }
    return true;
}

bool YahooClient::createData(clientData *&_data, Contact *contact)
{
    YahooUserData *data = (YahooUserData*)_data;
    YahooUserData *new_data = (YahooUserData*)(contact->clientData.createData(this));
    set_str(&new_data->Nick.ptr, data->Nick.ptr);
    _data = (clientData*)new_data;
    return true;
}

void YahooClient::setupContact(Contact*, void*)
{
}

QWidget	*YahooClient::setupWnd()
{
    return new YahooConfig(NULL, this, false);
}

QString YahooClient::getLogin()
{
    if (data.owner.Login.ptr)
        return QString::fromUtf8(data.owner.Login.ptr);
    return "";
}

void YahooClient::setLogin(const QString &login)
{
    set_str(&data.owner.Login.ptr, login.utf8());
}

void YahooClient::authOk()
{
    if (getState() == Connected)
        return;
    if (m_bHTTP && m_session_id.empty())
        return;
    setState(Connected);
    setPreviousPassword(NULL);
    setStatus(m_logonStatus);
    QTimer::singleShot(PING_TIMEOUT * 1000, this, SLOT(ping()));
}

void YahooClient::loadList(const char *str)
{
    Contact *contact;
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        YahooUserData *data;
        ClientDataIterator itd(contact->clientData, this);
        while ((data = (YahooUserData*)(++itd)) != NULL){
            data->bChecked.bValue = (contact->getGroup() == 0);
        }
    }
    if (str){
        string s = str;
        while (!s.empty()){
            string line = getToken(s, '\n');
            string grp = getToken(line, ':');
            if (line.empty()){
                line = grp;
                grp = "";
            }
            while (!line.empty()){
                string id = getToken(line, ',');
                ListRequest *lr = findRequest(id.c_str());
                if (lr)
                    continue;
                Contact *contact;
                YahooUserData *data = findContact(id.c_str(), grp.c_str(), contact, false);
                QString grpName;
                if (contact->getGroup()){
                    Group *grp = getContacts()->group(contact->getGroup());
                    if (grp)
                        grpName = grp->getName();
                }
                if (grpName != getContacts()->toUnicode(NULL, grp.c_str()))
                    moveBuddy(data, getContacts()->toUnicode(NULL, grp.c_str()));
                data->bChecked.bValue = true;
            }
        }
    }
    it.reset();
    for (list<ListRequest>::iterator itl = m_requests.begin(); itl != m_requests.end(); ++itl){
        if ((*itl).type == LR_CHANGE){
            YahooUserData *data = findContact((*itl).name.c_str(), NULL, contact, false);
            if (data){
                data->bChecked.bValue = true;
                QString grpName;
                if (contact->getGroup()){
                    Group *grp = getContacts()->group(contact->getGroup());
                    if (grp)
                        grpName = grp->getName();
                }
                if (grpName != getContacts()->toUnicode(NULL, data->Group.ptr))
                    moveBuddy(data, grpName.utf8());
            }
        }
        if ((*itl).type == LR_DELETE){
            YahooUserData data;
            load_data(yahooUserData, &data, NULL);
            set_str(&data.Login.ptr, (*itl).name.c_str());
            removeBuddy(&data);
            free_data(yahooUserData, &data);
        }
    }
    m_requests.clear();
    list<Contact*> forRemove;
    while ((contact = ++it) != NULL){
        YahooUserData *data;
        ClientDataIterator itd(contact->clientData, this);
        list<YahooUserData*> dataForRemove;
        bool bChanged = false;
        while ((data = (YahooUserData*)(++itd)) != NULL){
            if (!data->bChecked.bValue){
                dataForRemove.push_back(data);
                bChanged = true;
            }
        }
        if (!bChanged)
            continue;
        for (list<YahooUserData*>::iterator it = dataForRemove.begin(); it != dataForRemove.end(); ++it)
            contact->clientData.freeData(*it);
        if (contact->clientData.size()){
            Event e(EventContactChanged, contact);
            e.process();
        }else{
            forRemove.push_back(contact);
        }
    }
    for (list<Contact*>::iterator itr = forRemove.begin(); itr != forRemove.end(); ++itr)
        delete *itr;
}

YahooUserData *YahooClient::findContact(const char *id, const char *grpname, Contact *&contact, bool bSend, bool bJoin)
{
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        YahooUserData *data;
        ClientDataIterator itd(contact->clientData);
        while ((data = (YahooUserData*)(++itd)) != NULL){
            if (data->Login.ptr && !strcmp(id, data->Login.ptr))
                return data;
        }
    }
    it.reset();
    if (bJoin){
        while ((contact = ++it) != NULL){
            if (contact->getName() == id){
                YahooUserData *data = (YahooUserData*)contact->clientData.createData(this);
                set_str(&data->Login.ptr, id);
                set_str(&data->Group.ptr, grpname);
                Event e(EventContactChanged, contact);
                e.process();
                return data;
            }
        }
    }
    if (grpname == NULL)
        return NULL;
    Group *grp = NULL;
    if (*grpname){
        ContactList::GroupIterator it;
        while ((grp = ++it) != NULL)
            if (grp->getName() == getContacts()->toUnicode(NULL, grpname))
                break;
        if (grp == NULL){
            grp = getContacts()->group(0, true);
            grp->setName(getContacts()->toUnicode(NULL, grpname));
            Event e(EventGroupChanged, grp);
            e.process();
        }
    }
    if (grp == NULL)
        grp = getContacts()->group(0);
    contact = getContacts()->contact(0, true);
    YahooUserData *data = (YahooUserData*)(contact->clientData.createData(this));
    set_str(&data->Login.ptr, id);
    contact->setName(id);
    contact->setGroup(grp->id());
    Event e(EventContactChanged, contact);
    e.process();
    if (bSend)
        addBuddy(data);
    return data;
}

void YahooClient::messageReceived(Message *msg, const char *id)
{
    msg->setFlags(msg->getFlags() | MESSAGE_RECEIVED);
    if (msg->contact() == 0){
        Contact *contact;
        YahooUserData *data = findContact(id, NULL, contact);
        if (data == NULL){
            data = findContact(id, "", contact);
            if (data == NULL){
                delete msg;
                return;
            }
            contact->setFlags(CONTACT_TEMP);
            Event e(EventContactChanged, contact);
            e.process();
        }
        msg->setClient(dataName(data).c_str());
        msg->setContact(contact->id());
    }
    bool bAck = (msg->type() == MessageYahooFile);
    if (bAck){
        msg->setFlags(msg->getFlags() | MESSAGE_TEMP);
        m_ackMsg.push_back(msg);
    }
    Event e(EventMessageReceived, msg);
    if (e.process() && bAck){
        for (list<Message*>::iterator it = m_ackMsg.begin(); it != m_ackMsg.end(); ++it){
            if ((*it) == msg){
                m_ackMsg.erase(it);
                break;
            }
        }
    }
}

static void addIcon(string *s, const char *icon, const char *statusIcon)
{
    if (s == NULL)
        return;
    if (statusIcon && !strcmp(statusIcon, icon))
        return;
    string str = *s;
    while (!str.empty()){
        string item = getToken(str, ',');
        if (item == icon)
            return;
    }
    if (!s->empty())
        *s += ',';
    *s += icon;
}

void YahooClient::contactInfo(void *_data, unsigned long &status, unsigned&, const char *&statusIcon, string *icons)
{
    YahooUserData *data = (YahooUserData*)_data;
    unsigned cmp_status = STATUS_OFFLINE;
    switch (data->Status.value){
    case YAHOO_STATUS_AVAILABLE:
        cmp_status = STATUS_ONLINE;
        break;
    case YAHOO_STATUS_BUSY:
        cmp_status = STATUS_DND;
        break;
    case YAHOO_STATUS_NOTATHOME:
    case YAHOO_STATUS_NOTATDESK:
    case YAHOO_STATUS_NOTINOFFICE:
    case YAHOO_STATUS_ONVACATION:
        cmp_status = STATUS_NA;
        break;
    case YAHOO_STATUS_OFFLINE:
        break;
    case YAHOO_STATUS_CUSTOM:
        cmp_status = data->bAway.bValue ? STATUS_AWAY : STATUS_ONLINE;
        break;
    default:
        cmp_status = STATUS_AWAY;
    }

    const CommandDef *def;
    for (def = protocol()->statusList(); def->text; def++){
        if (def->id == cmp_status)
            break;
    }
    if (cmp_status > status){
        status = cmp_status;
        if (statusIcon && icons){
            string iconSave = *icons;
            *icons = statusIcon;
            if (iconSave.length())
                addIcon(icons, iconSave.c_str(), statusIcon);
        }
        statusIcon = def->icon;
    }else{
        if (statusIcon){
            addIcon(icons, def->icon, statusIcon);
        }else{
            statusIcon = def->icon;
        }
    }
    if (icons && data->bTyping.bValue)
        addIcon(icons, "typing", statusIcon);
}

QString YahooClient::contactTip(void *_data)
{
    YahooUserData *data = (YahooUserData*)_data;
    unsigned long status = STATUS_UNKNOWN;
    unsigned style  = 0;
    const char *statusIcon = NULL;
    contactInfo(data, status, style, statusIcon);
    QString res;
    res += "<img src=\"icon:";
    res += statusIcon;
    res += "\">";
    QString statusText;
    for (const CommandDef *cmd = protocol()->statusList(); cmd->text; cmd++){
        if (!strcmp(cmd->icon, statusIcon)){
            res += " ";
            statusText = i18n(cmd->text);
            res += statusText;
            break;
        }
    }
    res += "<br>";
    res += QString::fromUtf8(data->Login.ptr);
    res += "</b>";
    if (data->Status.value == YAHOO_STATUS_OFFLINE){
        if (data->StatusTime.value){
            res += "<br><font size=-1>";
            res += i18n("Last online");
            res += ": </font>";
            res += formatDateTime(data->StatusTime.value);
        }
    }else{
        if (data->OnlineTime.value){
            res += "<br><font size=-1>";
            res += i18n("Online");
            res += ": </font>";
            res += formatDateTime(data->OnlineTime.value);
        }
        if (data->Status.value != YAHOO_STATUS_AVAILABLE){
            res += "<br><font size=-1>";
            res += statusText;
            res += ": </font>";
            res += formatDateTime(data->StatusTime.value);
            QString msg;
            switch (data->Status.value){
            case YAHOO_STATUS_BRB:
                msg = i18n("Be right back");
                break;
            case YAHOO_STATUS_NOTATHOME:
                msg = i18n("Not at home");
                break;
            case YAHOO_STATUS_NOTATDESK:
                msg = i18n("Not at my desk");
                break;
            case YAHOO_STATUS_NOTINOFFICE:
                msg = i18n("Not in the office");
                break;
            case YAHOO_STATUS_ONPHONE:
                msg = i18n("On the phone");
                break;
            case YAHOO_STATUS_ONVACATION:
                msg = i18n("On vacation");
                break;
            case YAHOO_STATUS_OUTTOLUNCH:
                msg = i18n("Out to lunch");
                break;
            case YAHOO_STATUS_STEPPEDOUT:
                msg = i18n("Stepped out");
                break;
            case YAHOO_STATUS_CUSTOM:
                if (data->AwayMessage.ptr)
                    msg = QString::fromUtf8(data->AwayMessage.ptr);
            }
            if (!msg.isEmpty()){
                res += "<br>";
                res += quoteString(msg);
            }
        }
    }
    return res;
}

const unsigned MAIN_INFO = 1;
const unsigned NETWORK	 = 2;

static CommandDef yahooWnd[] =
    {
        {
            MAIN_INFO,
            "",
            "Yahoo!_online",
            NULL,
            NULL,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            NULL
        },
        {
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            NULL
        },
    };

static CommandDef cfgYahooWnd[] =
    {
        {
            MAIN_INFO,
            "",
            "Yahoo!_online",
            NULL,
            NULL,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            NULL
        },
        {
            NETWORK,
            I18N_NOOP("Network"),
            "network",
            NULL,
            NULL,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            NULL
        },
        {
            0,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            0,
            0,
            0,
            0,
            NULL,
            NULL
        },
    };

CommandDef *YahooClient::infoWindows(Contact*, void *_data)
{
    YahooUserData *data = (YahooUserData*)_data;
    QString name = i18n(protocol()->description()->text);
    name += " ";
    name += QString::fromUtf8(data->Login.ptr);
    yahooWnd[0].text_wrk = strdup(name.utf8());
    return yahooWnd;
}

CommandDef *YahooClient::configWindows()
{
    QString name = i18n(protocol()->description()->text);
    name += " ";
    name += QString::fromUtf8(data.owner.Login.ptr);
    cfgYahooWnd[0].text_wrk = strdup(name.utf8());
    return cfgYahooWnd;
}

QWidget *YahooClient::infoWindow(QWidget *parent, Contact*, void *_data, unsigned id)
{
    YahooUserData *data = (YahooUserData*)_data;
    switch (id){
    case MAIN_INFO:
        return new YahooInfo(parent, data, this);
    }
    return NULL;
}

QWidget *YahooClient::configWindow(QWidget *parent, unsigned id)
{
    switch (id){
    case MAIN_INFO:
        return new YahooInfo(parent, NULL, this);
    case NETWORK:
        return new YahooConfig(parent, this, true);
    }
    return NULL;
}

void YahooClient::ping()
{
    if (getState() != Connected)
        return;
    sendPacket(YAHOO_SERVICE_PING);
    QTimer::singleShot(PING_TIMEOUT * 1000, this, SLOT(ping()));
}

class YahooParser : public HTMLParser
{
public:
    YahooParser(const QString&);
    string res;
    bool bUtf;
protected:
    typedef struct style
    {
        QString		tag;
        QString		face;
        unsigned	size;
        unsigned	color;
        unsigned	state;
    } style;
    virtual void text(const QString &text);
    virtual void tag_start(const QString &tag, const list<QString> &options);
    virtual void tag_end(const QString &tag);
    void set_style(const style &s);
    void set_state(unsigned oldState, unsigned newState, unsigned st);
    void escape(const char *str);
    bool	m_bFirst;
    string   esc;
    stack<style>	tags;
    style	curStyle;
};

YahooParser::YahooParser(const QString &str)
{
    bUtf  = false;
    m_bFirst = true;
    curStyle.face  = "Arial";
    curStyle.size  = 10;
    curStyle.color = 0;
    curStyle.state = 0;
    parse(str);
}

void YahooParser::text(const QString &str)
{
    if (str.isEmpty())
        return;
    if (!bUtf){
        for (int i = 0; i < (int)str.length(); i++){
            if (str[i].unicode() > 0x7F){
                bUtf = true;
                break;
            }
        }
    }
    res += esc;
    esc = "";
    res += str.utf8();
}

void YahooParser::tag_start(const QString &tag, const list<QString> &options)
{
    if (tag == "img"){
        QString src;
        QString alt;
        for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
            QString name = (*it);
            ++it;
            QString value = (*it);
            if (name == "src"){
                src = value;
                break;
            }
            if (name == "alt"){
                alt = value;
                break;
            }
        }
        list<string> smiles = getIcons()->getSmile(src.latin1());
        if (smiles.empty()){
            text(alt);
            return;
        }
        text(QString::fromUtf8(smiles.front().c_str()));
        return;
    }
    if (tag == "br"){
        res += "\n";
        return;
    }
    style s = curStyle;
    s.tag = tag;
    tags.push(s);
    if (tag == "p"){
        if (!m_bFirst)
            res += "\n";
        m_bFirst = false;
    }
    if (tag == "font"){
        for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
            QString name = *it;
            ++it;
            if (name == "color"){
                QColor c;
                c.setNamedColor(*it);
                s.color = c.rgb() & 0xFFFFFF;
            }
        }
    }
    if (tag == "b"){
        s.state |= 1;
        return;
    }
    if (tag == "i"){
        s.state |= 2;
        return;
    }
    if (tag == "u"){
        s.state |= 4;
        return;
    }
    for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
        QString name = *it;
        ++it;
        if (name != "style")
            continue;
        list<QString> styles = parseStyle(*it);
        for (list<QString>::iterator its = styles.begin(); its != styles.end(); ++its){
            QString name = *its;
            ++its;
            if (name == "color"){
                QColor c;
                c.setNamedColor(*its);
                s.color = c.rgb() & 0xFFFFFF;
            }
            if (name == "font-size"){
                unsigned size = atol((*its).latin1());
                if (size)
                    s.size = size;
            }
            if (name == "font-family")
                s.face = (*its);
            if (name == "font-weight")
                s.state &= ~1;
            if (atol((*its).latin1()) >= 600)
                s.state |= 1;
            if ((name == "font-style") && ((*its) == "italic"))
                s.state |= 2;
            if ((name == "text-decoration") && ((*its) == "underline"))
                s.state |= 4;
        }
    }
    set_style(s);
}

void YahooParser::tag_end(const QString &tag)
{
    style saveStyle =curStyle;
    while (!tags.empty()){
        saveStyle = tags.top();
        tags.pop();
        if (saveStyle.tag == tag)
            break;
    }
    set_style(saveStyle);
}

void YahooParser::set_state(unsigned oldState, unsigned newState, unsigned st)
{
    string part;
    if ((oldState & st) == (newState & st))
        return;
    if ((newState & st) == 0)
        part = "x";
    part += number(st);
    escape(part.c_str());
}

void YahooParser::set_style(const style &s)
{
    set_state(curStyle.state, s.state, 1);
    set_state(curStyle.state, s.state, 2);
    set_state(curStyle.state, s.state, 4);
    curStyle.state = s.state;
    if (curStyle.color != s.color){
        curStyle.color = s.color;
        unsigned i;
        for (i = 0; i < 10; i++){
            if (esc_colors[i] == s.color){
                escape(number(30 + i).c_str());
                break;
            }
        }
        if (i >= 10){
            char b[10];
            sprintf(b, "#%06X", s.color & 0xFFFFFF);
            escape(b);
        }
    }
    QString fontAttr;
    if (curStyle.size != s.size){
        curStyle.size = s.size;
        fontAttr = QString(" size=\"%1\"") .arg(s.size);
    }
    if (curStyle.face != s.face){
        curStyle.face = s.face;
        fontAttr += QString(" face=\"%1\"") .arg(s.face);
    }
    if (!fontAttr.isEmpty()){
        esc += "<font";
        esc += fontAttr.utf8();
        esc += ">";
    }
}

void YahooParser::escape(const char *str)
{
    esc += "\x1B\x5B";
    esc += str;
    esc += "m";
}

void YahooClient::sendMessage(const QString &msgText, Message *msg, YahooUserData *data)
{
    YahooParser p(msgText);
    addParam(0, getLogin().utf8());
    addParam(1, getLogin().utf8());
    addParam(5, data->Login.ptr);
    addParam(14, p.res.c_str());
    if(p.bUtf)
        addParam(97, "1");
    addParam(63, ";0");
    addParam(64, "0");
    sendPacket(YAHOO_SERVICE_MESSAGE, 0x5A55AA56);
    if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
        msg->setClient(dataName(data).c_str());
        Event e(EventSent, msg);
        e.process();
    }
    Event e(EventMessageSent, msg);
    e.process();
    delete msg;
}

void YahooClient::sendTyping(YahooUserData *data, bool bState)
{
    addParam(5, data->Login.ptr);
    addParam(4, getLogin().utf8());
    addParam(14, " ");
    addParam(13, bState ? "1" : "0");
    addParam(49, "TYPING");
    sendPacket(YAHOO_SERVICE_NOTIFY, 0x16);
}

void YahooClient::addBuddy(YahooUserData *data)
{
    if ((getState() != Connected) || (data->Group.ptr == NULL))
        return;
    addParam(1, getLogin().utf8());
    addParam(7, data->Login.ptr);
    addParam(65, data->Group.ptr ? data->Group.ptr : "");
    sendPacket(YAHOO_SERVICE_ADDBUDDY);
}

void YahooClient::removeBuddy(YahooUserData *data)
{
    if (data->Group.ptr == NULL)
        return;
    addParam(1, getLogin().utf8());
    addParam(7, data->Login.ptr);
    addParam(65, data->Group.ptr ? data->Group.ptr : "");
    sendPacket(YAHOO_SERVICE_REMBUDDY);
    set_str(&data->Group.ptr, NULL);
}

void YahooClient::moveBuddy(YahooUserData *data, const char *grp)
{
    if (data->Group.ptr == NULL){
        if ((grp == NULL) || (*grp == 0))
            return;
        set_str(&data->Group.ptr, grp);
        addBuddy(data);
        return;
    }
    if ((grp == NULL) || (*grp == 0)){
        removeBuddy(data);
        return;
    }
    if (!strcmp(data->Group.ptr, grp))
        return;
    addParam(1, getLogin().utf8());
    addParam(7, data->Login.ptr);
    addParam(65, grp);
    sendPacket(YAHOO_SERVICE_ADDBUDDY);
    addParam(1, getLogin().utf8());
    addParam(7, data->Login.ptr);
    addParam(65, data->Group.ptr ? data->Group.ptr : "");
    sendPacket(YAHOO_SERVICE_REMBUDDY);
    set_str(&data->Group.ptr, grp);
}

void *YahooClient::processEvent(Event *e)
{
    TCPClient::processEvent(e);
    if (e->type() == EventContactChanged){
        Contact *contact = (Contact*)(e->param());
        string grpName;
        string name;
        name = contact->getName().utf8();
        Group *grp = NULL;
        if (contact->getGroup())
            grp = getContacts()->group(contact->getGroup());
        if (grp)
            grpName = grp->getName().utf8();
        ClientDataIterator it(contact->clientData, this);
        YahooUserData *data;
        while ((data = (YahooUserData*)(++it)) != NULL){
            if (getState() == Connected){
                moveBuddy(data, grpName.c_str());
            }else{
                ListRequest *lr = findRequest(data->Login.ptr);
                if (lr == NULL){
                    ListRequest r;
                    r.type = LR_CHANGE;
                    r.name = data->Login.ptr;
                    m_requests.push_back(r);
                }
            }
        }
    }
    if (e->type() == EventContactDeleted){
        Contact *contact = (Contact*)(e->param());
        ClientDataIterator it(contact->clientData, this);
        YahooUserData *data;
        while ((data = (YahooUserData*)(++it)) != NULL){
            if (getState() == Connected){
                removeBuddy(data);
            }else{
                ListRequest *lr = findRequest(data->Login.ptr);
                if (lr == NULL){
                    ListRequest r;
                    r.type = LR_DELETE;
                    r.name = data->Login.ptr;
                    m_requests.push_back(r);
                }
            }
        }
    }
    if (e->type() == EventTemplateExpanded){
        TemplateExpand *t = (TemplateExpand*)(e->param());
        sendStatus(YAHOO_STATUS_CUSTOM, getContacts()->fromUnicode(NULL, t->tmpl).c_str());
    }
    if (e->type() == EventMessageCancel){
        Message *msg = (Message*)(e->param());
        for (list<Message_ID>::iterator it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
            if ((*it).msg == msg){
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
                YahooFileMessage *msg = static_cast<YahooFileMessage*>(*it);
                m_ackMsg.erase(it);
                Contact *contact = getContacts()->contact(msg->contact());
                YahooUserData *data;
                ClientDataIterator it(contact->clientData, this);
                while ((data = (YahooUserData*)(++it)) != NULL){
                    if (dataName(data) == msg->client())
                        break;
                }
                if (data){
                    YahooFileTransfer *ft = new YahooFileTransfer(static_cast<FileMessage*>(msg), data, this);
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
                YahooFileMessage *msg = static_cast<YahooFileMessage*>(*it);
                m_ackMsg.erase(it);
                YahooUserData *data = NULL;
                Contact *contact = getContacts()->contact(msg->contact());
                if (contact){
                    ClientDataIterator itc(contact->clientData, this);
                    while ((data = (YahooUserData*)(++itc)) != NULL){
                        if (dataName(data) == msg->client())
                            break;
                    }
                }
                if (msg->getMsgID() && data){
                    addParam(5, data->Login.ptr);
                    addParam(49, "FILEXFER");
                    addParam(1, getLogin().utf8());
                    addParam(13, "2");
                    addParam(27, getContacts()->fromUnicode(contact, msg->getDescription()).c_str());
                    addParam(53, getContacts()->fromUnicode(contact, msg->getDescription()).c_str());
                    addParam(11, number(msg->getMsgID()).c_str());
                    sendPacket(YAHOO_SERVICE_P2PFILEXFER);
                }
                string reason = "";
                if (md->reason)
                    reason = md->reason;
                Event e(EventMessageDeleted, msg);
                e.process();
                delete msg;
                if (!reason.empty() && data){
                    Message *m = new Message(MessageGeneric);
                    m->setText(reason.c_str());
                    m->setFlags(MESSAGE_NOHISTORY);
                    if (!send(m, data))
                        delete m;
                }
                return msg;
            }
        }
        return NULL;
    }
    return NULL;
}

QWidget *YahooClient::searchWindow(QWidget *parent)
{
    if (getState() != Connected)
        return NULL;
    return new YahooSearch(this, parent);
}

void YahooClient::setInvisible(bool bState)
{
    if (bState == getInvisible())
        return;
    TCPClient::setInvisible(bState);
    if (getState() != Connected)
        return;
    sendStatus(data.owner.Status.value, data.owner.AwayMessage.ptr);
}

void YahooClient::sendStatus(unsigned long _status, const char *msg)
{
    unsigned long status = _status;
    if (getInvisible())
        status = YAHOO_STATUS_INVISIBLE;
    unsigned long service = YAHOO_SERVICE_ISAWAY;
    if (msg)
        status = YAHOO_STATUS_CUSTOM;
    /* data.owner.Status contains sim-status, not protocol-status! */
    if (data.owner.Status.value == STATUS_ONLINE)
        service = YAHOO_SERVICE_ISBACK;
    addParam(10, number(status).c_str());
    if ((status == YAHOO_STATUS_CUSTOM) && msg) {
        addParam(19, msg);
        addParam(47, "1");
    }
    sendPacket(service);
    if (data.owner.Status.value != status){
        time_t now;
        time(&now);
        data.owner.StatusTime.value = now;
    }
    data.owner.Status.value = _status;
    set_str(&data.owner.AwayMessage.ptr, msg);
}

void YahooClient::sendFile(FileMessage *msg, QFile *file, YahooUserData *data, unsigned short port)
{
    QString fn = file->name();
#ifdef WIN32
    fn = fn.replace(QRegExp("\\\\"), "/");
#endif
    int n = fn.findRev("/");
    if (n > 0)
        fn = fn.mid(n + 1);
    string url = "http://";
    struct in_addr addr;
    addr.s_addr = m_socket->localHost();
    url += inet_ntoa(addr);
    url += ":";
    url += number(port);
    url += "/";
    string nn;
    Contact *contact;
    findContact(data->Login.ptr, NULL, contact);
    string ff = getContacts()->fromUnicode(contact, fn);
    for (const char *p = ff.c_str(); *p; p++){
        if (((*p >= 'a') && (*p <='z')) || ((*p >= 'A') && (*p < 'Z')) || ((*p >= '0') && (*p <= '9')) || (*p == '.')){
            nn += *p;
        }else{
            nn += "_";
        }
    }
    url += nn;
    QString m = msg->getPlainText();
    addParam(5, data->Login.ptr);
    addParam(49, "FILEXFER");
    addParam(1, getLogin().utf8());
    addParam(13, "1");
    addParam(27, getContacts()->fromUnicode(contact, fn).c_str());
    addParam(28, number(file->size()).c_str());
    addParam(20, url.c_str());
    addParam(14, getContacts()->fromUnicode(contact, m).c_str());
    addParam(53, nn.c_str());
    addParam(11, number(++m_ft_id).c_str());
    addParam(54, "MSG1.0");
    sendPacket(YAHOO_SERVICE_P2PFILEXFER, YAHOO_STATUS_AVAILABLE);
    for (list<Message_ID>::iterator it = m_waitMsg.begin(); it != m_waitMsg.end(); ++it){
        if ((*it).msg == msg){
            (*it).id = m_ft_id;
            break;
        }
    }
}

static Message *createYahooFile(Buffer *cfg)
{
    return new YahooFileMessage(cfg);
}

static MessageDef defYahooFile =
    {
        NULL,
        NULL,
        MESSAGE_CHILD,
        "File",
        "%n files",
        createYahooFile,
        NULL,
        NULL
    };

static DataDef yahoMessageFile[] =
    {
        { "", DATA_STRING, 1, 0 },				// URL
        { "", DATA_ULONG, 1, 0 },
        { NULL, 0, 0, 0 }
    };

YahooFileMessage::YahooFileMessage(Buffer *cfg)
        : FileMessage(MessageYahooFile, cfg)
{
    load_data(yahoMessageFile, &data, cfg);
}

YahooFileMessage::~YahooFileMessage()
{
    free_data(yahoMessageFile, &data);
}

string YahooFileMessage::save()
{
    return save_data(yahoMessageFile, &data);
}

void YahooPlugin::registerMessages()
{
    Command cmd;
    cmd->id			= MessageYahooFile;
    cmd->text		= "YahooFile";
    cmd->icon		= "file";
    cmd->param		= &defYahooFile;
    Event eMsg(EventCreateMessageType, cmd);
    eMsg.process();
}

void YahooPlugin::unregisterMessages()
{
    Event eFile(EventRemoveMessageType, (void*)MessageYahooFile);
    eFile.process();
}

ListRequest *YahooClient::findRequest(const char *name)
{
    for (list<ListRequest>::iterator it = m_requests.begin(); it != m_requests.end(); ++it){
        if ((*it).name == name)
            return &(*it);
    }
    return NULL;
}

YahooFileTransfer::YahooFileTransfer(FileMessage *msg, YahooUserData *data, YahooClient *client)
        : FileTransfer(msg)
{
    m_data   = data;
    m_client = client;
    m_state  = None;
    m_socket = new ClientSocket(this);
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
}

YahooFileTransfer::~YahooFileTransfer()
{
    for (list<Message_ID>::iterator it = m_client->m_waitMsg.begin(); it != m_client->m_waitMsg.end(); ++it){
        if ((*it).msg == m_msg){
            m_client->m_waitMsg.erase(it);
            break;
        }
    }
    if (m_socket)
        delete m_socket;
}

void YahooFileTransfer::listen()
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
        return;
    }
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void YahooFileTransfer::startReceive(unsigned pos)
{
    m_startPos = pos;
    YahooFileMessage *msg = static_cast<YahooFileMessage*>(m_msg);
    string proto;
    string user;
    string pass;
    string uri;
    string extra;
    unsigned short port;
    FetchClient::crackUrl(msg->getUrl(), proto, m_host, port, user, pass, uri, extra);
    m_url = uri;
    if (!extra.empty()){
        m_url += "?";
        m_url += extra;
    }
    m_socket->connect(m_host.c_str(), port, m_client);
    m_state = Connect;
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
}

void YahooFileTransfer::bind_ready(unsigned short port)
{
    if (m_state == None){
        m_state = Listen;
    }else{
        m_state = ListenWait;
        FileTransfer::m_state = FileTransfer::Listen;
        if (m_notify)
            m_notify->process();
    }
    m_client->sendFile(m_msg, m_file, m_data, port);
}

bool YahooFileTransfer::error(const char *err)
{
    error_state(err, 0);
    return true;
}

bool YahooFileTransfer::accept(Socket *s, unsigned long)
{
    if (m_state == Listen){
        Event e(EventMessageAcked, m_msg);
        e.process();
    }
    m_state = ListenWait;
    log(L_DEBUG, "Accept connection");
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
    Socket *old_s = m_socket->socket();
    m_socket->setSocket(s);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    m_answer = 400;
    if (old_s)
        delete old_s;
    return false;
}

bool YahooFileTransfer::error_state(const char *err, unsigned)
{
    if ((m_state == Wait) || (m_state == Skip))
        return false;
    if (FileTransfer::m_state != FileTransfer::Done){
        m_state = None;
        FileTransfer::m_state = FileTransfer::Error;
        m_msg->setError(err);
        if (m_notify)
            m_notify->process();
    }
    m_msg->m_transfer = NULL;
    m_msg->setFlags(m_msg->getFlags() & ~MESSAGE_TEMP);
    Event e(EventMessageSent, m_msg);
    e.process();
    return true;
}

void YahooFileTransfer::packet_ready()
{
    if (m_socket->readBuffer.writePos() == 0)
        return;
    if (m_state == Skip)
        return;
    if (m_state != Receive){
        log_packet(m_socket->readBuffer, false, YahooPlugin::YahooPacket);
        for (;;){
            string s;
            if (!m_socket->readBuffer.scan("\n", s))
                break;
            if (!s.empty() && (s[s.length() - 1] == '\r'))
                s = s.substr(0, s.length() - 1);
            if (!get_line(s.c_str()))
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

void YahooFileTransfer::connect_ready()
{
    string line;
    line = "GET /";
    line += m_url;
    line += " HTTP/1.1\r\n"
            "Host :";
    line += m_host;
    line += "\r\n";
    if (m_startPos){
        line += "Range: ";
        line += number(m_startPos);
        line += "-\r\n";
    }
    m_startPos = 0;
    m_endPos   = 0xFFFFFFFF;
    send_line(line.c_str());
    FileTransfer::m_state = FileTransfer::Negotiation;
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
}

void YahooFileTransfer::write_ready()
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
            }else{
                if (isDirectory())
                    continue;
                m_state = Wait;
                FileTransfer::m_state = FileTransfer::Wait;
                if (!((Client*)m_client)->send(m_msg, m_data))
                    error_state(I18N_NOOP("File transfer failed"), 0);
                break;
            }
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

bool YahooFileTransfer::get_line(const char *str)
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
        string s;
        s = "HTTP/1.0 ";
        s += number(m_answer);
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
        send_line(s.c_str());
        if ((m_answer == 200) || (m_answer == 206)){
            send_line("Content-Type: application/data");
            s = "Content-Length: ";
            s += number(m_endPos - m_startPos);
            send_line(s.c_str());
        }
        if (m_answer == 206){
            s = "Range: ";
            s += number(m_startPos);
            s += "-";
            s += number(m_endPos);
            send_line(s.c_str());
        }
        send_line("");
        if (m_method == "HEAD"){
            m_state = Skip;
            return false;
        }
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
        if ((t == "GET") || (t == "HEAD")){
            m_method = t;
            m_answer = 200;
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
        if ((t == "Content-Length") || (t == "Content-length")){
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

void YahooFileTransfer::send_line(const char *line)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << line;
    m_socket->writeBuffer << "\r\n";
    log_packet(m_socket->writeBuffer, true, YahooPlugin::YahooPacket);
    m_socket->write();
}

void YahooFileTransfer::connect()
{
    m_nFiles = 1;
    if (m_notify)
        m_notify->createFile(m_msg->getDescription(), 0xFFFFFFFF, false);
}

#ifndef _MSC_VER
#include "yahooclient.moc"
#endif




