/***************************************************************************
                          socket.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : shutoff@mail.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef WIN32
#if _MSC_VER > 1020
#pragma warning(disable:4530)
#endif
#endif

#include "socket.h"
#include "proxy.h"
#include "log.h"

Socket::Socket()
{
    notify = NULL;
}

ServerSocket::ServerSocket()
{
    notify = NULL;
}

ClientSocket::ClientSocket(ClientSocketNotify *n, SocketFactory *f)
{
    notify = n;
    factory = f;
    bRawMode = false;
    m_sock = f->createSocket();
    m_sock->setNotify(this);
    m_proxy = NULL;
    bInProcess = false;
    bClosed = false;
}

ClientSocket::~ClientSocket()
{
    setProxy(NULL);
    if (m_sock) delete m_sock;
}

void ClientSocket::close()
{
    setProxy(NULL);
    m_sock->close();
    bClosed = true;
}

void ClientSocket::setProxy(Proxy *p)
{
    if (m_proxy){
        setSocket(m_proxy->socket());
        delete m_proxy;
        m_proxy = NULL;
    }
    if (p){
        m_proxy = p;
        m_proxy->setSocket(m_sock);
        setSocket(m_proxy);
    }
}

void ClientSocket::setProxyConnected()
{
    setProxy(NULL);
}

void ClientSocket::error(SocketError err)
{
    switch (err){
    case ErrorSocket:
        log(L_WARN, "Socket error");
        break;
    case ErrorConnect:
        log(L_WARN, "Connect error");
        break;
    case ErrorRead:
        log(L_WARN, "Read error");
        break;
    case ErrorWrite:
        log(L_WARN, "Write error");
        break;
    case ErrorConnectionClosed:
        log(L_WARN, "Connection closed");
        break;
    case ErrorProtocol:
        log(L_WARN, "Protocol error");
        break;
    case ErrorProxyAuth:
        log(L_WARN, "Proxy auth error");
        break;
    case ErrorProxyConnect:
        log(L_WARN, "Proxy connect error");
        break;
    case ErrorNone:
	return;
    }
    bInProcess = true;
    notify->error_state(err);
    bInProcess = false;
}

void ClientSocket::connect(const char *host, int port)
{
    m_sock->connect(host, port);
}

void ClientSocket::write()
{
    if (writeBuffer.size() == 0) return;
    m_sock->write(writeBuffer.Data(0), writeBuffer.size());
    writeBuffer.init(0);
}

bool ClientSocket::created()
{
    return (m_sock != NULL);
}

void ClientSocket::connect_ready()
{
    bClosed = false;
    notify->connect_ready();
}

void ClientSocket::setRaw(bool mode)
{
    bRawMode = mode;
    read_ready();
    if (mode) readBuffer.init(0);
}

void ClientSocket::processPacket()
{
    bInProcess = true;
    err = ErrorNone;
    notify->packet_ready();
    if (err != ErrorNone)
        notify->error_state(err);
    bInProcess = false;
}

void ClientSocket::read_ready()
{
    if (bRawMode){
        for (;;){
            char b[2048];
            int readn = m_sock->read(b, sizeof(b));
            if (readn == 0) break;
            readBuffer.setWritePos(readBuffer.writePos() + readn);
            if (readn < (int)sizeof(b)) break;
        }
        processPacket();
        return;
    }
    for (;;){
	if (bClosed) break;
        int readn = m_sock->read(readBuffer.Data(readBuffer.writePos()),
                                 readBuffer.size() - readBuffer.writePos());
        if (readn < 0){
            notify->error_state(ErrorRead);
            return;
        }
        if (readn == 0) break;
        readBuffer.setWritePos(readBuffer.writePos() + readn);
        if (readBuffer.writePos() < readBuffer.size()) break;
        processPacket();
    }
}

void ClientSocket::write_ready()
{
    notify->write_ready();
}

void ClientSocket::error_state(SocketError _err)
{
    if (bInProcess){
        err = _err;
        return;
    }
    bInProcess = true;
    notify->error_state(_err);
    bInProcess = false;
}

void ClientSocket::remove()
{
    m_sock->close();
    factory->removedNotifies.push_back(this);
}

unsigned long ClientSocket::localHost()
{
    return m_sock->localHost();
}

void ClientSocket::pause(unsigned n)
{
    m_sock->pause(n);
}

void ClientSocket::setSocket(Socket *s)
{
    m_sock = s;
    s->setNotify(this);
}

void SocketFactory::idle()
{
    for (list<SocketNotify*>::iterator itNot = removedNotifies.begin(); itNot != removedNotifies.end(); ++itNot)
        delete (*itNot);
    removedNotifies.clear();
}

