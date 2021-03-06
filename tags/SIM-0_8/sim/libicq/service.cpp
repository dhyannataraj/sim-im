/***************************************************************************
                          service.cpp  -  description
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

#include "icqclient.h"
#include "log.h"

#include <time.h>
#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

const unsigned short ICQ_SNACxSRV_READYxCLIENT  = 0x0002;
const unsigned short ICQ_SNACxSRV_READYxSERVER	= 0x0003;
const unsigned short ICQ_SNACxSRV_REQxRATExINFO = 0x0006;
const unsigned short ICQ_SNACxSRV_RATExINFO     = 0x0007;
const unsigned short ICQ_SNACxSRV_RATExACK      = 0x0008;
const unsigned short ICQ_SNACxSRV_RATExCHANGE   = 0x000A;
const unsigned short ICQ_SNACxSRV_GETxUSERxINFO = 0x000E;
const unsigned short ICQ_SNACxSRV_NAMExINFO     = 0x000F;
const unsigned short ICQ_SNACxSRV_NAMExINFOxACK = 0x0011;
const unsigned short ICQ_SNACxSRV_MOTD          = 0x0013;
const unsigned short ICQ_SNACxSRV_IMxICQ 	= 0x0017;
const unsigned short ICQ_SNACxSRV_ACKxIMxICQ    = 0x0018;
const unsigned short ICQ_SNACxSRV_SETxSTATUS    = 0x001E;

void ICQClient::snac_service(unsigned short type, unsigned short)
{
    switch (type){
    case ICQ_SNACxSRV_RATExCHANGE:
        break;
    case ICQ_SNACxSRV_RATExINFO:
        log(L_DEBUG, "Rate info");
        snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_RATExACK);
        sock->writeBuffer << 0x00010002L << 0x00030004L << 0x0005;
        sendPacket();
        snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_GETxUSERxINFO);
        sendPacket();
        listsRequest();
        locationRequest();
        buddyRequest();
        messageRequest();
        bosRequest();
        break;
    case ICQ_SNACxSRV_MOTD:
        log(L_DEBUG, "Motd");
        snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_REQxRATExINFO);
        sendPacket();
        break;
    case ICQ_SNACxSRV_ACKxIMxICQ:
        log(L_DEBUG, "Ack im icq");
        break;
    case ICQ_SNACxSRV_NAMExINFO:{
            unsigned long uin = sock->readBuffer.unpackUin();
            if (uin == 0){
                char n;
                sock->readBuffer >> n;
                sock->readBuffer.incReadPos(n);
                uin = sock->readBuffer.unpackUin();
            }
            if (uin != owner->Uin){
                log(L_WARN, "No my name info (%lu)", uin);
                break;
            }
            sock->readBuffer.incReadPos(4);
            TlvList tlv(sock->readBuffer);
            Tlv *tlvIP = tlv(0x000A);
            if (tlvIP)
                owner->IP = htonl((unsigned long)(*tlvIP));
            log(L_DEBUG, "Name info");
            bool bSend = true;
            if (needPhonebookUpdate){
                sendInfoUpdate();
                needPhonebookUpdate = false;
                bSend = false;
            }
            if (needPhoneStatusUpdate){
                sendPhoneStatus();
                needPhoneStatusUpdate = false;
                bSend = false;
            }
            if (needShareUpdate){
                sendShareUpdate();
                needShareUpdate = false;
                bSend = false;
            }
            break;
        }
    case ICQ_SNACxSRV_READYxSERVER:
        log(L_DEBUG, "Server ready");
        snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_IMxICQ);
        sock->writeBuffer
        << 0x00010003L
        << 0x00130002L
        << 0x00020001L
        << 0x00030001L
        << 0x00150001L
        << 0x00040001L
        << 0x00060001L
        << 0x00090001L
        << 0x000a0001L
        << 0x000b0001L;
        sendPacket();
        break;
    default:
        log(L_WARN, "Unknown service family type %04X", type);
    }
}

void ICQClient::sendLogonStatus()
{
    owner->IP = 0;
    owner->RealIP = htonl(sock->localHost());
    log(L_DEBUG, "RealIP: %X", owner->RealIP);

    log(L_DEBUG, "Logon status");
    if (owner->inInvisible) sendVisibleList();
    sendContactList();

    unsigned long now;
    time((time_t*)&now);
    if (owner->PhoneBookTime == 0) owner->PhoneBookTime = now;
    if (owner->PhoneStatusTime == 0) owner->PhoneStatusTime = now;
    if (owner->InfoUpdateTime == 0) owner->InfoUpdateTime = now;

    Buffer directInfo(25);
    fillDirectInfo(directInfo);

    snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_SETxSTATUS);
    sock->writeBuffer.tlv(0x0006, fullStatus(m_nLogonStatus));
    sock->writeBuffer.tlv(0x0008, 0);
    sock->writeBuffer.tlv(0x000C, directInfo);

    sendPacket();

    if (!owner->inInvisible) sendInvisibleList();
    m_state = Logged;
    owner->OnlineTime = now;

    owner->uStatus = m_nLogonStatus & 0xFF;
    ICQEvent e(EVENT_STATUS_CHANGED, owner->Uin);
    process_event(&e);

    processMsgQueue();

    lastInfoRequestTime = now;
    lastPhoneRequestTime = now;
    lastResponseRequestTime = now;
}

void ICQClient::setInvisible(bool bInvisible)
{
    if (owner->inInvisible == bInvisible) return;
    owner->inInvisible = bInvisible;
    if (m_state != Logged) return;
    if (owner->inInvisible) sendVisibleList();
    snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_SETxSTATUS);
    sock->writeBuffer.tlv(0x0006, fullStatus(owner->uStatus));
    sendPacket();
    if (!owner->inInvisible) sendInvisibleList();
    ICQEvent e(EVENT_STATUS_CHANGED, owner->Uin);
    process_event(&e);
}

void ICQClient::sendStatus(unsigned long status)
{
    snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_SETxSTATUS);
    sock->writeBuffer.tlv(0x0006, fullStatus(status));
    sendPacket();
    owner->uStatus = status & 0xFF;
    ICQEvent e(EVENT_STATUS_CHANGED, owner->Uin);
    process_event(&e);
}

void ICQClient::sendClientReady()
{
    snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_READYxCLIENT);
    sock->writeBuffer
    << 0x00010003L << 0x0110047BL
    << 0x00130002L << 0x0110047BL
    << 0x00020001L << 0x0101047BL
    << 0x00030001L << 0x0110047BL
    << 0x00150001L << 0x0110047BL
    << 0x00040001L << 0x0110047BL
    << 0x00060001L << 0x0110047BL
    << 0x00090001L << 0x0110047BL
    << 0x000A0001L << 0x0110047BL
    << 0x000B0001L << 0x0110047BL;

    sendPacket();
}

void ICQClient::fillDirectInfo(Buffer &directInfo)
{
    directInfo
    << (unsigned long)htonl(owner->RealIP)
    << (unsigned short)0
    << (unsigned short)(listener ? listener->port() : 0)

    << (char)0x01	// Mode
    << (char)0x00
    << (char)ICQ_TCP_VERSION;

    directInfo
    << owner->DCcookie
    << 0x00000050L
    << 0x00000003L
    << owner->InfoUpdateTime
    << owner->PhoneStatusTime
    << owner->PhoneBookTime
    << (unsigned short) 0x0000;
}

void ICQClient::sendUpdate(Buffer &b)
{
    Buffer directInfo(25);
    fillDirectInfo(directInfo);

    snac(ICQ_SNACxFAM_SERVICE, ICQ_SNACxSRV_SETxSTATUS);
    sock->writeBuffer.tlv(0x0006, fullStatus(owner->uStatus));
    sock->writeBuffer.tlv(0x000C, directInfo);

    sock->writeBuffer.tlv(0x11, b);
    sock->writeBuffer.tlv(0x12, (unsigned short)0x0000);
    sendPacket();
}

bool ICQClient::updatePhoneBook()
{
    if (m_state != Logged) return false;
    unsigned long now;
    time((time_t*)&now);
    Buffer b;
    b << (char)2;
    b.pack((unsigned long)now);
    b << (unsigned short)0x0200 << 0x01000100L;
    b.pack((char*)PHONEBOOK_SIGN, 0x10);
    b << (unsigned short)0;
    b.pack((unsigned long)now);
    b << (char)0;
    owner->PhoneBookTime = now;
    sendUpdate(b);
    needPhonebookUpdate = true;
    return true;
}

void ICQClient::sendInfoUpdate()
{
    time_t now;
    time((time_t*)&now);
    Buffer b;
    b << (char)1;
    b.pack((unsigned long)now);
    sendUpdate(b);
    owner->InfoUpdateTime = now;
    ICQEvent e(EVENT_INFO_CHANGED, owner->Uin);
    sendMessageRequest();
    process_event(&e);
}

bool ICQClient::updatePhoneStatus()
{
    if (m_state != Logged) return false;
    unsigned long now;
    time((time_t*)&now);
    owner->PhoneStatusTime = now;
    Buffer b;
    b << (char)3;
    b.pack(owner->PhoneStatusTime);
    b << (unsigned short)0x0000 << 0x01000100L;
    b.pack((const char*)PHONEBOOK_SIGN, 0x10);
    b << (char)0x02 << (unsigned short)0x0001 << owner->PhoneState << (char)0 << (unsigned short) 0;
    b.pack(owner->PhoneStatusTime);
    b << (unsigned short)0 << 0x00000100L;
    sendUpdate(b);
    needPhoneStatusUpdate = true;
    ICQEvent e(EVENT_INFO_CHANGED, owner->Uin);
    process_event(&e);
    return true;
}

void ICQClient::setShare(bool bState)
{
    if (bState == ShareOn) return;
    ShareOn = bState;
    time_t now;
    time(&now);
    Buffer b;
    b << (char)3;
    b.pack(now);
    b.pack((unsigned short)0);
    b.pack((unsigned short)1);
    b.pack((unsigned short)1);
    b.pack((char*)SHARED_FILES_SIGN, 16);
    b << (char)4 << (unsigned short)1;
    b.pack((unsigned long)(bState ? 1 : 0));
    b.pack(now);
    b.pack((unsigned long)0);
    b.pack((unsigned short)1);
    sendUpdate(b);
    needShareUpdate = true;
}
