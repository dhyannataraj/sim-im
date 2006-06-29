/***************************************************************************
                          icqdirect.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
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

#include "simapi.h"

#include "icqclient.h"
#include "icqmessage.h"

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

#include <time.h>

#include <qfile.h>
#include <qtimer.h>
#include <qregexp.h>

using namespace std;
using namespace SIM;

const unsigned short TCP_START  = 0x07EE;
const unsigned short TCP_ACK    = 0x07DA;
const unsigned short TCP_CANCEL	= 0x07D0;

const char FT_INIT		= 0;
const char FT_INIT_ACK	= 1;
const char FT_FILEINFO	= 2;
const char FT_START		= 3;
const char FT_SPEED		= 5;
const char FT_DATA		= 6;

const unsigned DIRECT_TIMEOUT	= 20;

ICQListener::ICQListener(ICQClient *client)
{
    m_client = client;
}

ICQListener::~ICQListener()
{
    if (m_client == NULL)
        return;
    m_client->m_listener = NULL;
    m_client->data.owner.Port.asULong() = 0;
}

bool ICQListener::accept(Socket *s, unsigned long ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    log(L_DEBUG, "Accept direct connection %s", inet_ntoa(addr));
    m_client->m_sockets.push_back(new DirectClient(s, m_client, ip));
    return false;
}

void ICQListener::bind_ready(unsigned short port)
{
    m_client->data.owner.Port.asULong() = port;
}

bool ICQListener::error(const char *err)
{
    log(L_WARN, "ICQListener error: %s", err);
    m_client->m_listener = NULL;
    m_client->data.owner.Port.asULong() = 0;
    m_client = NULL;
    return true;
}

// ___________________________________________________________________________________________

DirectSocket::DirectSocket(Socket *s, ICQClient *client, unsigned long ip)
{
    m_socket = new ClientSocket(this);
    m_socket->setSocket(s);
    m_bIncoming = true;
    m_client = client;
    m_state = WaitInit;
    m_version = 0;
    m_data	= NULL;
    m_port  = 0;
    m_ip    = ip;
    init();
}

DirectSocket::DirectSocket(ICQUserData *data, ICQClient *client)
{
    m_socket    = new ClientSocket(this);
    m_bIncoming = false;
    m_version   = (char)(data->Version.toULong());
    m_client    = client;
    m_state     = NotConnected;
    m_data		= data;
    m_port		= 0;
    m_localPort = 0;
    m_ip		= 0;
    init();
}

DirectSocket::~DirectSocket()
{
    if (m_socket)
        delete m_socket;
    removeFromClient();
}

void DirectSocket::timeout()
{
    if ((m_state != Logged) && m_socket)
        login_timeout();
}

void DirectSocket::login_timeout()
{
    m_socket->error_state("Timeout direct connection");
    if (m_data)
        m_data->bNoDirect.asBool() = true;
}

void DirectSocket::removeFromClient()
{
    for (list<DirectSocket*>::iterator it = m_client->m_sockets.begin(); it != m_client->m_sockets.end(); ++it){
        if (*it == this){
            m_client->m_sockets.erase(it);
            break;
        }
    }
}

void DirectSocket::init()
{
    if (!m_socket->created())
        m_socket->error_state("Connect error");
    m_nSequence = 0xFFFF;
    m_socket->writeBuffer.init(0);
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
}

unsigned long DirectSocket::Uin()
{
    if (m_data)
        return m_data->Uin.toULong();
    return 0;
}

unsigned short DirectSocket::localPort()
{
    return m_localPort;
}

unsigned short DirectSocket::remotePort()
{
    return m_port;
}

bool DirectSocket::error_state(const QString &error, unsigned)
{
    if ((m_state == ConnectIP1) || (m_state == ConnectIP2)){
        connect();
        return false;
    }
    if (!error.isEmpty())
        log(L_WARN, "Direct socket error %s", error.latin1());
    return true;
}

void DirectSocket::connect()
{
    m_socket->writeBuffer.init(0);
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
    if (m_port == 0){
        m_state = ConnectFail;
        m_socket->error_state(I18N_NOOP("Connect to unknown port"));
        return;
    }
    if (m_state == NotConnected){
        m_state = ConnectIP1;
        unsigned long ip = get_ip(m_data->RealIP);
        if (get_ip(m_data->IP) != get_ip(m_client->data.owner.IP))
            ip = 0;
        if (ip){
            struct in_addr addr;
            addr.s_addr = ip;
            m_socket->connect(inet_ntoa(addr), m_port, NULL);
            return;
        }
    }
    if (m_state == ConnectIP1){
        m_state = ConnectIP2;
        unsigned long ip = get_ip(m_data->IP);
        if ((ip == get_ip(m_client->data.owner.IP)) && (ip == get_ip(m_data->RealIP)))
            ip = 0;
        if (ip){
            struct in_addr addr;
            addr.s_addr = ip;
            m_socket->connect(inet_ntoa(addr), m_port, m_client);
            return;
        }
    }
    m_state = ConnectFail;
    m_socket->error_state(I18N_NOOP("Can't established direct connection"));
}

void DirectSocket::reverseConnect(unsigned long ip, unsigned short port)
{
    if (m_state != NotConnected){
        log(L_WARN, "Bad state for reverse connect");
        return;
    }
    m_bIncoming = true;
    m_state = ReverseConnect;
    struct in_addr addr;
    addr.s_addr = ip;
    m_socket->connect(inet_ntoa(addr), port, NULL);
}

void DirectSocket::acceptReverse(Socket *s)
{
    if (m_state != WaitReverse){
        log(L_WARN, "Accept reverse in bad state");
        if (s)
            delete s;
        return;
    }
    if (s == NULL){
        m_socket->error_state("Reverse fail");
        return;
    }
    delete m_socket->socket();
    m_socket->setSocket(s);
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader   = true;
    m_state     = WaitInit;
    m_bIncoming = true;
}

void DirectSocket::packet_ready()
{
    if (m_bHeader){
        unsigned short size;
        m_socket->readBuffer.unpack(size);
        if (size){
            m_socket->readBuffer.add(size);
            m_bHeader = false;
            return;
        }
    }
    if (m_state != Logged){
        ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
        log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, QString::number((unsigned long)this));
    }
    switch (m_state){
    case Logged:{
            processPacket();
            break;
        }
    case WaitAck:{
            unsigned short s1, s2;
            m_socket->readBuffer.unpack(s1);
            m_socket->readBuffer.unpack(s2);
            if (s2 != 0){
                m_socket->error_state("Bad ack");
                return;
            }
            if (m_bIncoming){
                m_state = Logged;
                connect_ready();
            }else{
                m_state = WaitInit;
            }
            break;
        }
    case WaitInit:{
            char cmd;
            m_socket->readBuffer.unpack(cmd);
            if ((unsigned char)cmd != 0xFF){
                m_socket->error_state("Bad direct init command");
                return;
            }
            m_socket->readBuffer.unpack(m_version);
            if (m_version < 6){
                m_socket->error_state("Use old protocol");
                return;
            }
            m_socket->readBuffer.incReadPos(3);
            unsigned long my_uin;
            m_socket->readBuffer.unpack(my_uin);
            if (my_uin != m_client->data.owner.Uin.toULong()){
                m_socket->error_state("Bad owner UIN");
                return;
            }
            m_socket->readBuffer.incReadPos(6);
            unsigned long p_uin;
            m_socket->readBuffer.unpack(p_uin);
            if (m_data == NULL){
                Contact *contact;
                m_data = m_client->findContact(p_uin, NULL, false, contact);
                if ((m_data == NULL) || contact->getIgnore()){
                    m_socket->error_state("User not found");
                    return;
                }
                if ((m_client->getInvisible() && (m_data->VisibleId.toULong() == 0)) ||
                        (!m_client->getInvisible() && m_data->InvisibleId.toULong())){
                    m_socket->error_state("User not found");
                    return;
                }
            }
            if (p_uin != m_data->Uin.toULong()){
                m_socket->error_state("Bad sender UIN");
                return;
            }
            if (get_ip(m_data->RealIP) == 0)
                set_ip(&m_data->RealIP, m_ip);
            m_socket->readBuffer.incReadPos(13);
            unsigned long sessionId;
            m_socket->readBuffer.unpack(sessionId);
            if (m_bIncoming){
                m_nSessionId = sessionId;
                sendInitAck();
                sendInit();
                m_state = WaitAck;
            }else{
                if (sessionId != m_nSessionId){
                    m_socket->error_state("Bad session ID");
                    return;
                }
                sendInitAck();
                m_state = Logged;
                connect_ready();
            }
            break;
        }
    default:
        m_socket->error_state("Bad session ID");
        return;
    }
    if (m_socket == NULL){
        delete this;
        return;
    }
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
}

void DirectSocket::sendInit()
{
    if (!m_bIncoming && (m_state != ReverseConnect)){
        if (m_data->DCcookie.toULong() == 0){
            m_socket->error_state("No direct info");
            return;
        }
        m_nSessionId = m_data->DCcookie.toULong();
    }

    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack((unsigned short)((m_version >= 7) ? 0x0030 : 0x002c));
    m_socket->writeBuffer.pack('\xFF');
    m_socket->writeBuffer.pack((unsigned short)m_version);
    m_socket->writeBuffer.pack((unsigned short)((m_version >= 7) ? 0x002b : 0x0027));
    m_socket->writeBuffer.pack(m_data->Uin.toULong());
    m_socket->writeBuffer.pack((unsigned short)0x0000);
    m_socket->writeBuffer.pack((unsigned long)m_data->Port.toULong());
    m_socket->writeBuffer.pack(m_client->data.owner.Uin.toULong());
    m_socket->writeBuffer.pack(get_ip(m_client->data.owner.IP));
    m_socket->writeBuffer.pack(get_ip(m_client->data.owner.RealIP));
    m_socket->writeBuffer.pack((char)0x04);
    m_socket->writeBuffer.pack(m_data->Port.toULong());
    m_socket->writeBuffer.pack(m_nSessionId);
    m_socket->writeBuffer.pack(0x00000050L);
    m_socket->writeBuffer.pack(0x00000003L);
    if (m_version >= 7)
        m_socket->writeBuffer.pack(0x00000000L);
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, QString::number((unsigned long)this));
    m_socket->write();
}

void DirectSocket::sendInitAck()
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack((unsigned short)0x0004);
    m_socket->writeBuffer.pack((unsigned short)0x0001);
    m_socket->writeBuffer.pack((unsigned short)0x0000);
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, QString::number((unsigned long)this));
    m_socket->write();
}

void DirectSocket::connect_ready()
{
    QTimer::singleShot(DIRECT_TIMEOUT * 1000, this, SLOT(timeout()));
    if (m_bIncoming){
        if (m_state == ReverseConnect)
            m_state = WaitInit;
    }else{
        sendInit();
        m_state = WaitAck;
    }
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
}

// ___________________________________________________________________________________________

static unsigned char client_check_data[] =
    {
        "As part of this software beta version Mirabilis is "
        "granting a limited access to the ICQ network, "
        "servers, directories, listings, information and databases (\""
        "ICQ Services and Information\"). The "
        "ICQ Service and Information may databases (\""
        "ICQ Services and Information\"). The "
        "ICQ Service and Information may\0"
    };

DirectClient::DirectClient(Socket *s, ICQClient *client, unsigned long ip)
        : DirectSocket(s, client, ip)
{
    m_channel = PLUGIN_NULL;
    m_state = WaitLogin;
#ifdef USE_OPENSSL
    m_ssl = NULL;
#endif
}

DirectClient::DirectClient(ICQUserData *data, ICQClient *client, unsigned channel)
        : DirectSocket(data, client)
{
    m_state   = None;
    m_channel = channel;
    m_port    = (unsigned short)(data->Port.toULong());
#ifdef USE_OPENSSL
    m_ssl = NULL;
#endif
}

DirectClient::~DirectClient()
{
    error_state(NULL, 0);
    switch (m_channel){
    case PLUGIN_NULL:
        if (m_data && ((m_data->Direct.object()) == this))
            m_data->Direct.clear();
        break;
    case PLUGIN_INFOxMANAGER:
        if (m_data && ((m_data->DirectPluginInfo.object()) == this))
            m_data->DirectPluginInfo.clear();
        break;
    case PLUGIN_STATUSxMANAGER:
        if (m_data && ((m_data->DirectPluginStatus.object()) == this))
            m_data->DirectPluginStatus.clear();
        break;
    }
#ifdef USE_OPENSSL
    secureStop(false);
#endif
}

bool DirectClient::isSecure()
{
#ifdef USE_OPENSSL
    return m_ssl && m_ssl->connected();
#else
    return false;
#endif
}

void DirectClient::processPacket()
{
    switch (m_state){
    case None:
        m_socket->error_state("Bad state process packet");
        return;
    case WaitInit2:
        if (m_bIncoming){
            ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
            log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, QString::number((unsigned long)this));
            if (m_version < 8){
                if (m_data->Direct.object()){
                    m_socket->error_state("Direct connection already established");
                    return;
                }
                m_state = Logged;
                processMsgQueue();
                break;
            }
            plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
            log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, QString::number((unsigned long)this));
            m_socket->readBuffer.incReadPos(13);
            char p[16];
            m_socket->readBuffer.unpack(p, 16);
            for (m_channel = 0; m_channel <= PLUGIN_NULL; m_channel++){
                if (!memcmp(m_client->plugins[m_channel], p, 16))
                    break;
            }
            removeFromClient();
            switch (m_channel){
            case PLUGIN_INFOxMANAGER: {
                DirectClient *dc = dynamic_cast<DirectClient*>(m_data->Direct.object());
                if (dc){
                    if (dc->copyQueue(this)){
                        delete dc;
                        m_data->DirectPluginInfo.setObject(this);
                    }else{
                        m_socket->error_state("Plugin info connection already established");
                    }
                }else{
                    m_data->DirectPluginInfo.setObject(this);
                }
                break;
            }
            case PLUGIN_STATUSxMANAGER: {
                DirectClient *dc = dynamic_cast<DirectClient*>(m_data->Direct.object());
                if (dc){
                    if (dc->copyQueue(this)){
                        delete dc;
                        m_data->DirectPluginStatus.setObject(this);
                    }else{
                        m_socket->error_state("Plugin status connection already established");
                    }
                }else{
                    m_data->DirectPluginStatus.setObject(this);
                }
                break;
            }
            case PLUGIN_NULL: {
                DirectClient *dc = dynamic_cast<DirectClient*>(m_data->Direct.object());
                if (dc){
                    if (dc->copyQueue(this)){
                        delete dc;
                        m_data->Direct.setObject(this);
                    }else{
                        m_socket->error_state("Direct connection already established");
                    }
                }else{
                    m_data->Direct.setObject(this);
                }
                break;
            }
            default:
                m_socket->error_state("Unknown direct channel");
                return;
            }
            sendInit2();
        }
        m_state = Logged;
        processMsgQueue();
        return;
    default:
        break;
    }
    unsigned long hex, key, B1, M1;
    unsigned int i;
    unsigned char X1, X2, X3;

    unsigned int correction = 2;
    if (m_version >= 7)
        correction++;

    unsigned int size = m_socket->readBuffer.size() - correction;
    if (m_version >= 7) m_socket->readBuffer.incReadPos(1);

    unsigned long check;
    m_socket->readBuffer.unpack(check);

    // main XOR key
    key = 0x67657268 * size + check;

    unsigned char *p = (unsigned char*)m_socket->readBuffer.data(m_socket->readBuffer.readPos()-4);
    for(i=4; i<(size+3)/4; i+=4) {
        hex = key + client_check_data[i&0xFF];
        p[i] ^= hex&0xFF;
        p[i+1] ^= (hex>>8) & 0xFF;
        p[i+2] ^= (hex>>16) & 0xFF;
        p[i+3] ^= (hex>>24) & 0xFF;
    }

    B1 = (p[4] << 24) | (p[6] << 16) | (p[4] <<8) | (p[6]<<0);

    // special decryption
    B1 ^= check;

    // validate packet
    M1 = (B1 >> 24) & 0xFF;
    if(M1 < 10 || M1 >= size){
        m_socket->error_state("Decrypt packet failed");
        return;
    }

    X1 = (unsigned char)(p[M1] ^ 0xFF);
    if(((B1 >> 16) & 0xFF) != X1){
        m_socket->error_state("Decrypt packet failed");
        return;
    }

    X2 = (unsigned char)((B1 >> 8) & 0xFF);
    if(X2 < 220) {
        X3 = (unsigned char)(client_check_data[X2] ^ 0xFF);
        if((B1 & 0xFF) != X3){
            m_socket->error_state("Decrypt packet failed");
            return;
        }
    }
    ICQPlugin *icq_plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->readBuffer, false, icq_plugin->ICQDirectPacket, name());

    m_socket->readBuffer.setReadPos(2);
    if (m_version >= 7){
        char startByte;
        m_socket->readBuffer.unpack(startByte);
        if (startByte != 0x02){
            m_socket->error_state("Bad start byte");
            return;
        }
    }
    unsigned long checksum;
    m_socket->readBuffer.unpack(checksum);
    unsigned short command;
    m_socket->readBuffer.unpack(command);
    m_socket->readBuffer.incReadPos(2);
    unsigned short seq;
    m_socket->readBuffer.unpack(seq);
    m_socket->readBuffer.incReadPos(12);

    unsigned short type, ackFlags, msgFlags;
    m_socket->readBuffer.unpack(type);
    m_socket->readBuffer.unpack(ackFlags);
    m_socket->readBuffer.unpack(msgFlags);
    QCString msg_str;
    m_socket->readBuffer >> msg_str;
    Message *m;
    list<SendDirectMsg>::iterator it;
    switch (command){
    case TCP_START:
        switch (type){
        case ICQ_MSGxAR_AWAY:
        case ICQ_MSGxAR_OCCUPIED:
        case ICQ_MSGxAR_NA:
        case ICQ_MSGxAR_DND:
        case ICQ_MSGxAR_FFC:{
                unsigned req_status = STATUS_AWAY;
                switch (type){
                case ICQ_MSGxAR_OCCUPIED:
                    req_status = STATUS_OCCUPIED;
                    break;
                case ICQ_MSGxAR_NA:
                    req_status = STATUS_NA;
                    break;
                case ICQ_MSGxAR_DND:
                    req_status = STATUS_DND;
                    break;
                case ICQ_MSGxAR_FFC:
                    req_status = STATUS_FFC;
                    break;
                }
                ar_request req;
                req.screen  = m_client->screen(m_data);
                req.type    = type;
                req.flags   = msgFlags;
                req.id.id_l = seq;
                req.id1     = 0;
                req.id2     = 0;
                req.bDirect = true;
                m_client->arRequests.push_back(req);

                Contact *contact = NULL;
                m_client->findContact(m_client->screen(m_data), NULL, false, contact);
                ARRequest ar;
                ar.contact  = contact;
                ar.param    = &m_client->arRequests.back();
                ar.receiver = m_client;
                ar.status   = req_status;
                Event e(EventARRequest, &ar);
                e.process();
                return;
            }
        case ICQ_MSGxSECURExOPEN:
        case ICQ_MSGxSECURExCLOSE:
            msg_str = "";
#ifdef USE_OPENSSL
            msg_str = "1";
#endif
            sendAck(seq, type, msgFlags, msg_str);
#ifdef USE_OPENSSL
            if (type == ICQ_MSGxSECURExOPEN){
                secureListen();
            }else{
                secureStop(true);
            }
#endif
            return;
        }
        if (m_channel == PLUGIN_NULL){
            MessageId id;
            id.id_l = seq;
            m = m_client->parseMessage(type, m_client->screen(m_data), msg_str, m_socket->readBuffer, id, 0);
            if (m == NULL){
                m_socket->error_state("Start without message");
                return;
            }
            unsigned flags = m->getFlags() | MESSAGE_RECEIVED | MESSAGE_DIRECT;
            if (isSecure())
                flags |= MESSAGE_SECURE;
            m->setFlags(flags);
            bool bAccept = true;
            switch (m_client->getStatus()){
            case STATUS_DND:
                if (!m_client->getAcceptInDND())
                    bAccept = false;
                break;
            case STATUS_OCCUPIED:
                if (!m_client->getAcceptInOccupied())
                    bAccept = false;
                break;
            }
            if (msgFlags & (ICQ_TCPxMSG_URGENT | ICQ_TCPxMSG_LIST))
                bAccept = true;
            if (bAccept){
                if (msgFlags & ICQ_TCPxMSG_URGENT)
                    m->setFlags(m->getFlags() | MESSAGE_URGENT);
                if (msgFlags & ICQ_TCPxMSG_LIST)
                    m->setFlags(m->getFlags() | MESSAGE_LIST);
                if (m_client->messageReceived(m, m_client->screen(m_data)))
                    sendAck(seq, type, msgFlags);
            }else{
                sendAck(seq, type, ICQ_TCPxMSG_AUTOxREPLY);
                delete m;
            }
        }else{
            plugin p;
            m_socket->readBuffer.unpack((char*)p, sizeof(p));
            unsigned plugin_index;
            for (plugin_index = 0; plugin_index < PLUGIN_NULL; plugin_index++){
                if (!memcmp(p, m_client->plugins[plugin_index], sizeof(p)))
                    break;
            }
            Buffer info;
            unsigned short type = 1;
            switch (plugin_index){
            case PLUGIN_FILESERVER:
            case PLUGIN_FOLLOWME:
            case PLUGIN_ICQPHONE:
                type = 2;
            case PLUGIN_PHONEBOOK:
            case PLUGIN_PICTURE:
            case PLUGIN_QUERYxINFO:
            case PLUGIN_QUERYxSTATUS:
                m_client->pluginAnswer(plugin_index, m_data->Uin.toULong(), info);
                startPacket(TCP_ACK, seq);
                m_socket->writeBuffer.pack(type);
                m_socket->writeBuffer << 0x00000000L
                << (char)1
                << type;
                m_socket->writeBuffer.pack(info.data(0), info.size());
                sendPacket();
                break;
            default:
                log(L_WARN, "Unknwon direct plugin request %u", plugin_index);
                break;
            }
        }
        break;
    case TCP_CANCEL:
	case TCP_ACK: {
        log(L_DEBUG, "Ack %X %X", ackFlags, msgFlags);
        if(m_queue.empty()) {
            log(L_DEBUG, "TCP_ACK/TCP_CANCEL with empty queue");
            break;
        }
		bool itDeleted = false;
        for (it = m_queue.begin(); it != m_queue.end(); ++it){
            if ((*it).seq != seq)
                continue;
            if ((*it).msg == NULL){
                if ((*it).type == PLUGIN_AR){
                    m_data->AutoReply.str() = msg_str;
                    m_queue.erase(it);
					itDeleted = true;
                    break;
                }
                unsigned plugin_index = (*it).type;
                switch (plugin_index){
                case PLUGIN_FILESERVER:
                case PLUGIN_FOLLOWME:
                case PLUGIN_ICQPHONE:
                    m_socket->readBuffer.incReadPos(-3);
                    break;
                case PLUGIN_QUERYxSTATUS:
                    m_socket->readBuffer.incReadPos(9);
                    break;
                }
                m_client->parsePluginPacket(m_socket->readBuffer, plugin_index, m_data, m_data->Uin.toULong(), true);
                m_queue.erase(it);
				itDeleted = true;
				break;
            }
            Message *msg = (*it).msg;
            if (command == TCP_CANCEL){
                Event e(EventMessageCancel, msg);
                e.process();
                delete msg;
                break;
            }
            MessageId id;
            id.id_l = seq;
            Message *m = m_client->parseMessage(type, m_client->screen(m_data), msg_str, m_socket->readBuffer, id, 0);
            switch (msg->type()){
#ifdef USE_OPENSSL
            case MessageCloseSecure:
                secureStop(true);
                break;
            case MessageOpenSecure:
                if (msg_str.isEmpty()){
                    msg->setError(I18N_NOOP("Other side does not support the secure connection"));
                }else{
                    secureConnect();
                }
                return;
#endif
            case MessageFile:
                if (m == NULL){
                    m_socket->error_state("Ack without message");
                    return;
                }
                if (ackFlags){
                    if (msg_str.isEmpty()){
                        msg->setError(I18N_NOOP("Send message fail"));
                    }else{
                        QString err = getContacts()->toUnicode(m_client->getContact(m_data), msg_str);
                        msg->setError(err);
                    }
                    Event e(EventMessageSent, msg);
                    e.process();
                    m_queue.erase(it);
                    delete msg;
                }else{
                    if (m->type() != MessageICQFile){
                        m_socket->error_state("Bad message type in ack file");
                        return;
                    }
                    ICQFileTransfer *ft = new ICQFileTransfer(static_cast<FileMessage*>(msg), m_data, m_client);
                    Event e(EventMessageAcked, msg);
                    e.process();
                    m_queue.erase(it);
                    m_client->m_processMsg.push_back(msg);
                    ft->connect(static_cast<ICQFileMessage*>(m)->getPort());
                }
                return;
            }
            unsigned flags = msg->getFlags() | MESSAGE_DIRECT;
            if (isSecure())
                flags |= MESSAGE_SECURE;
            if (m_client->ackMessage(msg, ackFlags, msg_str)){
                if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                    if (msg->type() == MessageGeneric){
                        Message m;
                        m.setContact(msg->contact());
                        m.setClient(msg->client());
                        if ((*it).type == CAP_RTF){
                            m.setText(m_client->removeImages(msg->getRichText(), true));
                            flags |= MESSAGE_RICHTEXT;
                        }else{
                            m.setText(msg->getPlainText());
                        }
                        m.setFlags(flags);
                        if (msg->getBackground() != msg->getForeground()){
                            m.setForeground(msg->getForeground());
                            m.setBackground(msg->getBackground());
                        }
                        Event e(EventSent, &m);
                        e.process();
                    }else if ((msg->type() != MessageOpenSecure) && (msg->type() != MessageCloseSecure)){
                        msg->setFlags(flags);
                        Event e(EventSent, msg);
                        e.process();
                    }
                }
            }
            Event e(EventMessageSent, msg);
            e.process();
            m_queue.erase(it);
            delete msg;
            break;
        }
        if (!itDeleted && (m_queue.size() == 0 || it == m_queue.end())){
            list<Message*>::iterator it;
            for (it = m_client->m_acceptMsg.begin(); it != m_client->m_acceptMsg.end(); ++it){
                QString name = m_client->dataName(m_data);
                Message *msg = *it;
                if ((msg->getFlags() & MESSAGE_DIRECT) &&
                        msg->client() && (name == msg->client())){
                    bool bFound = false;
                    switch (msg->type()){
                    case MessageICQFile:
                        if (static_cast<ICQFileMessage*>(msg)->getID_L() == seq)
                            bFound = true;
                        break;
                    }
                    if (bFound){
                        m_client->m_acceptMsg.erase(it);
                        Event e(EventMessageDeleted, msg);
                        e.process();
                        delete msg;
                        break;
                    }
                }
            }
            if (it == m_client->m_acceptMsg.end())
                log(L_WARN, "Message for ACK not found");
        }
        break;
	}
    default:
        m_socket->error_state("Unknown TCP command");
    }
}

bool DirectClient::copyQueue(DirectClient *to)
{
    if (m_state == Logged)
        return false;
    for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it)
        to->m_queue.push_back(*it);
    m_queue.clear();
    return true;
}

void DirectClient::connect_ready()
{
    if (m_state == None){
        m_state = WaitLogin;
        DirectSocket::connect_ready();
        return;
    }
    if (m_state == SSLconnect){
        for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
            SendDirectMsg &sm = *it;
            if ((sm.msg == NULL) || (sm.msg->type() != MessageOpenSecure))
                continue;
            Event e(EventMessageSent, sm.msg);
            e.process();
            delete sm.msg;
            m_queue.erase(it);
            break;
        }
        m_state = Logged;
        Contact *contact;
        if (m_client->findContact(m_client->screen(m_data), NULL, false, contact)){
            Event e(EventContactStatus, contact);
            e.process();
        }
        return;
    }
    if (m_state == SSLconnect){
        for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
            SendDirectMsg &sm = *it;
            if ((sm.msg == NULL) || (sm.msg->type() != MessageOpenSecure))
                continue;
            Event e(EventMessageSent, sm.msg);
            e.process();
            delete sm.msg;
            m_queue.erase(it);
            break;
        }
        m_state = Logged;
        Contact *contact;
        if (m_client->findContact(m_client->screen(m_data), NULL, false, contact)){
            Event e(EventContactStatus, contact);
            e.process();
        }
        return;
    }
    if (m_bIncoming){
        Contact *contact;
        m_data = m_client->findContact(m_client->screen(m_data), NULL, false, contact);
        if ((m_data == NULL) || contact->getIgnore()){
            m_socket->error_state("Connection from unknown user");
            return;
        }
        m_state = WaitInit2;
    }else{
        if (m_version >= 7){
            sendInit2();
            m_state = WaitInit2;
        }else{
            m_state = Logged;
            processMsgQueue();
        }
    }
}

void DirectClient::sendInit2()
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack((unsigned short)0x0021);
    m_socket->writeBuffer.pack((char) 0x03);
    m_socket->writeBuffer.pack(0x0000000AL);
    m_socket->writeBuffer.pack(0x00000001L);
    m_socket->writeBuffer.pack(m_bIncoming ? 0x00000001L : 0x00000000L);
    const plugin &p = m_client->plugins[m_channel];
    m_socket->writeBuffer.pack((const char*)p, 8);
    if (m_bIncoming) {
        m_socket->writeBuffer.pack(0x00040001L);
        m_socket->writeBuffer.pack((const char*)p + 8, 8);
    } else {
        m_socket->writeBuffer.pack((const char*)p + 8, 8);
        m_socket->writeBuffer.pack(0x00040001L);
    }
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, name());
    m_socket->write();
}

bool DirectClient::error_state(const QString &error, unsigned code)
{
    QString err = error;
    if (!err.isEmpty() && !DirectSocket::error_state(err, code))
        return false;
    if (m_data && (m_port == m_data->Port.toULong())){
        switch (m_state){
        case ConnectIP1:
        case ConnectIP2:
            m_data->bNoDirect.asBool() = true;
            break;
        default:
            break;
        }
    }
    if (err.isEmpty())
        err = I18N_NOOP("Send message fail");
    for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
        SendDirectMsg &sm = *it;
        if (sm.msg){
            if (!m_client->sendThruServer(sm.msg, m_data)){
                sm.msg->setError(err);
                Event e(EventMessageSent, sm.msg);
                e.process();
                delete sm.msg;
            }
        }else{
            m_client->addPluginInfoRequest(m_data->Uin.toULong(), sm.type);
        }
    }
    m_queue.clear();
    return true;
}

void DirectClient::sendAck(unsigned short seq, unsigned short type, unsigned short flags,
                           const char *msg, unsigned short status, Message *m)
{
    bool bAccept = true;
    if (status == ICQ_TCPxACK_ACCEPT){
        switch (m_client->getStatus()){
        case STATUS_AWAY:
            status = ICQ_TCPxACK_AWAY;
            break;
        case STATUS_OCCUPIED:
            bAccept = false;
            status = ICQ_TCPxACK_OCCUPIED;
            if (type == ICQ_MSGxAR_OCCUPIED){
                status = ICQ_TCPxACK_OCCUPIEDxCAR;
                bAccept = true;
            }
            break;
        case STATUS_NA:
            status = ICQ_TCPxACK_NA;
            break;
        case STATUS_DND:
            status = ICQ_TCPxACK_DND;
            bAccept = false;
            if (type == ICQ_MSGxAR_DND){
                status = ICQ_TCPxACK_DNDxCAR;
                bAccept = true;
            }
            break;
        default:
            break;
        }
    }
    if (!bAccept && (msg == NULL)){
        ar_request req;
        req.screen  = m_client->screen(m_data);
        req.type    = type;
        req.ack		= 0;
        req.flags   = flags;
        req.id.id_l = seq;
        req.id1     = 0;
        req.id2     = 0;
        req.bDirect = true;
        m_client->arRequests.push_back(req);

        unsigned short req_status = STATUS_ONLINE;
        if (m_data->Status.toULong() & ICQ_STATUS_DND){
            req_status = STATUS_DND;
        }else if (m_data->Status.toULong() & ICQ_STATUS_OCCUPIED){
            req_status = STATUS_OCCUPIED;
        }else if (m_data->Status.toULong() & ICQ_STATUS_NA){
            req_status = STATUS_NA;
        }else if (m_data->Status.toULong() & ICQ_STATUS_AWAY){
            req_status = STATUS_AWAY;
        }else if (m_data->Status.toULong() & ICQ_STATUS_FFC){
            req_status = STATUS_FFC;
        }

        Contact *contact = NULL;
        m_client->findContact(m_client->screen(m_data), NULL, false, contact);
        ARRequest ar;
        ar.contact  = contact;
        ar.param    = &m_client->arRequests.back();
        ar.receiver = m_client;
        ar.status   = req_status;
        Event e(EventARRequest, &ar);
        e.process();
        return;
    }

    QCString message;
    if (msg)
        message = msg;

    startPacket(TCP_ACK, seq);
    m_socket->writeBuffer.pack(type);
    m_socket->writeBuffer.pack(status);
    m_socket->writeBuffer.pack(flags);
    m_socket->writeBuffer << message;
    bool bExt = false;
    if (m){
        switch (m->type()){
        case MessageICQFile:
            if (static_cast<ICQFileMessage*>(m)->getExtended()){
                bExt = true;
                Buffer buf, msgBuf;
                Buffer b;
                m_client->packExtendedMessage(m, buf, msgBuf, m_data);
                b.pack((unsigned short)buf.size());
                b.pack(buf.data(0), buf.size());
                b.pack32(msgBuf);
                m_socket->writeBuffer.pack(b.data(), b.size());
            }
            break;
        }
    }
    if (!bExt){
        m_socket->writeBuffer
        << 0x00000000L
        << 0xFFFFFFFFL;
    }
    sendPacket();
}

void DirectClient::startPacket(unsigned short cmd, unsigned short seq)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer
    << (unsigned short)0;	// size
    if (m_version >= 7)
        m_socket->writeBuffer << (char)0x02;
    if (seq == 0)
        seq = --m_nSequence;
    m_socket->writeBuffer
    << (unsigned long)0;			// checkSum
    m_socket->writeBuffer.pack(cmd);
    m_socket->writeBuffer
    << (char) ((m_channel == PLUGIN_NULL) ? 0x0E : 0x12)
    << (char) 0;
    m_socket->writeBuffer.pack(seq);
    m_socket->writeBuffer
    << (unsigned long)0
    << (unsigned long)0
    << (unsigned long)0;
}

void DirectClient::sendPacket()
{
    unsigned size = m_socket->writeBuffer.size() - m_socket->writeBuffer.packetStartPos() - 2;
    unsigned char *p = (unsigned char*)(m_socket->writeBuffer.data(m_socket->writeBuffer.packetStartPos()));
    p[0] = (unsigned char)(size & 0xFF);
    p[1] = (unsigned char)((size >> 8) & 0xFF);

    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, name());

    unsigned long hex, key, B1, M1;
    unsigned long i, check;
    unsigned char X1, X2, X3;

    p += 2;
    if (m_version >= 7){
        size--;
        p++;
    }

    // calculate verification data
    M1 = (rand() % ((size < 255 ? size : 255)-10))+10;
    X1 = (unsigned char)(p[M1] ^ 0xFF);
    X2 = (unsigned char)(rand() % 220);
    X3 = (unsigned char)(client_check_data[X2] ^ 0xFF);

    B1 = (p[4] << 24) | (p[6]<<16) | (p[4]<<8) | (p[6]);

    // calculate checkcode
    check = (M1 << 24) | (X1 << 16) | (X2 << 8) | X3;
    check ^= B1;

    *((unsigned long*)p) = check;
    // main XOR key
    key = 0x67657268 * size + check;

    // XORing the actual data
    for(i=4; i<(size+3)/4; i+=4){
        hex = key + client_check_data[i & 0xFF];
        p[i] ^= hex & 0xFF;
        p[i+1] ^= (hex>>8) & 0xFF;
        p[i+2] ^= (hex>>16) & 0xFF;
        p[i+3] ^= (hex>>24) & 0xFF;
    }
    m_socket->write();
}

void DirectClient::acceptMessage(Message *msg)
{
    unsigned short seq = 0;
    switch (msg->type()){
    case MessageICQFile:
        seq = (unsigned short)(static_cast<ICQFileMessage*>(msg)->getID_L());
        sendAck(seq, static_cast<ICQFileMessage*>(msg)->getExtended() ? ICQ_MSGxEXT : ICQ_MSGxFILE, 0, NULL, ICQ_TCPxACK_ACCEPT, msg);
        break;
    default:
        log(L_WARN, "Unknown type for direct decline");
    }
}

void DirectClient::declineMessage(Message *msg, const QString &reason)
{
    QCString r;
    r = getContacts()->fromUnicode(m_client->getContact(m_data), reason);
    unsigned short seq = 0;
    switch (msg->type()){
    case MessageICQFile:
        seq = (unsigned short)(static_cast<ICQFileMessage*>(msg)->getID_L());
        sendAck(seq, static_cast<ICQFileMessage*>(msg)->getExtended() ? ICQ_MSGxEXT : ICQ_MSGxFILE, 0, r, ICQ_TCPxACK_REFUSE, msg);
        break;
    default:
        log(L_WARN, "Unknown type for direct decline");
    }
}

bool DirectClient::sendMessage(Message *msg)
{
    SendDirectMsg sm;
    sm.msg	= msg;
    sm.seq	= 0;
    sm.type	= 0;
    m_queue.push_back(sm);
    processMsgQueue();
    return true;
}

void packCap(Buffer &b, const capability &c);

void DirectClient::processMsgQueue()
{
    if (m_state != Logged)
        return;
    for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end();){
        SendDirectMsg &sm = *it;
        if (sm.seq){
            ++it;
            continue;
        }
        if (sm.msg){
            QCString message;
            Buffer &mb = m_socket->writeBuffer;
            unsigned short flags = ICQ_TCPxMSG_NORMAL;
            if (sm.msg->getFlags() & MESSAGE_URGENT)
                flags = ICQ_TCPxMSG_URGENT;
            if (sm.msg->getFlags() & MESSAGE_LIST)
                flags = ICQ_TCPxMSG_LIST;
            switch (sm.msg->type()){
            case MessageGeneric:
                startPacket(TCP_START, 0);
                mb.pack((unsigned short)ICQ_MSGxMSG);
                mb.pack(m_client->msgStatus());
                mb.pack(flags);
                if ((sm.msg->getFlags() & MESSAGE_RICHTEXT) &&
                        (m_client->getSendFormat() == 0) &&
                        (m_client->hasCap(m_data, CAP_RTF))){
                    QString text = sm.msg->getRichText();
                    QString part;
                    message = m_client->createRTF(text, part, sm.msg->getForeground(), m_client->getContact(m_data), 0xFFFFFFFF);
                    sm.type = CAP_RTF;
                }else if (m_client->hasCap(m_data, CAP_UTF) &&
                          (m_client->getSendFormat() <= 1) &&
                          ((sm.msg->getFlags() & MESSAGE_SECURE) == 0)){
                    message = ICQClient::addCRLF(sm.msg->getPlainText()).utf8();
                    sm.type = CAP_UTF;
                }else{
                    message = getContacts()->fromUnicode(m_client->getContact(m_data), sm.msg->getPlainText());
                    messageSend ms;
                    ms.msg  = sm.msg;
                    ms.text = sm.msg->getPlainText();
                    Event e(EventSend, &ms);
                    e.process();
                }
                mb << message;
                if (sm.msg->getBackground() == sm.msg->getForeground()){
                    mb << 0x00000000L << 0xFFFFFF00L;
                }else{
                    mb << (sm.msg->getForeground() << 8) << (sm.msg->getBackground() << 8);
                }
                if (sm.type){
                    mb << 0x26000000L;
                    packCap(mb, ICQClient::capabilities[sm.type]);
                }
                sendPacket();
                sm.seq = m_nSequence;
                sm.icq_type = ICQ_MSGxMSG;
                break;
            case MessageFile:
            case MessageUrl:
            case MessageContacts:
            case MessageOpenSecure:
            case MessageCloseSecure:
                startPacket(TCP_START, 0);
                m_client->packMessage(mb, sm.msg, m_data, sm.icq_type, true);
                sendPacket();
                sm.seq = m_nSequence;
                break;
            default:
                sm.msg->setError(I18N_NOOP("Unknown message type"));
                Event e(EventMessageSent, sm.msg);
                e.process();
                delete sm.msg;
                m_queue.erase(it);
                it = m_queue.begin();
                continue;
            }
        }else{
            if (sm.type == PLUGIN_AR){
                sm.icq_type = 0;
                unsigned s = m_data->Status.toULong();
                if (s != ICQ_STATUS_OFFLINE){
                    if (s & ICQ_STATUS_DND){
                        sm.icq_type = ICQ_MSGxAR_DND;
                    }else if (s & ICQ_STATUS_OCCUPIED){
                        sm.icq_type = ICQ_MSGxAR_OCCUPIED;
                    }else if (s & ICQ_STATUS_NA){
                        sm.icq_type = ICQ_MSGxAR_NA;
                    }else if (s & ICQ_STATUS_AWAY){
                        sm.icq_type = ICQ_MSGxAR_AWAY;
                    }else if (s & ICQ_STATUS_FFC){
                        sm.icq_type = ICQ_MSGxAR_FFC;
                    }
                }
                if (sm.type == 0){
                    m_queue.erase(it);
                    it = m_queue.begin();
                    continue;
                }
                Buffer &mb = m_socket->writeBuffer;
                startPacket(TCP_START, 0);
                mb.pack(sm.icq_type);
                mb.pack(m_client->msgStatus());
                mb.pack(ICQ_TCPxMSG_AUTOxREPLY);
                mb << (char)1 << (unsigned short)0;
                sendPacket();
                sm.seq = m_nSequence;
            }else{
                Buffer &mb = m_socket->writeBuffer;
                startPacket(TCP_START, 0);
                mb.pack((unsigned short)ICQ_MSGxMSG);
                mb.pack(m_client->msgStatus());
                mb.pack(ICQ_TCPxMSG_AUTOxREPLY);
                mb.pack((unsigned short)1);
                mb.pack((char)0);
                mb.pack((char*)m_client->plugins[sm.type], sizeof(plugin));
                mb.pack((unsigned long)0);
                sendPacket();
                sm.seq = m_nSequence;
            }
        }
        ++it;
    }
}

bool DirectClient::cancelMessage(Message *msg)
{
    for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
        if ((*it).msg == msg){
            if ((*it).seq){
                Buffer &mb = m_socket->writeBuffer;
                startPacket(TCP_CANCEL, (*it).seq);
                mb.pack((unsigned short)(*it).icq_type);
                mb.pack((unsigned short)0);
                mb.pack((unsigned short)0);
                QCString message;
                mb << message;
                sendPacket();
            }
            m_queue.erase(it);
            return true;
        }
    }
    return false;
}

void DirectClient::addPluginInfoRequest(unsigned plugin_index)
{
    for (list<SendDirectMsg>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
        SendDirectMsg &sm = *it;
        if (sm.msg)
            continue;
        if (sm.type == plugin_index)
            return;
    }
    SendDirectMsg sm;
    sm.msg = NULL;
    sm.seq = 0;
    sm.type = plugin_index;
    sm.icq_type = 0;
    m_queue.push_back(sm);
    processMsgQueue();
}

#ifdef USE_OPENSSL

class ICQ_SSLClient : public SSLClient
{
public:
ICQ_SSLClient(Socket *s) : SSLClient(s) {}
    virtual bool initSSL() { return initTLS1(true); }
};


void DirectClient::secureConnect()
{
    if (m_ssl != NULL) return;
    m_ssl = new ICQ_SSLClient(m_socket->socket());
    if (!m_ssl->init()){
        delete m_ssl;
        m_ssl = NULL;
        return;
    }
    m_socket->setSocket(m_ssl);
    m_state = SSLconnect;
    m_ssl->connect();
    m_ssl->process();
}

void DirectClient::secureListen()
{
    if (m_ssl != NULL)
        return;
    m_ssl = new ICQ_SSLClient(m_socket->socket());
    if (!m_ssl->init()){
        delete m_ssl;
        m_ssl = NULL;
        return;
    }
    m_socket->setSocket(m_ssl);
    m_state = SSLconnect;
    m_ssl->accept();
    m_ssl->process();
}

void DirectClient::secureStop(bool bShutdown)
{
    if (m_ssl){
        if (bShutdown){
            m_ssl->shutdown();
            m_ssl->process();
        }
        m_socket->setSocket(m_ssl->socket(), false);
        m_ssl->setSocket(NULL);
        delete m_ssl;
        m_ssl = NULL;
        Contact *contact;
        if (m_client->findContact(m_client->screen(m_data), NULL, false, contact)){
            Event e(EventContactStatus, contact);
            e.process();
        }
    }
}
#endif

QString DirectClient::name()
{
    if (m_data == NULL)
        return QString();
    m_name = "";
    switch (m_channel){
    case PLUGIN_NULL:
        break;
    case PLUGIN_INFOxMANAGER:
        m_name = "Info.";
        break;
    case PLUGIN_STATUSxMANAGER:
        m_name = "Status.";
        break;
    default:
        m_name = "Unknown.";
    }
    m_name += QString::number(m_data->Uin.toULong());
    m_name += ".";
    m_name += QString::number((unsigned long)this);
    return m_name;
}

ICQFileTransfer::ICQFileTransfer(FileMessage *msg, ICQUserData *data, ICQClient *client)
        : FileTransfer(msg), DirectSocket(data, client)
{
    m_state = None;
    FileMessage::Iterator it(*msg);
    m_nFiles     = it.count();
    m_totalSize = msg->getSize();
}

ICQFileTransfer::~ICQFileTransfer()
{
}

void ICQFileTransfer::connect(unsigned short port)
{
    m_port = port;
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
    DirectSocket::connect();
}

void ICQFileTransfer::listen()
{
    FileTransfer::m_state = FileTransfer::Listen;
    if (m_notify)
        m_notify->process();
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void ICQFileTransfer::processPacket()
{
    char cmd;
    m_socket->readBuffer >> cmd;
    if (cmd != FT_DATA){
        ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
        log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, "File transfer");
    }
    if (cmd == FT_SPEED){
        char speed;
        m_socket->readBuffer.unpack(speed);
        m_speed = speed;
        return;
    }
    switch (m_state){
    case InitSend:
        switch (cmd){
        case FT_INIT_ACK:
            sendFileInfo();
            break;
        case FT_START:{
                unsigned long pos, empty, speed, curFile;
                m_socket->readBuffer.unpack(pos);
                m_socket->readBuffer.unpack(empty);
                m_socket->readBuffer.unpack(speed);
                m_socket->readBuffer.unpack(curFile);
                curFile--;
                log(L_DEBUG, "Start send at %lu %lu", pos, curFile);
                FileMessage::Iterator it(*m_msg);
                if (curFile >= it.count()){
                    m_socket->error_state("Bad file index");
                    return;
                }
                while (curFile != m_nFile){
                    if (!openFile()){
                        m_socket->error_state("Can't open file");
                        return;
                    }
                }
                if (m_file && !m_file->at(pos)){
                    m_socket->error_state("Can't set transfer position");
                    return;
                }
                m_totalBytes += pos;
                m_bytes		  = pos;
                m_state       = Send;
                FileTransfer::m_state = FileTransfer::Write;
                if (m_notify){
                    m_notify->process();
                    m_notify->transfer(true);
                }
                write_ready();
                break;
            }
        default:
            log(L_WARN, "Bad init client command %X", cmd);
            m_socket->error_state("Bad packet");
        }
        break;
    case WaitInit:{
            if (cmd != FT_INIT){
                m_socket->error_state("No init command");
                return;
            }
            unsigned long n;
            m_socket->readBuffer.unpack(n);
            m_socket->readBuffer.unpack(n);
            m_nFiles = n;
            m_socket->readBuffer.unpack(n);
            m_totalSize = n;
            m_msg->setSize(m_totalSize);
            m_state = InitReceive;
            setSpeed(m_speed);
            startPacket(FT_INIT_ACK);
            m_socket->writeBuffer.pack((unsigned long)m_speed);
            QString uin = m_client->screen(&m_client->data.owner);
            m_socket->writeBuffer << uin;
            sendPacket();
            FileTransfer::m_state = Negotiation;
            if (m_notify)
                m_notify->process();
        }
        break;
    case InitReceive:{
            initReceive(cmd);
            break;
        }
    case Receive:{
            if (m_bytes < m_fileSize){
                if (cmd != FT_DATA){
                    m_socket->error_state("Bad data command");
                    return;
                }
                unsigned short size = (unsigned short)(m_socket->readBuffer.size() - m_socket->readBuffer.readPos());
                m_bytes      += size;
                m_totalBytes += size;
                m_transferBytes += size;
                if (size){
                    if (m_file == NULL){
                        m_socket->error_state("Write without file");
                        return;
                    }
                    if (m_file->writeBlock(m_socket->readBuffer.data(m_socket->readBuffer.readPos()), size) != size){
                        m_socket->error_state("Error write file");
                        return;
                    }
                }
            }
            if (m_bytes >= m_fileSize){
                if (m_nFile + 1 >= m_nFiles){
                    log(L_DEBUG, "File transfer OK");
                    FileTransfer::m_state = FileTransfer::Done;
                    if (m_notify)
                        m_notify->process();
                    m_socket->error_state("");
                    return;
                }
                m_state = InitReceive;
            }
            if (m_notify)
                m_notify->process();
            if (cmd != FT_DATA)
                initReceive(cmd);
            break;
        }

    default:
        log(L_WARN, "Bad state in process packet %u", m_state);
    }
}

void ICQFileTransfer::initReceive(char cmd)
{
    if (cmd != FT_FILEINFO){
        m_socket->error_state("Bad command in init receive");
        return;
    }
    QCString fileName;
    char isDir;
    m_socket->readBuffer >> isDir >> fileName;
    QString fName = getContacts()->toUnicode(m_client->getContact(m_data), fileName);
    QCString dir;
    unsigned long n;
    m_socket->readBuffer >> dir;
    m_socket->readBuffer.unpack(n);
    if (m_notify)
        m_notify->transfer(false);
    if (!dir.isEmpty())
        fName = getContacts()->toUnicode(m_client->getContact(m_data), dir) + "/" + fName;
    if (isDir)
        fName += "/";
    m_state = Wait;
    FileTransfer::m_state = FileTransfer::Read;
    if (m_notify)
        m_notify->createFile(fName, n, true);
}

bool ICQFileTransfer::error(const char *err)
{
    return error_state(err, 0);
}

bool ICQFileTransfer::accept(Socket *s, unsigned long)
{
    log(L_DEBUG, "Accept file transfer");
    if (m_state == WaitReverse){
        acceptReverse(s);
    }else{
        m_socket->setSocket(s);
        m_bIncoming = true;
        DirectSocket::m_state = DirectSocket::WaitInit;
        init();
    }
    return true;
}

void ICQFileTransfer::bind_ready(unsigned short port)
{
    m_localPort = port;
    if (m_state == WaitReverse){
        m_client->requestReverseConnection(m_client->screen(m_data), this);
        return;
    }
    m_state = Listen;
    static_cast<ICQFileMessage*>(m_msg)->setPort(port);
    m_client->accept(m_msg, m_data);
}

void ICQFileTransfer::login_timeout()
{
    if (ICQClient::hasCap(m_data, CAP_DIRECT)){
        DirectSocket::m_state = DirectSocket::WaitReverse;
        m_state = WaitReverse;
        bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
        return;
    }
    DirectSocket::login_timeout();
}

bool ICQFileTransfer::error_state(const QString &err, unsigned code)
{
    if (DirectSocket::m_state == DirectSocket::ConnectFail){
        if (ICQClient::hasCap(m_data, CAP_DIRECT)){
            login_timeout();
            return false;
        }
    }
    if (!DirectSocket::error_state(err, code))
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

void ICQFileTransfer::connect_ready()
{
    if (m_state == None){
        m_state = WaitLogin;
        DirectSocket::connect_ready();
        return;
    }
    if (m_state == WaitReverse){
        m_bIncoming = false;
        m_state = WaitReverseLogin;
        DirectSocket::connect_ready();
        return;
    }
    if (m_state == WaitReverseLogin)
        m_bIncoming = true;
    m_file = 0;
    FileTransfer::m_state = FileTransfer::Negotiation;
    if (m_notify)
        m_notify->process();
    if (m_bIncoming){
        m_state = WaitInit;
    }else{
        m_state = InitSend;
        startPacket(FT_SPEED);
        m_socket->writeBuffer.pack((unsigned long)m_speed);
        sendPacket(true);
        sendInit();
    }
}

void ICQFileTransfer::sendInit()
{
    startPacket(FT_INIT);
    m_socket->writeBuffer.pack((unsigned long)0);
    m_socket->writeBuffer.pack((unsigned long)m_nFiles);			// nFiles
    m_socket->writeBuffer.pack((unsigned long)m_totalSize);		// Total size
    m_socket->writeBuffer.pack((unsigned long)m_speed);			// speed
    m_socket->writeBuffer << QString::number(m_client->data.owner.Uin.toULong());
    sendPacket();
    if ((m_nFiles == 0) || (m_totalSize == 0))
        m_socket->error_state(I18N_NOOP("No files for transfer"));
}

void ICQFileTransfer::startPacket(char cmd)
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer << (unsigned short)0;
    m_socket->writeBuffer << cmd;
}

void ICQFileTransfer::sendPacket(bool dump)
{
    unsigned long start_pos = m_socket->writeBuffer.packetStartPos();
    unsigned size = m_socket->writeBuffer.size() - start_pos - 2;
    unsigned char *p = (unsigned char*)(m_socket->writeBuffer.data(start_pos));
    p[0] = (unsigned char)(size & 0xFF);
    p[1] = (unsigned char)((size >> 8) & 0xFF);
    if (dump){
        ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
        QString name = "FileTranfer";
        if (m_data){
            name += ".";
            name += QString::number(m_data->Uin.toULong());
        }
        log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, name.latin1());
    }
    m_socket->write();
}

void ICQFileTransfer::setSpeed(unsigned speed)
{
    FileTransfer::setSpeed(speed);
    switch (m_state){
    case InitSend:
    case InitReceive:
    case Send:
    case Receive:
    case Wait:
        startPacket(FT_SPEED);
        m_socket->writeBuffer.pack((unsigned long)m_speed);
        sendPacket(true);
        break;
    default:
        break;
    }
}

void ICQFileTransfer::write_ready()
{
    if (m_state != Send){
        DirectSocket::write_ready();
        return;
    }
    if (m_transfer){
        m_transferBytes += m_transfer;
        m_transfer = 0;
        if (m_notify)
            m_notify->process();
    }
    if (m_bytes >= m_fileSize){
        m_state = None;
        m_state = InitSend;
        sendFileInfo();
        if (m_notify)
            m_notify->process();
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
    unsigned long tail = m_fileSize - m_bytes;
    if (tail > 2048) tail = 2048;
    startPacket(FT_DATA);
    char buf[2048];
    int readn = m_file->readBlock(buf, tail);
    if (readn <= 0){
        m_socket->error_state("Read file error");
        return;
    }
    m_transfer   = readn;
    m_bytes      += readn;
    m_totalBytes += readn;
    m_sendSize   += readn;
    m_socket->writeBuffer.pack(buf, readn);
    sendPacket(false);
}

void ICQFileTransfer::sendFileInfo()
{
    if (!openFile()){
        if (FileTransfer::m_state == FileTransfer::Done)
            m_socket->error_state("");
        if (m_notify)
            m_notify->transfer(false);
        return;
    }
    if (m_notify)
        m_notify->transfer(false);
    startPacket(FT_FILEINFO);
    m_socket->writeBuffer.pack((char)(isDirectory() ? 1 : 0));
    QString fn  = filename();
    QString dir;
    int n = fn.findRev("/");
    if (n >= 0){
        dir = fn.left(n);
        dir = dir.replace(QRegExp("/"), "\\");
        fn  = fn.mid(n);
    }
    QCString s1 = getContacts()->fromUnicode(m_client->getContact(m_data), fn);
    QCString s2;
    if (!dir.isEmpty())
        s2 = getContacts()->fromUnicode(m_client->getContact(m_data), dir);
    m_socket->writeBuffer << s1.data() << s2.data();
    m_socket->writeBuffer.pack((unsigned long)m_fileSize);
    m_socket->writeBuffer.pack((unsigned long)0);
    m_socket->writeBuffer.pack((unsigned long)m_speed);
    sendPacket();
    if (m_notify)
        m_notify->process();
}

void ICQFileTransfer::setSocket(ClientSocket *socket)
{
    if (m_socket)
        delete m_socket;
    m_socket = socket;
    m_socket->setNotify(this);
    m_state  = WaitInit;
    processPacket();
    if ((m_msg->getFlags() & MESSAGE_RECEIVED) == 0){
        m_state = InitSend;
        sendInit();
    }
    m_socket->readBuffer.init(2);
    m_socket->readBuffer.packetStart();
    m_bHeader = true;
    DirectSocket::m_state = DirectSocket::Logged;
}

void ICQFileTransfer::startReceive(unsigned pos)
{
    if (m_state != Wait){
        log(L_WARN, "Start receive in bad state");
        return;
    }
    startPacket(FT_START);
    if (pos > m_fileSize)
        pos = m_fileSize;
    m_bytes = pos;
    m_totalBytes += pos;
    m_socket->writeBuffer.pack((unsigned long)pos);
    m_socket->writeBuffer.pack((unsigned long)0);
    m_socket->writeBuffer.pack((unsigned long)m_speed);
    m_socket->writeBuffer.pack((unsigned long)(m_nFile + 1));
    sendPacket();
    m_state = Receive;
    if (m_notify)
        m_notify->transfer(true);
}

AIMFileTransfer::AIMFileTransfer(FileMessage *msg, ICQUserData *data, ICQClient *client)
        : FileTransfer(msg), DirectSocket(data, client)
{
    m_msg		= msg;
    m_client	= client;
    m_state		= None;
}

AIMFileTransfer::~AIMFileTransfer()
{
}

void AIMFileTransfer::listen()
{
    m_state = Listen;
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void AIMFileTransfer::accept()
{
    m_state = Accept;
    bind(m_client->getMinPort(), m_client->getMaxPort(), m_client);
}

void AIMFileTransfer::connect(unsigned short port)
{
    m_port  = port;
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
    DirectSocket::connect();
}

void AIMFileTransfer::processPacket()
{
}

void AIMFileTransfer::packet_ready()
{
    if (m_socket->readBuffer.readPos() <= m_socket->readBuffer.writePos())
        return;
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->AIMDirectPacket, m_client->screen(m_data));
    m_socket->readBuffer.init(0);
}

void AIMFileTransfer::connect_ready()
{
    log(L_DEBUG, "Connect ready");
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
}

bool AIMFileTransfer::error_state(const QString &err, unsigned)
{
    m_msg->setError(err);
    Event e(EventMessageSent, m_msg);
    e.process();
    return true;
}

void AIMFileTransfer::write_ready()
{
}

void AIMFileTransfer::startReceive(unsigned)
{
}

void AIMFileTransfer::bind_ready(unsigned short port)
{
    for (list<Message*>::iterator it = m_client->m_processMsg.begin(); it != m_client->m_processMsg.end(); ++it){
        if ((*it) == m_msg){
            m_client->m_processMsg.erase(it);
            break;
        }
    }
    m_port = port;
    SendMsg s;
    s.flags  = (m_state == Listen) ? PLUGIN_AIM_FT : PLUGIN_AIM_FT_ACK;
    s.socket = this;
    s.screen = m_client->screen(m_data);
    s.msg	 = m_msg;
    m_client->sendFgQueue.push_front(s);
    m_client->processSendQueue();
}

bool AIMFileTransfer::accept(Socket *s, unsigned long)
{
    log(L_DEBUG, "Accept AIM file transfer");
    m_socket->setSocket(s);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    m_socket->setRaw(true);
    FileTransfer::m_state = FileTransfer::Negotiation;
    if (m_notify)
        m_notify->process();
    return true;
}

bool AIMFileTransfer::error(const char *err)
{
    error_state(err, 0);
    return true;
}

