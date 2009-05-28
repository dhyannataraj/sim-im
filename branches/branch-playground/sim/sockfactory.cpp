/***************************************************************************
                          sockfactory.cpp  -  description
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

#include "sockfactory.h"
#include "fetch.h"
#include "log.h"
#include "misc.h"

#ifdef WIN32
	#include <winsock.h>
	#include <wininet.h>
#else
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <sys/un.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <pwd.h>
	#include <net/if.h>
	#include <sys/ioctl.h>
#endif

#include <errno.h>

#include <qfile.h>
#include <qregexp.h>
#include <q3socket.h>
#include <q3socketdevice.h>
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <QTimerEvent>

#ifndef WIN32
	// name resolving
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <q3dns.h>
#else
	#include <q3dns.h>
#endif

#ifndef INADDR_NONE
	#define INADDR_NONE     0xFFFFFFFF
#endif

const unsigned CONNECT_TIMEOUT = 60;

namespace SIM
{

using namespace std;

SIMSockets::SIMSockets(QObject *parent)
 : SocketFactory(parent)
{
}

SIMSockets::~SIMSockets()
{
}

void SIMSockets::checkState()
{
#ifdef WIN32
    bool state;
    if (get_connection_state(state))
        setActive(state);
#endif
}

void SIMSockets::idle()
{
    SocketFactory::idle();
}

SIMResolver::SIMResolver(QObject *parent, const QString &host)
    : QObject(parent)
{
    bDone = false;
    bTimeout = false;
#ifdef WIN32
    bool bState;
    if (get_connection_state(bState) && !bState){
        QTimer::singleShot(0, this, SLOT(resolveTimeout()));
        return;
    }
#endif
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(resolveTimeout()));
    timer->start(20000);
    dns = new Q3Dns(host, Q3Dns::A);
    connect(dns, SIGNAL(resultsReady()), this, SLOT(resolveReady()));
}

SIMResolver::~SIMResolver()
{
    delete dns;
    delete timer;
}

void SIMResolver::resolveTimeout()
{
    bDone    = true;
    bTimeout = true;
    getSocketFactory()->setActive(false);
    QTimer::singleShot(0, parent(), SLOT(resultsReady()));
}

void SIMResolver::resolveReady()
{
    bDone = true;
    QTimer::singleShot(0, parent(), SLOT(resultsReady()));
}

unsigned long SIMResolver::addr()
{
    if (dns->addresses().isEmpty())
        return INADDR_NONE;
    // crissi
    struct hostent * server_entry;
    if ( ( server_entry = gethostbyname( dns->label().ascii() ) ) == NULL ) {
        log( L_WARN, "gethostbyname failed\n" );
        return htonl(dns->addresses().first().toIPv4Address());
    }
    return inet_addr(inet_ntoa(*( struct in_addr* ) server_entry->h_addr_list[ 0 ] ));
}

QString SIMResolver::host() const
{
    return dns->label();
}

bool SIMResolver::isDone()
{
	return bDone;
}

bool SIMResolver::isTimeout()
{
	return bTimeout;
}



StdResolver::StdResolver(QObject* parent, const QString& host) : QThread(parent), m_done(false),
	m_timeout(false), m_addr(0),
	m_host(host)
{
	log(L_DEBUG, "StdResolver::StdResolver()");
	this->start();
}

StdResolver::~StdResolver()
{

}

unsigned long StdResolver::addr()
{
	return m_addr;
}

QString StdResolver::host() const
{
	return m_host;
}

void StdResolver::run()
{
	struct hostent* server_entry = gethostbyname(m_host.toUtf8().constData());
	if(server_entry == NULL)
	{
		log(L_WARN, "gethostbyname failed");
		m_timeout = true;
		m_done = true;
    	QTimer::singleShot(0, parent(), SLOT(resultsReady()));
		return;
	}
	m_addr = inet_addr(inet_ntoa(*(struct in_addr*)server_entry->h_addr_list[0]));
	m_done = true;
    QTimer::singleShot(0, parent(), SLOT(resultsReady()));
}

bool StdResolver::isDone()
{
	return m_done;
}

bool StdResolver::isTimeout()
{
	return m_timeout;
}

void SIMSockets::resolve(const QString &host)
{
	// Win32 uses old resolver, based on QDns (which is buggy in qt3)
	// *nix use new resolver
//#ifdef WIN32
//    SIMResolver *resolver = new SIMResolver(this, host);
//#else
    StdResolver *resolver = new StdResolver(this, host);
//#endif
    resolvers.push_back(resolver);
}

void SIMSockets::resultsReady()
{
    list<IResolver*>::iterator it;
    for (it = resolvers.begin(); it != resolvers.end();){
        IResolver *r = *it;
        if (!r->isDone()){
            ++it;
            continue;
        }
        bool isActive;
        if (r->isTimeout()){
            isActive = false;
        }else{
            isActive = true;
        }
        if (r->addr() == INADDR_NONE)
            isActive = false;
#ifdef WIN32
        bool bState;
        if (get_connection_state(bState))
            isActive = bState;
#endif
        setActive(isActive);
        emit resolveReady(r->addr(), r->host());
        resolvers.remove(r);
        delete r;
        it = resolvers.begin();
    }
}

Socket *SIMSockets::createSocket()
{
    return new SIMClientSocket;
}

ServerSocket *SIMSockets::createServerSocket()
{
    return new SIMServerSocket();
}

SIMClientSocket::SIMClientSocket(Q3Socket *s)
{
    sock = s;
    if (sock == NULL)
        sock = new Q3Socket(this);
    QObject::connect(sock, SIGNAL(connected()), this, SLOT(slotConnected()));
    QObject::connect(sock, SIGNAL(connectionClosed()), this, SLOT(slotConnectionClosed()));
    QObject::connect(sock, SIGNAL(error(int)), this, SLOT(slotError(int)));
    QObject::connect(sock, SIGNAL(readyRead()), this, SLOT(slotReadReady()));
    QObject::connect(sock, SIGNAL(bytesWritten(int)), this, SLOT(slotBytesWritten(int)));
    bInWrite = false;
    timer = NULL;
	m_carrierCheckTimer = 0;
}

SIMClientSocket::~SIMClientSocket()
{
    if (!sock)
        return;
    timerStop();
    sock->close();

    if (sock->state() == Q3Socket::Closing)
        sock->connect(sock, SIGNAL(delayedCloseFinished()), SLOT(deleteLater()));
    else
        delete sock;
}

void SIMClientSocket::close()
{
    timerStop();
    sock->close();
}

void SIMClientSocket::timerStop()
{
    if (timer){
        delete timer;
        timer = NULL;
    }
}

void SIMClientSocket::slotLookupFinished(int state)
{
    log(L_DEBUG, "Lookup finished %u", state);
    if (state == 0){
        log(L_WARN, "Can't lookup");
        notify->error_state(I18N_NOOP("Connect error"));
        getSocketFactory()->setActive(false);
    }
}

int SIMClientSocket::read(char *buf, unsigned int size)
{
    unsigned available = sock->bytesAvailable();
    if (size > available)
        size = available;
    if (size == 0)
        return size;
    int res = sock->read(buf, size);
    if (res < 0){
        log(L_DEBUG, "QClientSocket::read error %u", errno);
        if (notify)
            notify->error_state(I18N_NOOP("Read socket error"));
        return -1;
    }
    return res;
}

void SIMClientSocket::write(const char *buf, unsigned int size)
{
    bInWrite = true;
    int res = sock->write(buf, size);
    bInWrite = false;
    if (res != (int)size){
        if (notify)
            notify->error_state(I18N_NOOP("Write socket error"));
        return;
    }
    if (sock->bytesToWrite() == 0)
        QTimer::singleShot(0, this, SLOT(slotBytesWritten()));
}

void SIMClientSocket::connect(const QString &_host, unsigned short _port)
{
    port = _port;
    host = _host;
    if (host.isNull())
        host=""; // Avoid crashing when _host is NULL
#ifdef WIN32
    bool bState;
    if (get_connection_state(bState) && !bState){
        QTimer::singleShot(0, this, SLOT(slotConnectionClosed()));
        return;
    }
#endif
    log(L_DEBUG, QString("Connect to %1:%2").arg(host).arg(port));
    if (inet_addr(host) == INADDR_NONE){
        log(L_DEBUG, QString("Start resolve %1").arg(host));
        SIMSockets *s = static_cast<SIMSockets*>(getSocketFactory());
        QObject::connect(s, SIGNAL(resolveReady(unsigned long, const QString&)),
                         this, SLOT(resolveReady(unsigned long, const QString&)));
        s->resolve(host);
        return;
    }
    resolveReady(inet_addr(host), host);
}

void SIMClientSocket::resolveReady(unsigned long addr, const QString &_host)
{
    if (_host != host)
        return;
    if (addr == INADDR_NONE){
        if (notify)
            notify->error_state(I18N_NOOP("Can't resolve host"));
        return;
    }
    if (notify)
        notify->resolve_ready(addr);
    in_addr a;
    a.s_addr = addr;
    host = inet_ntoa(a);
    log(L_DEBUG, QString("Resolve ready %1").arg(host));
    timerStop();
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(CONNECT_TIMEOUT * 1000);
    sock->connectToHost(host, port);
}

void SIMClientSocket::slotConnected()
{
    log(L_DEBUG, "Connected");
    timerStop();
    if (notify) notify->connect_ready();
    getSocketFactory()->setActive(true);
	m_state = true;
#if !defined(WIN32) && !defined(Q_OS_MAC)
	m_carrierCheckTimer = startTimer(10000); // FIXME hardcoded
#endif
}

void SIMClientSocket::slotConnectionClosed()
{
    log(L_WARN, "Connection closed");
    timerStop();
    if (notify)
        notify->error_state(I18N_NOOP("Connection closed"));
#ifdef WIN32
    bool bState;
    if (get_connection_state(bState) && !bState)
        getSocketFactory()->setActive(false);
#endif
}

void SIMClientSocket::timeout()
{
    QTimer::singleShot(0, this, SLOT(slotConnectionClosed()));
}

void SIMClientSocket::timerEvent(QTimerEvent* ev)
{
	if(m_carrierCheckTimer != 0 && ev->timerId() == m_carrierCheckTimer)
	{
		checkInterface();
	}
}

void SIMClientSocket::checkInterface()
{
#if !defined(WIN32) && !defined(Q_OS_MAC)
	int fd = sock->socket();
	if(fd == -1)
	{
		return;
	}
	struct ifreq ifr;
	struct ifreq* ifrp;
	struct ifreq ibuf[16];
	struct ifconf	ifc;

	ifc.ifc_len = sizeof(ifr)*16;
	ifc.ifc_buf = (caddr_t)&ibuf;
	memset(ibuf, 0, sizeof(struct ifreq)*16);

	int hret = ioctl(fd, SIOCGIFCONF, &ifc);
	if(hret == -1)
	{
		return;
	}
	bool iffound = false;
	for(int i = 0; i < ifc.ifc_len/sizeof(struct ifreq); i++)
	{
		ifrp = ibuf + i;
		strncpy(ifr.ifr_name, ifrp->ifr_name, sizeof(ifr.ifr_name));

		if  (
			strcmp(ifr.ifr_name, "lo") == 0 ||
		    (htonl(((sockaddr_in*)&ifrp->ifr_addr)->sin_addr.s_addr) != sock->address().toIPv4Address())
			)	continue;

		m_interface = ifr.ifr_name;
		iffound = true;

		hret = ioctl(fd, SIOCGIFFLAGS, &ifr);
		if(hret != -1)
		{
			int state = ifr.ifr_flags & IFF_RUNNING;
			if(state < 0)
			{
				log(L_DEBUG, "Incorrect state: %d (%s)", state, ifr.ifr_name);
				return;
			}
			if((state == 0) && (m_state))
			{
				m_state = false;
				emit interfaceDown(fd);
				EventInterfaceDown e(fd);
				e.process();
				return;
			}
			if((state != 0) && (!m_state))
			{
				m_state = true;
				emit interfaceUp(fd);
				EventInterfaceUp e(fd);
				e.process();
				return;
			}
			return;
		}
	}
	if(!iffound)
	{
		m_state = false;
		emit interfaceDown(fd);
		EventInterfaceDown e(fd);
		e.process();
	}
#else
	return;
#endif
}

void SIMClientSocket::error(int errcode)
{
	log(L_DEBUG, "SIMClientSocket::error(%d), SocketDevice error: %d", errcode, sock->socketDevice()->error());
}

void SIMClientSocket::slotReadReady()
{
    if (notify)
        notify->read_ready();
}

void SIMClientSocket::slotBytesWritten(int)
{
    slotBytesWritten();
}

void SIMClientSocket::slotBytesWritten()
{
    if (bInWrite || (sock == NULL)) return;
    if ((sock->bytesToWrite() == 0) && notify)
        notify->write_ready();
}

#ifdef WIN32
#define socklen_t int
#endif

unsigned long SIMClientSocket::localHost()
{
    unsigned long res = 0;
    int s = sock->socket();
    struct sockaddr_in addr;
    memset(&addr, sizeof(addr), 0);
    socklen_t size = sizeof(addr);
    if (getsockname(s, (struct sockaddr*)&addr, &size) >= 0)
        res = addr.sin_addr.s_addr;
    if (res == 0x7F000001){
        char hostName[255];
        if (gethostname(hostName,sizeof(hostName)) >= 0) {
            struct hostent *he = NULL;
            he = gethostbyname(hostName);
            if (he != NULL)
                res = *((unsigned long*)(he->h_addr));
        }
    }
    return res;
}

void SIMClientSocket::slotError(int err)
{
    if (err)
        log(L_DEBUG, "Slot error %u", err);
    timerStop();
    if (notify)
        notify->error_state(I18N_NOOP("Socket error"));
}

void SIMClientSocket::pause(unsigned t)
{
    QTimer::singleShot(t * 1000, this, SLOT(slotBytesWritten()));
}

int SIMClientSocket::getFd()
{
	return sock->socket();
}

SIMServerSocket::SIMServerSocket()
{
    sn = NULL;
    sock = new Q3SocketDevice;
}

SIMServerSocket::~SIMServerSocket()
{
    close();
}

void SIMServerSocket::close()
{
    if (sn){
        delete sn;
        sn = NULL;
    }
    if (sock){
        delete sock;
        sock = NULL;
    }
    if (!m_name.isEmpty())
        QFile::remove(m_name);
}

void SIMServerSocket::bind(unsigned short minPort, unsigned short maxPort, TCPClient *client)
{
    if (client && notify){
        EventSocketListen e(notify, client);
        if (e.process())
            return;
    }
    unsigned short startPort = (unsigned short)(minPort + get_random() % (maxPort - minPort + 1));
    bool bOK = false;
    for (m_nPort = startPort;;){
        if (sock->bind(QHostAddress(), m_nPort)){
            bOK = true;
            break;
        }
        if (++m_nPort > maxPort)
            m_nPort = minPort;
        if (m_nPort == startPort)
            break;
    }
    if (!bOK || !sock->listen(50)){
        error(I18N_NOOP("Can't allocate port"));
        return;
    }
    listen(client);
}

#ifndef WIN32

void SIMServerSocket::bind(const char *path)
{
    m_name = QFile::decodeName(path);
    QString user_id;
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    if (pwd){
        user_id = pwd->pw_name;
    }else{
        user_id = QString::number(uid);
    }
    m_name = m_name.replace(QRegExp("\\%user\\%"), user_id);
    QFile::remove(m_name);

    int s = socket(PF_UNIX, SOCK_STREAM, 0);
    if (s == -1){
        error("Can't create listener");
        return;
    }
    sock->setSocket(s, Q3SocketDevice::Stream);

    struct sockaddr_un nsun;
    nsun.sun_family = AF_UNIX;
    strcpy(nsun.sun_path, QFile::encodeName(m_name));
    if (::bind(s, (struct sockaddr*)&nsun, sizeof(nsun)) < 0){
        log(L_WARN, "Can't bind %s: %s", nsun.sun_path, strerror(errno));
        error("Can't bind");
        return;
    }
    if (::listen(s, 156) < 0){
        log(L_WARN, "Can't listen %s: %s", nsun.sun_path, strerror(errno));
        error("Can't listen");
        return;
    }
    listen(NULL);
}

#endif

void SIMServerSocket::error(const char *err)
{
    close();
    if (notify && notify->error(err)){
        notify->m_listener = NULL;
        getSocketFactory()->remove(this);
    }
}

void SIMServerSocket::listen(TCPClient*)
{
    sn = new QSocketNotifier(sock->socket(), QSocketNotifier::Read, this);
    connect(sn, SIGNAL(activated(int)), this, SLOT(activated(int)));
    if (notify)
        notify->bind_ready(m_nPort);
}

void SIMServerSocket::activated(int)
{
    if (sock == NULL) return;
    int fd = sock->accept();
    if (fd >= 0){
        log(L_DEBUG, "accept ready");
        if (notify){
            Q3Socket *s = new Q3Socket;
            s->setSocket(fd);
            if (notify->accept(new SIMClientSocket(s), htonl(s->address().toIPv4Address()))){
                if (notify)
                    notify->m_listener = NULL;
                getSocketFactory()->remove(this);
            }
        }else{
#ifdef WIN32
            ::closesocket(fd);
#else
            ::close(fd);
#endif
        }
    }
}

void SIMServerSocket::activated()
{
}

SocketFactory *getSocketFactory()
{
    return PluginManager::factory;
}

// ______________________________________________________________________________________

static IPResolver *pResolver = NULL;

void deleteResolver()
{
    delete pResolver;
}

IP::IP()
 : m_ip(0)
{
}

IP::~IP()
{
    if (pResolver){
        for (list<IP*>::iterator it = pResolver->queue.begin(); it != pResolver->queue.end(); ++it){
            if ((*it) == this){
                pResolver->queue.erase(it);
                break;
            }
        }
    }
}

void IP::set(unsigned long ip, const QString &host)
{
    bool bResolve = false;
    if (ip != m_ip){
        m_ip = ip;
        m_host = QString::null;
        bResolve = true;
    }
    m_host = host;
    if (bResolve && !m_host.isEmpty())
        resolve();
}

void IP::resolve()
{
    if(!m_host.isEmpty())
        return;
    if (pResolver == NULL)
        pResolver = new IPResolver;
    for (list<IP*>::iterator it = pResolver->queue.begin(); it != pResolver->queue.end(); ++it){
        if ((*it) == this)
            return;
    }
    pResolver->queue.push_back(this);
    pResolver->start_resolve();
}

IPResolver::IPResolver()
{
    resolver = new Q3Dns;
    resolver->setRecordType(Q3Dns::Ptr);
    QObject::connect(resolver, SIGNAL(resultsReady()), this, SLOT(resolve_ready()));
}

IPResolver::~IPResolver()
{
    if (resolver)
        delete resolver;
}

#define iptoul(a,b,c,d) (unsigned long)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

static inline bool isPrivate(unsigned long ip)
{
    ip = ntohl(ip);
    if (ip >= iptoul(10,0,0,0) && ip <= iptoul(10,255,255,255) ||
        ip >= iptoul(172,16,0,0) && ip <= iptoul(172,31,255,255) ||
        ip >= iptoul(192,168,0,0) && ip <= iptoul(192,168,255,255))
        return true;
    return false;
}

void IPResolver::resolve_ready()
{
    if (queue.empty()) return;
    QString m_host;
    if (resolver->hostNames().count())
        m_host = resolver->hostNames().first();
    struct in_addr inaddr;
    inaddr.s_addr = m_addr;
    log(L_DEBUG, "Resolver ready %s %s", inet_ntoa(inaddr), qPrintable(m_host));
    delete resolver;
    resolver = NULL;
    for (list<IP*>::iterator it = queue.begin(); it != queue.end(); ){
        if ((*it)->ip() != m_addr){
            ++it;
            continue;
        }
        (*it)->set((*it)->ip(), m_host);
        queue.erase(it);
        it = queue.begin();
    }
    start_resolve();
}

void IPResolver::start_resolve()
{
    if (resolver && resolver->isWorking()) return;
    struct in_addr inaddr;
    for(;;) {
        if (queue.empty())
            return;
        IP *ip = *queue.begin();
        m_addr = ip->ip();
        inaddr.s_addr = m_addr;
        if (!isPrivate(m_addr))
            break;
        log(L_DEBUG, "Private IP: %s", inet_ntoa(inaddr));
        queue.erase(queue.begin());
    }
    log(L_DEBUG, "start resolve %s", inet_ntoa(inaddr));
    if (resolver)
        delete resolver;
    resolver = new Q3Dns(QHostAddress(htonl(m_addr)), Q3Dns::Ptr);
    connect(resolver, SIGNAL(resultsReady()), this, SLOT(resolve_ready()));
}

}

