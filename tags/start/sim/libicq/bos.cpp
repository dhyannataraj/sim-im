/***************************************************************************
                          bos.cpp  -  description
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

const unsigned short ICQ_SNACxBOS_REQUESTxRIGHTS     = 0x0002;
const unsigned short ICQ_SNACxBOS_RIGHTSxGRANTED     = 0x0003;
const unsigned short ICQ_SNACxBOS_ADDxVISIBLExLIST   = 0x0005;
const unsigned short ICQ_SNACxBOS_REMxVISIBLExLIST   = 0x0006;
const unsigned short ICQ_SNACxBOS_ADDxINVISIBLExLIST = 0x0007;
const unsigned short ICQ_SNACxBOS_REMxINVISIBLExLIST = 0x0008;

void ICQClient::snac_bos(unsigned short type, unsigned short)
{
    switch (type){
    case ICQ_SNACxBOS_RIGHTSxGRANTED:
        log(L_DEBUG, "BOS rights granted");
        break;
    default:
        log(L_WARN, "Unknown bos family type %04X", type);
    }
}

void ICQClient::sendVisibleList()
{
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_ADDxVISIBLExLIST);
    list<ICQUser*>::iterator it;
    for (it = contacts.users.begin(); it != contacts.users.end(); it++){
        if (((*it)->Uin() < UIN_SPECIAL) && (*it)->inVisible()){
            writeBuffer.packUin((*it)->Uin());
        }
    }
    sendPacket();
}

void ICQClient::sendInvisibleList()
{
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_ADDxINVISIBLExLIST);
    list<ICQUser*>::iterator it;
    for (it = contacts.users.begin(); it != contacts.users.end(); it++){
        if (((*it)->Uin() < UIN_SPECIAL) && (*it)->inInvisible()){
            writeBuffer.packUin((*it)->Uin());
        }
    }
    sendPacket();
}

void ICQClient::addToVisibleList(unsigned long uin)
{
    if (uin >= UIN_SPECIAL) return;
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_ADDxVISIBLExLIST);
    writeBuffer.packUin(uin);
    sendPacket();
}

void ICQClient::addToInvisibleList(unsigned long uin)
{
    if (uin >= UIN_SPECIAL) return;
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_ADDxINVISIBLExLIST);
    writeBuffer.packUin(uin);
    sendPacket();
}

void ICQClient::removeFromVisibleList(unsigned long uin)
{
    if (uin >= UIN_SPECIAL) return;
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_REMxVISIBLExLIST);
    writeBuffer.packUin(uin);
    sendPacket();
}

void ICQClient::removeFromInvisibleList(unsigned long uin)
{
    if (uin >= UIN_SPECIAL) return;
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_REMxINVISIBLExLIST);
    writeBuffer.packUin(uin);
    sendPacket();
}

void ICQClient::bosRequest()
{
    snac(ICQ_SNACxFAM_BOS, ICQ_SNACxBOS_REQUESTxRIGHTS);
    sendPacket();
}


