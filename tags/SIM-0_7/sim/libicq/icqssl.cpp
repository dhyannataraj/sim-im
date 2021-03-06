/***************************************************************************
                          icqssl.cpp  -  description
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

#include "icqssl.h"
#include "log.h"

#ifdef USE_OPENSSL

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *gSSL_CTX = NULL;

void ssl_info_callback(SSL *s, int where, int ret)
{
    const char *str;
    int w;

    w = where & ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT) str="SSL_connect";
    else if (w & SSL_ST_ACCEPT) str="SSL_accept";
    else str="undefined";

    if (where & SSL_CB_LOOP)
    {
        log(L_DEBUG, "SSL: %s", SSL_state_string_long(s));
    }
    else if (where & SSL_CB_ALERT)
    {
        str=(where & SSL_CB_READ)?"read":"write";
        log(L_DEBUG, "SSL: SSL3 alert %s:%s:%s", str,
            SSL_alert_type_string_long(ret),
            SSL_alert_desc_string_long(ret));
    }
    else if (where & SSL_CB_EXIT)
    {
        if (ret == 0)
            log(L_DEBUG, "SSL: %s:failed in %s",
                str,SSL_state_string_long(s));
        else if (ret < 0)
        {
            log(L_DEBUG, "SSL: %s:%s", str,SSL_state_string_long(s));
        }
    }
    else if (where & SSL_CB_ALERT)
    {
        str=(where & SSL_CB_READ)?"read":"write";
        log(L_DEBUG, "SSL: SSL3 alert %s:%s:%s", str,
            SSL_alert_type_string_long(ret),
            SSL_alert_desc_string_long(ret));
    }
    else if (where & SSL_CB_EXIT)
    {
        if (ret == 0)
            log(L_DEBUG, "SSL: %s:failed in %s",
                str,SSL_state_string_long(s));
        else if (ret < 0)
        {
            log(L_DEBUG, "SSL: %s:error in %s",
                str,SSL_state_string_long(s));
        }
    }
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

static void init()
{
    if (gSSL_CTX) return;
    SSL_load_error_strings();
    SSL_library_init();
    gSSL_CTX = SSL_CTX_new(TLSv1_method());
#if OPENSSL_VERSION_NUMBER >= 0x00905000L
    SSL_CTX_set_cipher_list(gSSL_CTX, "ADH:@STRENGTH");
#else
    SSL_CTX_set_cipher_list(gSSL_CTX, "ADH");
#endif
    SSL_CTX_set_info_callback(gSSL_CTX, (void (*)())ssl_info_callback);
    DH *dh = get_dh512();
    SSL_CTX_set_tmp_dh(gSSL_CTX, dh);
    DH_free(dh);
}

SSL *newSSL()
{
    init();
    return SSL_new(gSSL_CTX);
}

#endif

