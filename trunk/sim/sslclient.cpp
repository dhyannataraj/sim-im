/***************************************************************************
                          sslclient.cpp  -  description
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ENABLE_OPENSSL

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <openssl/md5.h>

#include "log.h"
#include "misc.h"
#include "socket.h"

namespace SIM
{

static bool bInit = false;

static void initLib()
{
    if (!bInit){
        SSL_load_error_strings();
        SSL_library_init();
        bInit = true;
    }
}

SSLClient::SSLClient(Socket *_sock)
{
    sock = _sock;
    sock->setNotify(this);
    mpSSL = NULL;
    mrBIO = NULL;
    mwBIO = NULL;
    mpCTX = NULL;
    m_bSecure = false;
}

SSLClient::~SSLClient()
{
    clear();
    if (sock){
        sock->close();
        delete sock;
    }
}

void SSLClient::clear()
{
    if (mpSSL != NULL)
        SSL_free(mpSSL);
    if (mpCTX != NULL)
        SSL_CTX_free(mpCTX);
    mpSSL = NULL;
    mpCTX = NULL;
    mrBIO = NULL;
    mwBIO = NULL;
    m_bSecure = false;
}

bool SSLClient::initBIO()
{
    mrBIO = BIO_new(BIO_s_mem());
    mwBIO = BIO_new(BIO_s_mem());
    if ((mpSSL == NULL) || (mrBIO == NULL) || (mwBIO == NULL)){
        log(L_WARN, "SSL error");
        return false;
    }
    SSL_set_bio(mpSSL, mrBIO, mwBIO);
    SSL_set_mode(mpSSL, SSL_MODE_AUTO_RETRY);
    return true;
}

bool SSLClient::init()
{
    initLib();
    if (!initSSL())
        return false;
    return initBIO();
}

void SSLClient::process(bool bInRead, bool bWantRead)
{
    for (;;){
        if (!bWantRead){
            switch (state){
            case SSLWrite:
                write();
                break;
            case SSLConnect:
                connect();
                break;
            case SSLAccept:
                accept();
                break;
            case SSLShutdown:
                shutdown();
                break;
            case SSLConnected:
                if (!bInRead && (SSL_pending(mpSSL) > 0))
                    notify->read_ready();
                break;
            }
        }
        char b[2048];
        int i = BIO_read(mwBIO, b, sizeof(b));
        if (i == 0) return;
        if (i > 0){
            sock->write(b, i);
            continue;
        }
        if (i < 0){
            if (!BIO_should_retry(mwBIO))
                notify->error_state(I18N_NOOP("SSL write error"));
            return;
        }
    }
}

void SSLClient::connect()
{
    if (mpSSL == NULL){
        notify->error_state(I18N_NOOP("SSL connect error"));
        return;
    }
    int i = SSL_connect(mpSSL);
    int j = SSL_get_error(mpSSL, i);
    if (j == SSL_ERROR_NONE)
    {
        m_bSecure = true;
        state = SSLConnected;
        notify->connect_ready();
        return;
    }
    switch (j)
    {
    case SSL_ERROR_SSL: {
        char errStr[200];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, errStr, sizeof(errStr)-1);
        log(L_WARN, "SSL: SSL_connect error = %lx (%s)", err, errStr);
        ERR_clear_error();
        notify->error_state(errStr, err);
        return;
    }
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_READ:
        state = SSLConnect;
        return;
    default:
        log(L_DEBUG, "SSL: SSL_connect error %d, SSL_%d", i, j);
        notify->error_state(I18N_NOOP("SSL connect error"));
    }
}

void SSLClient::shutdown()
{
    if (mpSSL == NULL){
        notify->error_state(I18N_NOOP("SSL shutdown error"));
        return;
    }
    int i = SSL_shutdown(mpSSL);
    int j = SSL_get_error(mpSSL, i);
    if (j == SSL_ERROR_NONE)
    {
        SSL_free(mpSSL);
        mpSSL = NULL;
        m_bSecure = false;
        return;
    }
    switch (j)
    {
    case SSL_ERROR_SSL: {
        char errStr[200];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, errStr, sizeof(errStr)-1);
        log(L_WARN, "SSL: SSL_shutdown error = %lx (%s)", err, errStr);
        ERR_clear_error();
        notify->error_state(errStr, err);
        return;
    }
    case SSL_ERROR_SYSCALL: {
        log(errno ? L_WARN : L_DEBUG, "SSL: SSL_shutdown errno: = %d ", errno);
        return;
    }
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
        state = SSLShutdown;
        return;
    default:
        log(L_DEBUG, "SSL: SSL_shutdown error %d, SSL_%d", i, j);
        notify->error_state(I18N_NOOP("SSL shutdown error"));
    }
}

void SSLClient::accept()
{
    if (mpSSL == NULL){
        notify->error_state(I18N_NOOP("SSL accept error"));
        return;
    }
    int i = SSL_accept(mpSSL);
    int j = SSL_get_error(mpSSL, i);
    if (j == SSL_ERROR_NONE)
    {
        m_bSecure = true;
        notify->connect_ready();
        state = SSLConnected;
        return;
    }
    switch (j)
    {
    case SSL_ERROR_SSL: {
        char errStr[200];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, errStr, sizeof(errStr)-1);
        log(L_WARN, "SSL: SSL_accept error = %lx (%s)", err, errStr);
        ERR_clear_error();
        notify->error_state(errStr, err);
        return;
    }
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
        state = SSLAccept;
        return;
    default:
        log(L_DEBUG, "SSL: SSL_accept error %d, SSL_%d", i, j);
        notify->error_state(I18N_NOOP("SSL accept error"));
    }
}

int SSLClient::read(char *buf, unsigned int size)
{
    if (state != SSLConnected) return 0;
    int nBytesReceived = SSL_read(mpSSL, buf, size);
    int tmp = SSL_get_error(mpSSL, nBytesReceived);
    switch (tmp){
    case SSL_ERROR_NONE:
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_X509_LOOKUP:
        break;
    case SSL_ERROR_SSL: {
        char errStr[200];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, errStr, sizeof(errStr)-1);
        log(L_WARN, "SSL: SSL_read error = %lx (%s)", err, errStr);
        ERR_clear_error();
        notify->error_state(errStr, err);
        return -1;
    }
    default:
        log(L_DEBUG, "SSL: SSL_read error %d, SSL_%d", nBytesReceived, tmp);
        notify->error_state(I18N_NOOP("SSL read error"));
        return -1;
    }
    process(true);
    if (nBytesReceived < 0) nBytesReceived = 0;
    return nBytesReceived;
}

void SSLClient::write(const char *buf, unsigned int size)
{
    wBuffer.pack(buf, size);
    state = SSLWrite;
    process();
}

void SSLClient::write()
{
    int nBytesSend = SSL_write(mpSSL, wBuffer.data(), wBuffer.size());
    int tmp = SSL_get_error(mpSSL, nBytesSend);
    bool bWantRead = false;
    switch (tmp){
    case SSL_ERROR_NONE:
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_X509_LOOKUP:
        break;
    case SSL_ERROR_WANT_READ:
        bWantRead = true;
        break;
    case SSL_ERROR_SSL: {
        char errStr[200];
        unsigned long err = ERR_get_error();
        ERR_error_string_n(err, errStr, sizeof(errStr)-1);
        log(L_WARN, "SSL: SSL_write error = %lx (%s)", err, errStr);
        ERR_clear_error();
        notify->error_state(errStr, err);
        return;
    }
    default:
        log(L_DEBUG, "SSL: SSL_write error %d, SSL_%d", nBytesSend, tmp);
        notify->error_state(I18N_NOOP("SSL write error"));
        return;
    }
    if (nBytesSend > 0)
        wBuffer.incReadPos(nBytesSend);
    if (wBuffer.readPos() == wBuffer.writePos()){
        wBuffer.init(0);
        state = SSLConnected;
    }
    process(false, true);
}

void SSLClient::connect(const QString &host, unsigned short port)
{
    sock->connect(host, port);
}

void SSLClient::close()
{
    if (mpSSL)
        shutdown();
    clear();
    sock->close();
}

unsigned long SSLClient::localHost()
{
    return sock->localHost();
}

void SSLClient::pause(unsigned n)
{
    sock->pause(n);
}

void SSLClient::connect_ready()
{
    if (!init())
        if (notify)
            notify->error_state(I18N_NOOP("SSL init error"));
    connect();
}

void SSLClient::read_ready()
{
    for (;;){
        char b[2048];
        int n = sock->read(b, sizeof(b));
        if (n == -1){
            if (notify)
                notify->error_state(I18N_NOOP("SSL read error"));
            return;
        }
        if (n == 0) break;
        n = BIO_write(mrBIO, b, n);
        if (n == -1)
            if (notify)
                notify->error_state(I18N_NOOP("SSL read error"));
        process();
    }
    if (state == SSLConnected)
        if (notify) notify->read_ready();
}

void SSLClient::write_ready()
{
}

void SSLClient::error_state(const QString &err, unsigned code)
{
    if (notify)
        notify->error_state(err, code);
}

void SSLClient::setSocket(Socket *s)
{
    sock = s;
}

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

bool SSLClient::initTLS1(bool bDH, bool forceSSL)
{
    mpCTX = SSL_CTX_new(forceSSL ? SSLv23_method() : TLSv1_method());
    if (mpCTX == NULL)
        return false;
    if (bDH){
#if OPENSSL_VERSION_NUMBER >= 0x00905000L
        SSL_CTX_set_cipher_list(mpCTX, "ADH:@STRENGTH");
#else
        SSL_CTX_set_cipher_list(mpCTX, "ADH");
#endif
        DH *dh = get_dh512();
        SSL_CTX_set_tmp_dh(mpCTX, dh);
        DH_free(dh);
    }
    mpSSL = SSL_new(mpCTX);
    if(!mpSSL)
        return false;
    return true;
}

QByteArray md5(const char *str, int size)
{
    MD5_CTX c;
    unsigned char md[MD5_DIGEST_LENGTH];
    if (size < 0)
        size = strlen(str);
    MD5_Init(&c);
    MD5_Update(&c, str, size);
    MD5_Final(md, &c);
    QByteArray ba;
    ba.duplicate((char*)md, sizeof(md));
    return ba;
}

QByteArray sha1(const char *str, int size)
{
    unsigned char digest[20];
    SHA_CTX ctx;
    if (size < 0)
        size = strlen(str);
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, str, size);
    SHA1_Final(digest, &ctx);
    QByteArray ba;
    ba.duplicate((char*)digest, sizeof(digest));
    return ba;
}

} // namespace SIM

#endif
