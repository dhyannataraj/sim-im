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

#ifdef USE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#endif

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

#include <qfile.h>
#include <qtimer.h>

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
    m_socket = getSocketFactory()->createServerSocket();
    m_socket->setNotify(this);
    m_client = client;
}

ICQListener::~ICQListener()
{
    if (m_socket)
        delete m_socket;
}

void ICQListener::accept(Socket *s, unsigned long ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    log(L_DEBUG, "Accept direct connection %s", inet_ntoa(addr));
    m_client->m_sockets.push_back(new DirectClient(s, m_client));
}

unsigned short ICQListener::port()
{
    if (m_socket)
        return m_socket->port();
    return 0;
}

// ___________________________________________________________________________________________

DirectSocket::DirectSocket(Socket *s, ICQClient *client)
{
    m_socket = new ClientSocket(this);
    m_socket->setSocket(s);
    m_bIncoming = true;
    m_client = client;
    m_state = WaitInit;
    m_version = 0;
    m_data	= NULL;
    m_port  = 0;
    init();
}

DirectSocket::DirectSocket(ICQUserData *data, ICQClient *client)
{
    m_socket    = new ClientSocket(this);
    m_bIncoming = false;
    m_version   = data->Version;
    m_client    = client;
    m_state     = NotConnected;
    m_data		= data;
    m_port		= 0;
    init();
}

DirectSocket::~DirectSocket()
{
    if (m_socket)
        delete m_socket;
    //    if (m_listener)
    //        delete m_listener;
    removeFromClient();
}

void DirectSocket::timeout()
{
    if ((m_state != Logged) && m_socket)
        m_socket->error_state("Timeout direct connection");
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
    //    m_listener = NULL;
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
        return m_data->Uin;
    return 0;
}

unsigned short DirectSocket::localPort()
{
    //	if (m_listener)
    //		return m_listener->port();
    return m_client->data.owner.Port;
}

unsigned short DirectSocket::remotePort()
{
    return m_port;
}

bool DirectSocket::error_state(const char *error, unsigned)
{
    if ((m_state == ConnectIP1) || (m_state == ConnectIP2)){
        connect();
        return false;
    }
    log(L_WARN, "Direct socket error %s", error);
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
        m_state = WaitReverse;
        /*
        		if (m_data->Caps & (1 << CAP_DIRECT)){
        //			m_listener = new DirectListener(this);
        //			if (m_listener->created()){
        				m_client->requestReverseConnection(m_data->Uin, this);
        				return;
        //			}
        //			delete m_listener;
        //			m_listener = NULL;
        		}
        */
    }
    if (m_state == WaitReverse){
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
    m_state    = WaitInit;
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
        log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, number((unsigned)this).c_str());
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
            if ((s1 != 1) || (s2 != 0)){
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
            if (my_uin != m_client->data.owner.Uin){
                m_socket->error_state("Bad owner UIN");
                return;
            }
            m_socket->readBuffer.incReadPos(6);
            unsigned long p_uin;
            m_socket->readBuffer.unpack(p_uin);
            if (m_bIncoming){
                Contact *contact;
                m_data = m_client->findContact(number(p_uin).c_str(), NULL, false, contact);
                if ((m_data == NULL) || contact->getIgnore()){
                    m_socket->error_state("User not found");
                    return;
                }
                if ((m_client->getInvisible() && (m_data->VisibleId == 0)) ||
                        (!m_client->getInvisible() && m_data->InvisibleId)){
                    m_socket->error_state("User not found");
                    return;
                }
            }else{
                if (p_uin != m_data->Uin){
                    m_socket->error_state("Bad sender UIN");
                    return;
                }
            }
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
    if (!m_bIncoming){
        if (m_data->DCcookie == 0){
            m_socket->error_state("No direct info");
            return;
        }
        m_nSessionId = m_data->DCcookie;
    }

    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack((unsigned short)((m_version >= 7) ? 0x0030 : 0x002c));
    m_socket->writeBuffer.pack((char)0xFF);
    m_socket->writeBuffer.pack((unsigned short)m_version);
    m_socket->writeBuffer.pack((unsigned short)((m_version >= 7) ? 0x002b : 0x0027));
    m_socket->writeBuffer.pack(m_data->Uin);
    m_socket->writeBuffer.pack((unsigned short)0x0000);
    m_socket->writeBuffer.pack((unsigned long)m_data->Port);
    m_socket->writeBuffer.pack(m_client->data.owner.Uin);
    m_socket->writeBuffer.pack(get_ip(m_client->data.owner.IP));
    m_socket->writeBuffer.pack(get_ip(m_client->data.owner.RealIP));
    m_socket->writeBuffer.pack((char)0x01);
    m_socket->writeBuffer.pack(0x00000000L);
    m_socket->writeBuffer.pack(m_nSessionId);
    m_socket->writeBuffer.pack(0x00000050L);
    m_socket->writeBuffer.pack(0x00000003L);
    if (m_version >= 7)
        m_socket->writeBuffer.pack(0x00000000L);
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, number((unsigned)this).c_str());
    m_socket->write();
}

void DirectSocket::sendInitAck()
{
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack((unsigned short)0x0004);
    m_socket->writeBuffer.pack((unsigned short)0x0001);
    m_socket->writeBuffer.pack((unsigned short)0x0000);
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, number((unsigned)this).c_str());
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
        m_socket->readBuffer.init(2);
        m_socket->readBuffer.packetStart();
        m_bHeader = true;
    }
}

/*
DirectListener::DirectListener(DirectSocket *dsock)
{
    m_socket = getSocketFactory()->createServerSocket();
    m_socket->setNotify(this);
    m_dsock  = dsock;
}

DirectListener::~DirectListener()
{
    if (m_socket)
        delete m_socket;
}

void DirectListener::accept(Socket *s, unsigned long ip)
{
    struct in_addr addr;
    addr.s_addr = ip;
    log(L_DEBUG, "Accept reverse connection %s", inet_ntoa(addr));
	m_dsock->acceptReverse(s);
}

unsigned short DirectListener::port()
{
    if (m_socket)
        return m_socket->port();
    return 0;
}
*/

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

DirectClient::DirectClient(Socket *s, ICQClient *client)
        : DirectSocket(s, client)
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
    m_port    = data->Port;
#ifdef USE_OPENSSL
    m_ssl = NULL;
#endif
}

DirectClient::~DirectClient()
{
    error_state(NULL, 0);
    switch (m_channel){
    case PLUGIN_NULL:
        if (m_data && (m_data->Direct == this))
            m_data->Direct = NULL;
        break;
    case PLUGIN_INFOxMANAGER:
        if (m_data && (m_data->DirectPluginInfo == this))
            m_data->DirectPluginInfo = NULL;
        break;
    case PLUGIN_STATUSxMANAGER:
        if (m_data && (m_data->DirectPluginStatus == this))
            m_data->DirectPluginStatus = NULL;
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
            bool bFileTransfer = false;
            unsigned pos = m_socket->readBuffer.readPos();
            unsigned packetSize = m_socket->readBuffer.size() - m_socket->readBuffer.packetStartPos();
            if (packetSize == 7){
                char cmd;
                m_socket->readBuffer >> cmd;
                if (cmd == FT_SPEED)
                    bFileTransfer = true;
            }
            if (packetSize == 23){
                char cmd;
                m_socket->readBuffer >> cmd;
                if (cmd == FT_INIT)
                    bFileTransfer = true;
            }
            m_socket->readBuffer.setReadPos(pos);
            if (bFileTransfer){
                ICQFileTransfer *transfer = NULL;
                for (list<Message*>::iterator it = m_client->m_processMsg.begin(); it != m_client->m_processMsg.end(); ++it){
                    if ((*it)->type() != MessageFile)
                        continue;
                    FileMessage *msg = static_cast<FileMessage*>(*it);
                    transfer = static_cast<ICQFileTransfer*>(msg->m_transfer);
                    if (transfer->Uin() == m_data->Uin)
                        break;
                    transfer = NULL;
                }
                if (transfer){
                    transfer->setSocket(m_socket);
                    m_socket = NULL;
                    return;
                }
                m_socket->error_state("File transfer not found");
                return;
            }
            if (m_version < 8){
                if (m_data->Direct){
                    m_socket->error_state("Direct connection already established");
                    return;
                }
                m_state = Logged;
                processMsgQueue();
                break;
            }
            ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
            log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, number((unsigned)this).c_str());
            m_socket->readBuffer.incReadPos(13);
            char p[16];
            m_socket->readBuffer.unpack(p, 16);
            for (m_channel = 0; m_channel <= PLUGIN_NULL; m_channel++){
                if (!memcmp(m_client->plugins[m_channel], p, 16))
                    break;
            }
            removeFromClient();
            switch (m_channel){
            case PLUGIN_INFOxMANAGER:
                if (m_data->DirectPluginInfo){
                    if (m_data->DirectPluginInfo->copyQueue(this)){
                        delete m_data->DirectPluginInfo;
                        m_data->DirectPluginInfo = this;
                    }else{
                        m_socket->error_state("Plugin info connection already established");
                    }
                }else{
                    m_data->DirectPluginInfo = this;
                }
                break;
            case PLUGIN_STATUSxMANAGER:
                if (m_data->DirectPluginStatus){
                    if (m_data->DirectPluginStatus->copyQueue(this)){
                        delete m_data->DirectPluginStatus;
                        m_data->DirectPluginStatus = this;
                    }else{
                        m_socket->error_state("Plugin status connection already established");
                    }
                }else{
                    m_data->DirectPluginStatus = this;
                }
                break;
            case PLUGIN_NULL:
                if (m_data->Direct){
                    if (m_data->Direct->copyQueue(this)){
                        delete m_data->Direct;
                        m_data->Direct = this;
                    }else{
                        m_socket->error_state("Direct connection already established");
                    }
                }else{
                    m_data->Direct = this;
                }
                break;
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

    X1 = p[M1] ^ 0xFF;
    if(((B1 >> 16) & 0xFF) != X1){
        m_socket->error_state("Decrypt packet failed");
        return;
    }

    X2 = ((B1 >> 8) & 0xFF);
    if(X2 < 220) {
        X3 = client_check_data[X2] ^ 0xFF;
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
    string msg_str;
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
                m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact);
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
            sendAck(seq, type, msgFlags, msg_str.c_str());
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
            m = m_client->parseMessage(type, m_client->screen(m_data).c_str(), msg_str, m_socket->readBuffer);
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
                sendAck(seq, type, msgFlags);
                if (msgFlags & ICQ_TCPxMSG_URGENT)
                    m->setFlags(m->getFlags() | MESSAGE_URGENT);
                if (msgFlags & ICQ_TCPxMSG_LIST)
                    m->setFlags(m->getFlags() | MESSAGE_LIST);
                m_client->messageReceived(m, m_client->screen(m_data).c_str());
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
                m_client->pluginAnswer(plugin_index, m_data->Uin, info);
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
    case TCP_ACK:
        log(L_DEBUG, "Ack %X %X", ackFlags, msgFlags);
        for (it = m_queue.begin(); it != m_queue.end(); ++it){
            if ((*it).seq != seq)
                continue;

            if ((*it).msg == NULL){
                if ((*it).type == PLUGIN_AR)
                    set_str(&m_data->AutoReply, msg_str.c_str());
                unsigned plugin_index = (*it).type;
                m_queue.erase(it);
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
                m_client->parsePluginPacket(m_socket->readBuffer, plugin_index, m_data, m_data->Uin, true);
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
            Message *m = m_client->parseMessage(type, m_client->screen(m_data).c_str(), msg_str, m_socket->readBuffer);
            switch (msg->type()){
#ifdef USE_OPENSSL
            case MessageCloseSecure:
                secureStop(true);
                break;
            case MessageOpenSecure:
                if (*msg_str.c_str() == 0){
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
                    if (msg_str.empty()){
                        msg->setError(I18N_NOOP("Send message fail"));
                    }else{
                        QString err = m_client->toUnicode(msg_str.c_str(), m_data);
                        msg->setError(err.utf8());
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
            if (m_client->ackMessage(msg, ackFlags, msg_str.c_str())){
                if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
                    if (msg->type() == MessageGeneric){
                        Message m;
                        m.setContact(msg->contact());
                        m.setClient(msg->client());
                        if ((*it).type == CAP_RTF){
                            m.setText(m_client->removeImages(msg->getRichText(), 16));
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
                    }else{
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
        if (it == m_queue.end())
            log(L_WARN, "Message for ACK not found");
        break;
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
        if (m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact)){
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
        if (m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact)){
            Event e(EventContactStatus, contact);
            e.process();
        }
        return;
    }
    if (m_bIncoming){
        Contact *contact;
        m_data = m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact);
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

bool DirectClient::error_state(const char *err, unsigned code)
{
    if (err && !DirectSocket::error_state(err, code))
        return false;
    if (m_data && (m_port == m_data->Port)){
        switch (m_state){
        case ConnectIP1:
        case ConnectIP2:
            m_data->bNoDirect = true;
            break;
        default:
            break;
        }
    }
    if (err == NULL)
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
            m_client->addPluginInfoRequest(m_data->Uin, sm.type);
        }
    }
    m_queue.clear();
    return true;
}

void DirectClient::sendAck(unsigned short seq, unsigned short type, unsigned short flags, const char *msg)
{
    bool bAccept = true;
    unsigned short status = ICQ_TCPxACK_ACCEPT;
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
        if (m_data->Status & ICQ_STATUS_DND){
            req_status = STATUS_DND;
        }else if (m_data->Status & ICQ_STATUS_OCCUPIED){
            req_status = STATUS_OCCUPIED;
        }else if (m_data->Status & ICQ_STATUS_NA){
            req_status = STATUS_NA;
        }else if (m_data->Status & ICQ_STATUS_AWAY){
            req_status = STATUS_AWAY;
        }else if (m_data->Status & ICQ_STATUS_FFC){
            req_status = STATUS_FFC;
        }

        Contact *contact = NULL;
        m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact);
        ARRequest ar;
        ar.contact  = contact;
        ar.param    = &m_client->arRequests.back();
        ar.receiver = m_client;
        ar.status   = req_status;
        Event e(EventARRequest, &ar);
        e.process();
        return;
    }

    string message;
    if (msg)
        message = msg;

    startPacket(TCP_ACK, seq);
    m_socket->writeBuffer.pack(type);
    m_socket->writeBuffer.pack(status);
    m_socket->writeBuffer.pack(flags);
    m_socket->writeBuffer
    << message
    << 0x00000000L
    << 0xFFFFFFFFL;
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
    *((unsigned short*)p) = size;

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
    X1 = p[M1] ^ 0xFF;
    X2 = rand() % 220;
    X3 = client_check_data[X2] ^ 0xFF;

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

void DirectClient::acceptMessage(Message*)
{
}

void DirectClient::declineMessage(Message*, const char*)
{
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
            string message;
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
                    message = m_client->createRTF(sm.msg->getRichText(), sm.msg->getForeground(), m_data->Encoding);
                    sm.type = CAP_RTF;
                }else if (m_client->hasCap(m_data, CAP_UTF) &&
                          (m_client->getSendFormat() <= 1)){
                    message = sm.msg->getPlainText().utf8();
                    sm.type = CAP_UTF;
                }else{
                    message = m_client->fromUnicode(sm.msg->getPlainText(), m_data);
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
            case MessageURL:
            case MessageContact:
            case MessageOpenSecure:
            case MessageCloseSecure:
                startPacket(TCP_START, 0);
                m_client->packMessage(mb, sm.msg, m_data, sm.icq_type, m_nSequence);
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
                unsigned s = m_data->Status;
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
                string message;
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
    virtual bool initSSL();
};

// AUTOGENERATED by dhparam
static DH *get_dh512()
{
    static unsigned char dh512_p[]={
                                       0xFF,0xD3,0xF9,0x7C,0xEB,0xFE,0x45,0x2E,0x47,0x41,0xC1,0x8B,
                                       0xF7,0xB9,0xC6,0xF2,0x40,0xCF,0x10,0x8B,0xF3,0xD7,0x08,0xC7,
                                       0xF0,0x3F,0x46,0x7A,0xAD,0x71,0x6A,0x70,0xE1,0x76,0x8F,0xD9,
                                       0xD4,0x46,0x70,0xFB,0x31,0x9B,0xD8,0x86,0x58,0x03,0xE6,0x6F,
                                       0x08,0x9B,0x16,0xA0,0x78,0x70,0x6C,0xB1,0x78,0x73,0x52,0x3F,
                                       0xD2,0x74,0xED,0x9B,
                                   };
    static unsigned char dh512_g[]={
                                       0x02,
                                   };
    DH *dh;

    if ((dh=DH_new()) == NULL) return(NULL);
    dh->p=BN_bin2bn(dh512_p,sizeof(dh512_p),NULL);
    dh->g=BN_bin2bn(dh512_g,sizeof(dh512_g),NULL);
    if ((dh->p == NULL) || (dh->g == NULL))
    { DH_free(dh); return(NULL); }
    return(dh);
}

bool ICQ_SSLClient::initSSL()
{
    mpCTX = SSL_CTX_new(TLSv1_method());
    if (mpCTX == NULL)
        return false;
#if OPENSSL_VERSION_NUMBER >= 0x00905000L
    SSL_CTX_set_cipher_list(pCTX, "ADH:@STRENGTH");
#else
    SSL_CTX_set_cipher_list(pCTX, "ADH");
#endif
    DH *dh = get_dh512();
    SSL_CTX_set_tmp_dh(pCTX, dh);
    DH_free(dh);
    mpSSL = SSL_new(pCTX);
    if(!mpSSL)
        return false;
    return true;
}

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
        m_socket->setSocket(m_ssl->socket());
        m_ssl->setSocket(NULL);
        delete m_ssl;
        m_ssl = NULL;
        Contact *contact;
        if (m_client->findContact(m_client->screen(m_data).c_str(), NULL, false, contact)){
            Event e(EventContactStatus, contact);
            e.process();
        }
    }
}
#endif

const char *DirectClient::name()
{
    if (m_data == NULL)
        return NULL;
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
    m_name += number(m_data->Uin);
    m_name += ".";
    m_name += number((unsigned)this);
    return m_name.c_str();
}

ICQFileTransfer::ICQFileTransfer(FileMessage *msg, ICQUserData *data, ICQClient *client)
        : FileTransfer(msg), DirectSocket(data, client)
{
    m_state = None;
    FileMessage::Iterator it(*msg);
    m_files     = it.count();
    m_totalSize = msg->getSize();
    m_sendTime  = 0;
    m_sendSize  = 0;
    m_transfer  = 0;
    m_f = new QFile;
}

ICQFileTransfer::~ICQFileTransfer()
{
    delete m_f;
}

void ICQFileTransfer::connect(unsigned short port)
{
    m_port = port;
    FileTransfer::m_state = FileTransfer::Connect;
    if (m_notify)
        m_notify->process();
    DirectSocket::connect();
}

void ICQFileTransfer::processPacket()
{
    ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
    log_packet(m_socket->readBuffer, false, plugin->ICQDirectPacket, "File transfer");
    char cmd;
    m_socket->readBuffer >> cmd;
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
                if (curFile != m_file){
                    m_f->setName(QFile::decodeName(it[curFile]));
                    if (!m_f->open(IO_ReadOnly)){
                        m_socket->error_state("Can't open file");
                        return;
                    }
                    if (!m_f->at(pos)){
                        m_socket->error_state("Can't set transfer position");
                        return;
                    }
                    m_file     = curFile;
                }
                m_totalBytes += pos;
                m_bytes		  = pos;
                m_fileSize    = m_f->size();
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
    default:
        log(L_WARN, "Bas state in process packet");
    }
}

bool ICQFileTransfer::error_state(const char *err, unsigned code)
{
    if (!DirectSocket::error_state(err, code))
        return false;
    if (FileTransfer::m_state != FileTransfer::Done){
        m_state = None;
        FileTransfer::m_state = FileTransfer::Error;
        m_msg->setError(err);
    }
    m_msg->m_transfer = NULL;
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
    m_socket->writeBuffer.pack((unsigned long)m_files);			// nFiles
    m_socket->writeBuffer.pack((unsigned long)m_totalSize);		// Total size
    m_socket->writeBuffer.pack((unsigned long)m_speed);			// speed
    m_socket->writeBuffer << number(m_client->data.owner.Uin);
    sendPacket();
    if ((m_files == 0) || (m_totalSize == 0))
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
    *((unsigned short*)p) = size;
    if (dump){
        ICQPlugin *plugin = static_cast<ICQPlugin*>(m_client->protocol()->plugin());
        string name = "FileTranfer";
        if (m_data){
            name += ".";
            name += number(m_data->Uin);
        }
        log_packet(m_socket->writeBuffer, true, plugin->ICQDirectPacket, name.c_str());
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
        m_f->close();
        m_file++;
        if (m_notify)
            m_notify->transfer(false);
        if (m_file >= m_files){
            FileTransfer::m_state = FileTransfer::Done;
            if (m_notify)
                m_notify->process();
            m_socket->error_state("Done");
            return;
        }
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
    int readn = m_f->readBlock(buf, tail);
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
    startPacket(FT_FILEINFO);
    m_socket->writeBuffer.pack((char)0);
    FileMessage::Iterator it(*m_msg);
    QString curFile = QFile::decodeName(it[m_file]);
    m_f->setName(curFile);
    if (!m_f->open(IO_ReadOnly)){
        m_socket->error_state(I18N_NOOP("Can't open file"));
        return;
    }
    m_bytes    = 0;
    m_fileSize = m_f->size();
    if (m_notify)
        m_notify->process();
    string empty;
#ifdef WIN32
    int n = curFile.findRev('\\');
#else
    int n = curFile.findRev('/');
#endif
    if (n >= 0)
        curFile = curFile.mid(n + 1);
    string s = m_client->fromUnicode(curFile, m_data);
    m_socket->writeBuffer << s << empty;
    m_socket->writeBuffer.pack((unsigned long)m_fileSize);
    m_socket->writeBuffer.pack((unsigned long)0);
    m_socket->writeBuffer.pack((unsigned long)m_speed);
    sendPacket();
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
}



