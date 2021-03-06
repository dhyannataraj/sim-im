/***************************************************************************
                          proxy.h  -  description
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

#ifndef _PROXY_H
#define _PROXY_H

#include "socket.h"

class Proxy : public SocketNotify, public Socket
{
public:
    Proxy();
    ~Proxy();
    Socket *socket() { return sock; }
    virtual int read(char *buf, unsigned int size);
    virtual void write(const char *buf, unsigned int size);
    void setSocket(Socket*);
    virtual void close();
    virtual unsigned long localHost();
    virtual void pause(unsigned);
protected:
    virtual void write_ready();
    virtual void error_state(SocketError);
    virtual void proxy_connect_ready();
    void read(unsigned size, unsigned minsize=0);
    void write();
    Socket *sock;
    Buffer bOut;
    Buffer bIn;
};

class SOCKS4_Proxy : public Proxy
{
public:
    SOCKS4_Proxy(const char *host, unsigned short port);
    virtual void connect(const char *host, int port);
protected:
    virtual void connect_ready();
    virtual void read_ready();
    string m_host;
    unsigned short m_port;
    string m_connectHost;
    unsigned short m_connectPort;
    enum State
    {
        None,
        Connect,
        WaitConnect
    };
    State state;
};

class SOCKS5_Proxy : public Proxy
{
public:
    SOCKS5_Proxy(const char *host, unsigned short port, const char *user, const char *passwd);
    virtual void connect(const char *host, int port);
protected:
    virtual void connect_ready();
    virtual void read_ready();
    string m_host;
    unsigned short m_port;
    string m_user;
    string m_passwd;
    string m_connectHost;
    unsigned short m_connectPort;
    enum State
    {
        None,
        Connect,
        WaitAnswer,
        WaitAuth,
        WaitConnect
    };
    State state;
    void send_connect();
};

class HTTPS_Proxy : public Proxy
{
public:
    HTTPS_Proxy(const char *host, unsigned short port, const char *user, const char *passwd);
    virtual void connect(const char *host, int port);
protected:
    virtual void connect_ready();
    virtual void read_ready();
    string m_host;
    unsigned short m_port;
    string m_user;
    string m_passwd;
    string m_connectHost;
    unsigned short m_connectPort;
    enum State
    {
        None,
        Connect,
        WaitConnect,
        WaitEmpty
    };
    State state;
    bool readLine(string &s);
};

#endif
