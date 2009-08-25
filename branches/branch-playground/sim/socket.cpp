/***************************************************************************
                          socket.cpp  -  description
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

#include <QMutex>
#include <QTimer>
#include <QSet>
#include <QTimerEvent>

#ifdef WIN32
	#include <winsock.h>
#else
#ifndef Q_OS_MAC
	#include <net/if.h>
#endif
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#endif


#include "socket.h"
#include "misc.h"
#include "log.h"

namespace SIM
{

using namespace std;

#ifndef INADDR_NONE
#define INADDR_NONE	0xFFFFFFFF
#endif

const unsigned RECONNECT_TIME		= 5;
const unsigned RECONNECT_IFINACTIVE = 60;
const unsigned LOGIN_TIMEOUT		= 120;

struct SocketFactoryPrivate
{
    bool m_bActive;

    QSet<ClientSocket*> errSockets;
    QSet<ClientSocket*> errSocketsCopy;
    QSet<Socket*> removedSockets;
    QSet<ServerSocket*> removedServerSockets;

    SocketFactoryPrivate() : m_bActive(true) {}
};

void Socket::error(const QString &err_text, unsigned code)
{
    if (notify)
        notify->error_state(err_text, code);
}

ServerSocket::ServerSocket()
{
    notify = NULL;
}

ClientSocket::ClientSocket(ClientSocketNotify *notify, Socket *sock)
{
    m_notify = notify;
    bRawMode = false;
    bClosed  = false;
    m_sock   = sock;
    if (m_sock == NULL)
        m_sock = getSocketFactory()->createSocket();
    m_sock->setNotify(this);
}

ClientSocket::~ClientSocket()
{
    getSocketFactory()->erase(this);
    delete m_sock;
}

void ClientSocket::close()
{
    m_sock->close();
    bClosed = true;
}

const QString &ClientSocket::errorString() const
{
    return errString;
}

void ClientSocket::connect(const QString &host, unsigned short port, TCPClient *client)
{
    if (client){
        EventSocketConnect e(this, client, host, port);
        e.process();
    }
    m_sock->connect(host, port);
}

void ClientSocket::connect(unsigned long ip, unsigned short port, TCPClient* /* client */)
{
	struct in_addr addr;
	addr.s_addr = ip;
	this->connect(inet_ntoa(addr), port, NULL);
}

void ClientSocket::write()
{
    if (writeBuffer().size() == 0)
        return;
    m_sock->write(writeBuffer().data(), writeBuffer().size());
    writeBuffer().init(0);
}

bool ClientSocket::created()
{
    return (m_sock != NULL);
}

void ClientSocket::resolve_ready(unsigned long ip)
{
    m_notify->resolve_ready(ip);
}

void ClientSocket::connect_ready()
{
    m_notify->connect_ready();
    bClosed = false;
}

void ClientSocket::setRaw(bool mode)
{
    bRawMode = mode;
    read_ready();
}

void ClientSocket::read_ready()
{
    if (bRawMode){
        for (;;){
            char b[2048];
            int readn = m_sock->read(b, sizeof(b));
            if (readn < 0){
                error_state(I18N_NOOP("Read socket error"));
                return;
            }
            if (readn == 0)
                break;
            unsigned pos = readBuffer().writePos();
            readBuffer().setWritePos(readBuffer().writePos() + readn);
            memcpy((void*)readBuffer().data(pos), b, readn);
        }
        if (m_notify)
            m_notify->packet_ready();
        return;
    }
    for (;;){
        if (bClosed || errString.length())
          break;
        int readn = m_sock->read((char*)readBuffer().data(readBuffer().writePos()), (readBuffer().size() - readBuffer().writePos()));
        if (readn < 0){
            error_state(I18N_NOOP("Read socket error"));
            return;
        }
        if (readn == 0)
          break;
        readBuffer().setWritePos(readBuffer().writePos() + readn);
        if(readBuffer().writePos() < (unsigned)readBuffer().size())
          break;
        if (m_notify)
            m_notify->packet_ready();
    }
}

void ClientSocket::write_ready()
{
    if (m_notify)
        m_notify->write_ready();
}

unsigned long ClientSocket::localHost()
{
    return m_sock->localHost();
}

void ClientSocket::pause(unsigned n)
{
    m_sock->pause(n);
}

void ClientSocket::setSocket(Socket *s, bool bClearError)
{
    if (m_sock){
        if (m_sock->getNotify() == this)
            m_sock->setNotify(NULL);
        if (bClearError){
            getSocketFactory()->erase(this);
        }
    }
    m_sock = s;
    if (s)
        s->setNotify(this);
}

void ClientSocket::error_state(const QString &err, unsigned code)
{
    // -> false -> already there
    if(!getSocketFactory()->add(this))
      return;

    errString = err;
    errCode = code;
    QTimer::singleShot(0, getSocketFactory(), SLOT(idle()));
}

SocketFactory::SocketFactory(QObject *parent)
  : QObject(parent)
{
  d = new SocketFactoryPrivate;
}

SocketFactory::~SocketFactory()
{
    idle();
    delete d;
}

bool SocketFactory::isActive() const
{
    return d->m_bActive;
}

void SocketFactory::setActive(bool isActive)
{
    if (isActive == d->m_bActive)
        return;
    d->m_bActive = isActive;
    EventSocketActive(d->m_bActive).process();
}

void SocketFactory::remove(Socket *s)
{
    s->setNotify(NULL);
    s->close();

    if(d->removedSockets.contains(s))
      return;

    d->removedSockets.insert(s);

    QTimer::singleShot(0, this, SLOT(idle()));
}

void SocketFactory::remove(ServerSocket *s)
{
    s->setNotify(NULL);
    s->close();

    if(d->removedServerSockets.contains(s))
      return;

    d->removedServerSockets.insert(s);
    QTimer::singleShot(0, this, SLOT(idle()));
}

bool SocketFactory::add(ClientSocket *s)
{
	if(!d->errSockets.contains(s)) 
	{
		d->errSockets += s;
		return true;
	}
	return false;
}

bool SocketFactory::erase(ClientSocket *s)
{
  return(d->errSockets.remove(s));
}

void SocketFactory::idle()
{
    d->errSocketsCopy = d->errSockets;  // important! error_state() modifes d->errSockets
    d->errSockets.clear();

    QSetIterator<ClientSocket*> it(d->errSocketsCopy);
    while ( it.hasNext() ){
        ClientSocket *s = it.next();
        // can be removed in SocketFactory::erase();
        if(!s)
          continue;
        ClientSocketNotify *n = s->m_notify;
        if (n){
            QString errString = s->errorString();
            s->errString.clear();
            if (n->error_state(errString, s->errCode))
                delete n;
        }
    }

    qDeleteAll(d->removedSockets);
    d->removedSockets.clear();

    qDeleteAll(d->removedServerSockets);
    d->removedServerSockets.clear();
}

TCPClient::TCPClient(Protocol *protocol, Buffer *cfg, unsigned priority)
  : Client(protocol, cfg)
  , EventReceiver(priority)
  , m_reconnect(RECONNECT_TIME)
  , m_logonStatus(STATUS_OFFLINE)
  , m_ip(0)
  , m_timer(new QTimer(this))
  , m_loginTimer(new QTimer(this))
  , m_bWaitReconnect(false)
  , m_clientSocket(NULL)
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(reconnect()));
    connect(m_loginTimer, SIGNAL(timeout()), this, SLOT(loginTimeout()));
}

bool TCPClient::processEvent(Event *e)
{
    if (e->type() == eEventSocketActive)
	{
		EventSocketActive *s = static_cast<EventSocketActive*>(e);
        if (m_bWaitReconnect && s->active())
            reconnect();
    }
    return false;
}

void TCPClient::resolve_ready(unsigned long ip)
{
    m_ip = ip;
}

bool TCPClient::error_state(const QString &err, unsigned code)
{
    log(L_DEBUG, "Socket error %s (%u)", qPrintable(err), code);
    m_loginTimer->stop();
    if (m_reconnect == NO_RECONNECT){
        m_timer->stop();
        setStatus(STATUS_OFFLINE, getCommonStatus());
        setState(Error, err, code);
        return false;
    }
    if (!m_timer->isActive()){
        unsigned reconnectTime = m_reconnect;
        if (!getSocketFactory()->isActive()){
            if (reconnectTime < RECONNECT_IFINACTIVE)
                reconnectTime = RECONNECT_IFINACTIVE;
        }
        setClientStatus(STATUS_OFFLINE);
        setState((m_reconnect == NO_RECONNECT) ? Error : Connecting, err, code);
        m_bWaitReconnect = true;
        log(L_DEBUG, "Wait reconnect %u sec", reconnectTime);
        m_timer->start(reconnectTime * 1000);
    } else {
        /*
          slot reconnect() neeeds this flag 
          to be true to make actual reconnect,
          but it was somehow false. serzh.
        */
        m_bWaitReconnect = true;
    }
    return false;
}

void TCPClient::reconnect()
{
    m_timer->stop();
    if (m_bWaitReconnect)
        setClientStatus(getManualStatus());
}

void TCPClient::setStatus(unsigned status, bool bCommon)
{
    setClientStatus(status);
    Client::setStatus(status, bCommon);
}

void TCPClient::connect_ready()
{
    m_timer->stop();
    m_bWaitReconnect = false;
    m_loginTimer->stop();
    m_loginTimer->setSingleShot(true);
    m_loginTimer->start(LOGIN_TIMEOUT * 1000);
}

void TCPClient::loginTimeout()
{
    m_loginTimer->stop();
    if ((m_state != Connected) && socket())
        socket()->error_state(I18N_NOOP("Login timeout"));
}

Socket *TCPClient::createSocket()
{
    return NULL;
}

void TCPClient::socketConnect()
{
    if (socket())
        socket()->close();
    if (socket() == NULL)
        m_clientSocket = createClientSocket();
    log(L_DEBUG, "Start connect %s:%u", qPrintable(getServer()), getPort());
    socket()->connect(getServer(), getPort(), this);
}

ClientSocket *TCPClient::createClientSocket()
{
    return new ClientSocket(this, createSocket());
}

void TCPClient::setClientStatus(unsigned status)
{
    if (status != STATUS_OFFLINE){
        if (getState() == Connected){
            setStatus(status);
            return;
        }
        m_logonStatus = status;
        if ((getState() != Connecting) || m_bWaitReconnect){
            setState(Connecting, NULL);
            m_reconnect = RECONNECT_TIME;
            m_bWaitReconnect = false;
            setState(Connecting);
            socketConnect();
        }
        return;
    }
    m_bWaitReconnect = false;
    m_timer->stop();
    m_loginTimer->stop();
    if (socket())
        setStatus(STATUS_OFFLINE);
    m_status = STATUS_OFFLINE;
    setState(Offline);
    disconnected();
    if (socket()){
        socket()->close();
        delete socket();
        m_clientSocket = NULL;
    }
}

ServerSocketNotify::ServerSocketNotify()
{
    m_listener = NULL;
}

ServerSocketNotify::~ServerSocketNotify()
{
    if (m_listener)
        getSocketFactory()->remove(m_listener);
}

void ServerSocketNotify::setListener(ServerSocket *listener)
{
    if (m_listener)
        getSocketFactory()->remove(m_listener);
    m_listener = listener;
    if (m_listener)
        m_listener->setNotify(this);
}

void ServerSocketNotify::bind(unsigned short minPort, unsigned short maxPort, TCPClient *client)
{
    if (m_listener)
        getSocketFactory()->remove(m_listener);
    m_listener = getSocketFactory()->createServerSocket();
    m_listener->setNotify(this);
    m_listener->bind(minPort, maxPort, client);
}

#ifndef WIN32

void ServerSocketNotify::bind(const char *path)
{
    if (m_listener)
        getSocketFactory()->remove(m_listener);
    m_listener = getSocketFactory()->createServerSocket();
    m_listener->setNotify(this);
    m_listener->bind(path);
}

#endif

InterfaceChecker::InterfaceChecker(int polltime, bool raiseevents) : QObject(), m_pollTime(polltime), m_raiseEvents(raiseevents)
{
	m_timerID = startTimer(polltime);
	m_testSocket = socket(PF_INET, SOCK_STREAM, 0);
}

InterfaceChecker::~InterfaceChecker()
{
	killTimer(m_timerID);
#ifdef WIN32
	closesocket(m_testSocket);
#else
	close(m_testSocket);
#endif
}

void InterfaceChecker::setPollTime(int polltime)
{
	killTimer(m_timerID);
	m_timerID = startTimer(polltime);
	m_pollTime = polltime;
}

void InterfaceChecker::timerEvent(QTimerEvent* ev)
{
	if(ev->timerId() != m_timerID)
		return;
#if !defined(WIN32) && !defined(Q_OS_MAC)
	if(m_testSocket == -1)
	{
		log(L_DEBUG, "testsocket == -1");
		// TODO try to reinitialize test socket
		return;
	}
	struct ifreq ifr;
	struct ifreq* ifrp;
	struct ifreq ibuf[16];
	struct ifconf ifc;

	ifc.ifc_len = sizeof(ifr)*16;
	ifc.ifc_buf = (caddr_t)&ibuf;
	memset(ibuf, 0, sizeof(struct ifreq)*16);

	int hret = ioctl(m_testSocket, SIOCGIFCONF, &ifc);
	if(hret == -1)
	{
		log(L_DEBUG, "hret == -1");
		return;
	}

	for(std::map<std::string, tIFState>::iterator it = m_states.begin(); it != m_states.end(); ++it)
	{
		(*it).second.present = false;
	}

	for(int i = 0; i < ifc.ifc_len/sizeof(struct ifreq); i++)
	{
		ifrp = ibuf + i; 
		strncpy(ifr.ifr_name, ifrp->ifr_name, sizeof(ifr.ifr_name));

		if(strcmp(ifr.ifr_name, "lo") == 0 )
			continue;

		std::map<std::string, tIFState>::iterator it = m_states.find(ifr.ifr_name);
		if(it == m_states.end())
		{
			// New interface
			tIFState s = {true, true};
			m_states[ifr.ifr_name] = s;
			emit interfaceUp(QString(ifr.ifr_name));
			log(L_DEBUG, "%s: appeared", ifr.ifr_name);
		}

		m_states[ifr.ifr_name].present = true;
		hret = ioctl(m_testSocket, SIOCGIFFLAGS, &ifr);
		if(hret != -1)
		{
			int state = ifr.ifr_flags & IFF_RUNNING;
			if(state < 0)
			{
				log(L_DEBUG, "Incorrect state: %d (%s)", state, ifr.ifr_name);
				return;
			}
			if((state == 0) && (m_states[ifr.ifr_name].state))
			{
				m_states[ifr.ifr_name].state = false;
				emit interfaceDown(QString(ifr.ifr_name));
				if(m_raiseEvents)
				{
					EventInterfaceDown e(-1);
					e.process();
				}
				return;
			}
			if((state != 0) && (!m_states[ifr.ifr_name].state))
			{
				m_states[ifr.ifr_name].state = true;
				emit interfaceUp(QString(ifr.ifr_name));
				if(m_raiseEvents)
				{
					EventInterfaceUp e(-1);
					e.process();
				}
				return;
			}
			return;
		}
	}

	for(std::map<std::string, tIFState>::iterator it = m_states.begin(); it != m_states.end(); ++it)
	{
		if(it->second.present == false)
		{
			log(L_DEBUG, "%s: disappeared", it->first.c_str());
			it->second.state = false;
			emit interfaceDown(QString(it->first.c_str()));
			if(m_raiseEvents)
			{
				EventInterfaceDown e(-1);
				e.process();
			}
			// TODO make it work with more than one disappeared interface
			m_states.erase(it);
			return;
		}
	}
#else
	return;
#endif
}

}

/*
#ifndef NO_MOC_INCLUDES
#include "socket.moc"
#endif
*/


