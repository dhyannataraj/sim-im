/***************************************************************************
                          httppool.cpp  -  description
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
#include "fetch.h"

class JabberHttpPool : public Socket, public FetchClient
{
public:
    JabberHttpPool(const char *url);
    ~JabberHttpPool();
    virtual void connect(const char *host, unsigned short port);
    virtual int  read(char *buf, unsigned size);
    virtual void write(const char *buf, unsigned size);
    virtual void close();
    virtual Mode mode() { return Web; }
protected:
    string getKey();
    virtual bool done(unsigned code, Buffer &data, const char *headers);
    Buffer readData;
    Buffer writeData;
    string m_url;
    string m_key;
    string m_seed;
    string m_cookie;
    virtual unsigned long localHost();
    virtual void pause(unsigned);
};

// ______________________________________________________________________________________

JabberHttpPool::JabberHttpPool(const char *url)
{
    m_url = url;
    m_cookie = "0";
#ifdef USE_OPENSSL
    /*
        Buffer k;
        for (unsigned i = 0; i < 48; i++){
            char c = get_random() & 0xFF;
            k.pack(&c, 1);
        }
        Buffer to;
        to.toBase64(k);
        m_seed.append(to.data(), to.size());
    */
    m_seed = "foo";
#endif
}

JabberHttpPool::~JabberHttpPool()
{
}

string JabberHttpPool::getKey()
{
#ifdef USE_OPENSSL
    if (m_key.empty()){
        m_key = m_seed;
        return m_key;
    }
	string digest = sha1(m_key.c_str());
    Buffer b;
    b.pack(digest.c_str(), digest.length());
    Buffer r;
    r.toBase64(b);
    m_key = "";
    m_key.append(r.data(), r.size());
    return m_key;
#else
    return "";
#endif
}

int JabberHttpPool::read(char *buf, unsigned size)
{
    unsigned tail = readData.size() - readData.readPos();
    if (size > tail) size = tail;
    if (size == 0) return 0;
    readData.unpack(buf, size);
    if (readData.readPos() == readData.size())
        readData.init(0);
    return size;
}

void JabberHttpPool::write(const char *buf, unsigned size)
{
    writeData.pack(buf, size);
    if (!isDone())
        return;
    Buffer *packet = new Buffer;
    string key = getKey();
    *packet << m_cookie.c_str();
#ifdef USE_OPENSSL
    *packet << ";" << key.c_str();
#endif
    *packet << ",";
    log(L_DEBUG, "%s;%s,", m_cookie.c_str(), key.c_str());
    packet->pack(writeData.data(), writeData.writePos());
    char headers[] = "Content-Type: application/x-www-form-urlencoded";
    fetch(m_url.c_str(), headers, packet);
    writeData.init(0);
}

void JabberHttpPool::close()
{
    writeData.init(0);
    stop();
}

void JabberHttpPool::connect(const char*, unsigned short)
{
    if (notify)
        notify->connect_ready();
}

bool JabberHttpPool::done(unsigned code, Buffer &data, const char *headers)
{
    if (code != 200){
        log(L_DEBUG, "HTTP result %u", code);
        error("Bad result");
        return false;
    }
    string cookie;
    for (const char *p = headers; *p; p += strlen(p) + 1){
        string h = p;
        if (getToken(h, ':') != "Set-Cookie")
            continue;
        while (!h.empty()){
            string part = trim(getToken(h, ';').c_str());
            if (getToken(part, '=') == "ID")
                cookie = part;
        }
        if (!cookie.empty())
            break;
    }
    m_cookie = cookie;
    int err_code = atol(getToken(cookie, ':').c_str());
    if (cookie == "0"){
        const char *err = "Unknown poll error";
        switch (err_code){
        case -1:
            err = "Server Error";
            break;
        case -2:
            err = "Bad Request";
            break;
        case -3:
            err = "Key Sequence Error";
            break;
        }
        error(err);
        return false;
    }
    readData.pack(data.data(), data.writePos());
    if (notify)
        notify->read_ready();
    return false;
}

unsigned long JabberHttpPool::localHost()
{
    return 0;
}

void JabberHttpPool::pause(unsigned)
{
}

Socket *JabberClient::createSocket()
{
    m_bHTTP = getUseHTTP() && *getURL();
    if (m_bHTTP)
        return new JabberHttpPool(getURL());
    return NULL;
}


