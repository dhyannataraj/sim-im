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

#include "simapi.h"
#include "sockfactory.h"
#include "fetch.h"

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
#endif

#include <errno.h>

#include <QFile>
#include <QRegExp>
#include <q3socket.h>
#include <q3socketdevice.h>
#include <QSocketNotifier>
#include <QTimer>
#include <q3dns.h>

#ifndef INADDR_NONE
#define INADDR_NONE     0xFFFFFFFF
#endif

const unsigned CONNECT_TIMEOUT = 60;

namespace SIM
{

SIMSockets::SIMSockets()
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

SIMResolver::SIMResolver(QObject *parent, const char *host)
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
    return htonl(dns->addresses().first().ip4Addr());
}

string SIMResolver::host()
{
    return static_cast<string>(dns->label().toLatin1());
}

void SIMSockets::resolve(const char *host)
{
    SIMResolver *resolver = new SIMResolver(this, host);
    resolvers.push_back(resolver);
}

void SIMSockets::resultsReady()
{
    list<SIMResolver*>::iterator it;
    for (it = resolvers.begin(); it != resolvers.end();){
        SIMResolver *r = *it;
        if (!r->bDone){
            ++it;
            continue;
        }
        bool isActive;
        if (r->bTimeout){
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
        emit resolveReady(r->addr(), r->host().c_str());
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
}

SIMClientSocket::~SIMClientSocket()
{
    close();
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
    int res = sock->readBlock(buf, size);
    if (res < 0){
        log(L_DEBUG, "QClientSocket::read error %u", errno);
        if (notify) notify->error_state("Read socket error");
        return -1;
    }
    return res;
}

void SIMClientSocket::write(const char *buf, unsigned int size)
{
    bInWrite = true;
    int res = sock->writeBlock(buf, size);
    bInWrite = false;
    if (res != (int)size){
        if (notify) notify->error_state("Write socket error");
        return;
    }
    if (sock->bytesToWrite() == 0)
        QTimer::singleShot(0, this, SLOT(slotBytesWritten()));
}

void SIMClientSocket::connect(const char *_host, unsigned short _port)
{
    port = _port;
    host = _host;
#ifdef WIN32
    bool bState;
    if (get_connection_state(bState) && !bState){
        QTimer::singleShot(0, this, SLOT(slotConnectionClosed()));
        return;
    }
#endif
    log(L_DEBUG, "Connect to %s:%u", host.c_str(), port);
    if (inet_addr(host.c_str()) == INADDR_NONE){
        if (!host.empty() && (host[host.length() - 1] != '.'))
            host += ".";
        log(L_DEBUG, "Start resolve %s", host.c_str());
        SIMSockets *s = static_cast<SIMSockets*>(getSocketFactory());
        QObject::connect(s, SIGNAL(resolveReady(unsigned long, const char*)), this, SLOT(resolveReady(unsigned long, const char*)));
        s->resolve(host.c_str());
        return;
    }
    resolveReady(inet_addr(host.c_str()), host.c_str());
}

void SIMClientSocket::resolveReady(unsigned long addr, const char *_host)
{
    if (strcmp(_host, host.c_str())) return;
    if (addr == INADDR_NONE){
        if (notify) notify->error_state(I18N_NOOP("Can't resolve host"));
        return;
    }
    if (notify)
        notify->resolve_ready(addr);
    in_addr a;
    a.s_addr = addr;
    host = inet_ntoa(a);
    log(L_DEBUG, "Resolve ready %s", host.c_str());
    timerStop();
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(CONNECT_TIMEOUT * 1000);
    sock->connectToHost(host.c_str(), port);
}

void SIMClientSocket::slotConnected()
{
    log(L_DEBUG, "Connected");
    timerStop();
    if (notify) notify->connect_ready();
    getSocketFactory()->setActive(true);
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
    if (notify) notify->error_state(I18N_NOOP("Socket error"));
}

void SIMClientSocket::pause(unsigned t)
{
    QTimer::singleShot(t * 1000, this, SLOT(slotBytesWritten()));
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
        ListenParam p;
        p.notify = notify;
        p.client = client;
        Event e(EventSocketListen, &p);
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
    string user_id;
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    if (pwd){
        user_id = pwd->pw_name;
    }else{
        user_id = number(uid);
    }
    m_name = m_name.replace(QRegExp("\\%user\\%"), user_id.c_str());
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
            if (notify->accept(new SIMClientSocket(s), htonl(s->address().ip4Addr()))){
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
    if (pResolver)
        delete pResolver;
}

IP::IP()
{
    m_ip = 0;
    m_host = NULL;
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
    if (m_host)
        delete[] m_host;
}

void IP::set(unsigned long ip, const char *host)
{
    bool bResolve = false;
    if (ip != m_ip){
        m_ip = ip;
        if (m_host){
            delete[] m_host;
            m_host = NULL;
        }
        bResolve = true;
    }
    if (host && *host){
        if (m_host){
            if (!strcmp(m_host, host))
                return;
            delete[] m_host;
            m_host = NULL;
        }
        m_host = new char[strlen(host) + 1];
        strcpy(m_host, host);
    }
    if (bResolve && m_host)
        resolve();
}

void IP::resolve()
{
    if (m_host)
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
    string m_host;
    if (resolver->hostNames().count())
        m_host = static_cast<string>(resolver->hostNames().first().toLatin1());
    struct in_addr inaddr;
    inaddr.s_addr = m_addr;
    log(L_DEBUG, "Resolver ready %s %s", inet_ntoa(inaddr), m_host.c_str());
#if COMPAT_QT_VERSION >= 0x030000
    delete resolver;
    resolver = NULL;
#endif
    for (list<IP*>::iterator it = queue.begin(); it != queue.end(); ){
        if ((*it)->ip() != m_addr){
            ++it;
            continue;
        }
        (*it)->set((*it)->ip(), m_host.c_str());
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
#if COMPAT_QT_VERSION >= 0x030000
    if (resolver)
        delete resolver;
    resolver = new Q3Dns(QHostAddress(htonl(m_addr)), Q3Dns::Ptr);
    connect(resolver, SIGNAL(resultsReady()), this, SLOT(resolve_ready()));
#else
    resolver->setLabel(QHostAddress(htonl(m_addr)));
#endif
}

}

#ifndef _MSC_VER
#include "sockfactory.moc"
#endif
