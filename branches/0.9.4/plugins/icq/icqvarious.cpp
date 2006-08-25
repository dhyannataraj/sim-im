/***************************************************************************
                          icqvarious.cpp  -  description
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

#include "icqclient.h"

#include <time.h>
#include <stdio.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <qtimer.h>
#include "xml.h"

const unsigned short ICQ_SNACxVAR_ERROR             = 0x0001;
const unsigned short ICQ_SNACxVAR_REQxSRV           = 0x0002;
const unsigned short ICQ_SNACxVAR_DATA              = 0x0003;

const unsigned short ICQ_SRVxREQ_OFFLINE_MSG        = 0x3C00;
const unsigned short ICQ_SRVxREQ_ACK_OFFLINE_MSG    = 0x3E00;
const unsigned short ICQ_SRVxOFFLINE_MSG            = 0x4100;
const unsigned short ICQ_SRVxEND_OFFLINE_MSG        = 0x4200;
const unsigned short ICQ_SRVxREQ_MORE               = 0xD007;
const unsigned short ICQ_SRVxANSWER_MORE            = 0xDA07;

const unsigned short ICQ_SRVxREQ_FULL_INFO          = 0xB204;
const unsigned short ICQ_SRVxREQ_SHORT_INFO         = 0xBA04;
const unsigned short ICQ_SRVxREQ_OWN_INFO           = 0xD004;
const unsigned short ICQ_SRVxREQ_SEND_SMS           = 0x8214;
const unsigned short ICQ_SRVxREQ_WP_UIN				= 0x6905;
const unsigned short ICQ_SRVxREQ_WP_MAIL            = 0x7305;
const unsigned short ICQ_SRVxREQ_WP_FULL            = 0x5F05;
const unsigned short ICQ_SRVxREQ_CHANGE_PASSWD      = 0x2E04;
const unsigned short ICQ_SRVxREQ_PERMISSIONS        = 0x2404;
const unsigned short ICQ_SRVxREQ_XML_KEY            = 0x9808;

const unsigned short ICQ_SRVxGENERAL_INFO           = 0xC800;
const unsigned short ICQ_SRVxMORE_INFO              = 0xDC00;
const unsigned short ICQ_SRVxEMAIL_INFO             = 0xEB00;
const unsigned short ICQ_SRVxWORK_INFO              = 0xD200;
const unsigned short ICQ_SRVxABOUT_INFO             = 0xE600;
const unsigned short ICQ_SRVxINTERESTS_INFO         = 0xF000;
const unsigned short ICQ_SRVxBACKGROUND_INFO        = 0xFA00;
const unsigned short ICQ_SRVxUNKNOWN_INFO           = 0x0E01;

const unsigned short ICQ_SRVxREQ_MODIFY_MAIN        = 0xEA03;
const unsigned short ICQ_SRVxREQ_MODIFY_HOME        = 0xFD03;
const unsigned short ICQ_SRVxREQ_MODIFY_ABOUT       = 0x0604;
const unsigned short ICQ_SRVxREQ_MODIFY_WORK        = 0xF303;
const unsigned short ICQ_SRVxREQ_MODIFY_MORE        = 0xFD03;
const unsigned short ICQ_SRVxREQ_MODIFY_INTERESTS   = 0x1004;
const unsigned short ICQ_SRVxREQ_MODIFY_BACKGROUND  = 0x1A04;
const unsigned short ICQ_SRVxREQ_MODIFY_MAIL        = 0x0B04;

const unsigned short ICQ_SRVxREQ_PHONE_UPDATE       = 0x5406;
const unsigned short ICQ_SRVxREQ_SET_CHAT_GROUP     = 0x5807;
const unsigned short ICQ_SRVxREQ_RANDOM_CHAT        = 0x4E07;

const unsigned short ICQ_SRVxWP_SET                 = 0x3A0C;
const unsigned short ICQ_SRVxWP_SET_RESP            = 0x3F0C;

const unsigned short TLV_UIN						= 0x0136;
const unsigned short TLV_FIRST_NAME					= 0x0140;
const unsigned short TLV_LAST_NAME					= 0x014A;
const unsigned short TLV_NICK						= 0x0154;
const unsigned short TLV_EMAIL						= 0x015E;
const unsigned short TLV_AGE_RANGE					= 0x0168;
const unsigned short TLV_AGE						= 0x0172;
const unsigned short TLV_GENDER						= 0x017C;
const unsigned short TLV_LANGUAGE					= 0x0186;
const unsigned short TLV_CITY						= 0x0190;
const unsigned short TLV_STATE						= 0x019A;
const unsigned short TLV_COUNTRY					= 0x01A4;
const unsigned short TLV_WORK_COMPANY				= 0x01AE;
const unsigned short TLV_WORK_DEPARTMENT			= 0x01B8;
const unsigned short TLV_WORK_POSITION				= 0x01C2;
const unsigned short TLV_WORK_OCCUPATION			= 0x01CC;
const unsigned short TLV_AFFILATIONS				= 0x01D6;
const unsigned short TLV_INTERESTS					= 0x01EA;
const unsigned short TLV_PAST						= 0x01FE;
const unsigned short TLV_HOMEPAGE_CATEGORY			= 0x0212;
const unsigned short TLV_HOMEPAGE					= 0x0213;
const unsigned short TLV_KEYWORDS					= 0x0226;
const unsigned short TLV_SEARCH_ONLINE				= 0x0230;
const unsigned short TLV_BIRTHDAY					= 0x023A;
const unsigned short TLV_NOTES						= 0x0258;
const unsigned short TLV_STREET						= 0x0262;
const unsigned short TLV_ZIP						= 0x026D;
const unsigned short TLV_PHONE						= 0x0276;
const unsigned short TLV_FAX						= 0x0280;
const unsigned short TLV_CELLULAR					= 0x028A;
const unsigned short TLV_WORK_STREET				= 0x0294;
const unsigned short TLV_WORK_CITY					= 0x029E;
const unsigned short TLV_WORK_STATE					= 0x02A8;
const unsigned short TLV_WORK_COUNTRY				= 0x02B2;
const unsigned short TLV_WORK_ZIP					= 0x02BD;
const unsigned short TLV_WORK_PHONE					= 0x02C6;
const unsigned short TLV_WORK_FAX					= 0x02D0;
const unsigned short TLV_WORK_HOMEPAGE				= 0x02DA;
const unsigned short TLV_NEED_AUTH					= 0x02F8;
const unsigned short TLV_SHOW_WEB					= 0x030C;
const unsigned short TLV_TIMEZONE					= 0x0316;
const unsigned short TLV_ORIGINALLY_CITY			= 0x0320;
const unsigned short TLV_ORIGINALLY_STATE			= 0x032A;
const unsigned short TLV_ORIGINALLY_COUNTRY			= 0x0334;
const unsigned short TLV_RECV_ICQ_SPAM				= 0x0348;

const char SEARCH_STATE_OFFLINE  = 0;
const char SEARCH_STATE_ONLINE   = 1;
const char SEARCH_STATE_DISABLED = 2;

const unsigned INFO_REQUEST_TIMEOUT = 60;

class ServerRequest
{
public:
    ServerRequest(unsigned short id);
    virtual ~ServerRequest() {}
    unsigned short id() { return m_id; }
    virtual bool answer(Buffer&, unsigned short nSubType) = 0;
    virtual void fail(unsigned short error_code = 0);
protected:
    unsigned short m_id;
};

ServerRequest::ServerRequest(unsigned short id)
{
    m_id = id;
}

ServerRequest *ICQClient::findServerRequest(unsigned short id)
{
    for (list<ServerRequest*>::iterator it = varRequests.begin(); it != varRequests.end(); ++it){
        if ((*it)->id() == id)
            return *it;
    }
    return NULL;
}

void ServerRequest::fail(unsigned short)
{
}

void ICQClient::clearServerRequests()
{
    for (list<ServerRequest*>::iterator it_req = varRequests.begin(); it_req != varRequests.end(); ++it_req){
        (*it_req)->fail();
        delete *it_req;
    }
    varRequests.clear();
    list<InfoRequest>::iterator it;
    for (it = infoRequests.begin(); it != infoRequests.end(); ++it){
        Contact *contact = getContacts()->contact((*it).uin);
        if (contact == NULL)
            continue;
        Event e(EventFetchInfoFail, contact);
        e.process();
    }
    infoRequests.clear();
}

void ICQClient::snac_various(unsigned short type, unsigned short id)
{
    switch (type){
    case ICQ_SNACxVAR_ERROR:{
            unsigned short error_code;
            m_socket->readBuffer >> error_code;
            if (id == m_offlineMessagesRequestId)
            {
                log(L_WARN, "Server responded with error %04X for offline messages request.", error_code);
                // We'll never get ICQ_SRVxEND_OFFLINE_MSG, so we finish initing here instead.
            }
            else
            {
                ServerRequest *req = findServerRequest(id);
                if (req == NULL){
                    log(L_WARN, "Various event ID %04X not found for error %04X", id, error_code);
                    break;
                }
                req->fail(error_code);
            }
            break;
        }
    case ICQ_SNACxVAR_DATA:{
            TlvList tlv(m_socket->readBuffer);
            if (tlv(0x0001) == NULL){
                log(L_WARN, "Bad server response");
                break;
            }
            Buffer msg(*tlv(1));
            unsigned short len, nType, nId;
            unsigned long own_uin;
            msg >> len >> own_uin >> nType;
            msg.unpack(nId);
            switch (nType){
            case ICQ_SRVxEND_OFFLINE_MSG:
                serverRequest(ICQ_SRVxREQ_ACK_OFFLINE_MSG);
                sendServerRequest();
                setChatGroup();
                addFullInfoRequest(data.owner.Uin.value);
                m_bReady = true;
                processSendQueue();
                break;
            case ICQ_SRVxOFFLINE_MSG:{
                    unsigned long uin;
                    unsigned char type, flag;
                    struct tm sendTM;
                    memset(&sendTM, 0, sizeof(sendTM));
                    string message;
                    unsigned short year;
                    unsigned char month, day, hours, min;
                    msg.unpack(uin);
                    msg.unpack(year);
                    msg.unpack(month);
                    msg.unpack(day);
                    msg.unpack(hours);
                    msg.unpack(min);
                    msg.unpack(type);
                    msg.unpack(flag);
                    msg.unpack(message);
                #ifndef HAVE_TM_GMTOFF
                    sendTM.tm_sec  = -timezone;
                #else
                    time_t now = time (NULL);
                    sendTM = *localtime (&now);
                    sendTM.tm_sec  = sendTM.tm_gmtoff - (sendTM.tm_isdst == 1 ? 3600 : 0);
                #endif
                    sendTM.tm_year = year-1900;
                    sendTM.tm_mon  = month-1;
                    sendTM.tm_mday = day;
                    sendTM.tm_hour = hours;
                    sendTM.tm_min  = min;
                    sendTM.tm_isdst = -1;
                    time_t send_time = mktime(&sendTM);
                    MessageId id;
                    Message *m = parseMessage(type, number(uin).c_str(), message, msg, id, 0);
                    if (m){
                        m->setTime(send_time);
                        messageReceived(m, number(uin).c_str());
                    }
                    break;
                }
            case ICQ_SRVxANSWER_MORE:{
                    unsigned short nSubtype;
                    char nResult;
                    msg >> nSubtype >> nResult;
                    if ((nResult == 0x32) || (nResult == 0x14) || (nResult == 0x1E)){
                        ServerRequest *req = findServerRequest(nId);
                        if (req == NULL){
                            log(L_WARN, "Various event ID %04X not found (%X)", nId, nResult);
                            break;
                        }
                        req->fail();
                        varRequests.remove(req);
                        delete req;
                        break;
                    }
                    ServerRequest *req = findServerRequest(nId);
                    if (req == NULL){
                        log(L_WARN, "Various event ID %04X not found (%X)", nId, nResult);
                        break;
                    }
                    if (req->answer(msg, nSubtype)){
                        varRequests.remove(req);
                        delete req;
                    }
                    break;
                }
                break;
            default:
                log(L_WARN, "Unknown SNAC(15,03) response type %04X", nType);
            }
            break;
        }
    default:
        log(L_WARN, "Unknown various family type %04X", type);
    }
}

void ICQClient::serverRequest(unsigned short cmd, unsigned short seq)
{
    snac(ICQ_SNACxFAM_VARIOUS, ICQ_SNACxVAR_REQxSRV, true);
    m_socket->writeBuffer.tlv(0x0001, 0);
    m_socket->writeBuffer.pack(data.owner.Uin.value);
    m_socket->writeBuffer << cmd;
    m_socket->writeBuffer.pack((unsigned short)(seq ? seq : m_nMsgSequence));
}

void ICQClient::sendServerRequest()
{
    Buffer &b = m_socket->writeBuffer;
    char *packet = b.data(b.packetStartPos());
    unsigned short packet_size = (unsigned short)(b.size() - b.packetStartPos());
    unsigned short size = (unsigned short)(packet_size - 0x14);
    packet[0x12] = (char)((size >> 8) & 0xFF);
    packet[0x13] = (char)(size & 0xFF);
    size = (unsigned short)(packet_size - 0x16);
    packet[0x14] = (char)(size & 0xFF);
    packet[0x15] = (char)((size >> 8) & 0xFF);
    sendPacket(true);
}

void ICQClient::sendMessageRequest()
{
    serverRequest(ICQ_SRVxREQ_OFFLINE_MSG);
    m_offlineMessagesRequestId = m_nMsgSequence;
    sendServerRequest();
}

// _________________________________________________________________________________________

class FullInfoRequest : public ServerRequest
{
public:
    FullInfoRequest(ICQClient *client, unsigned short id, unsigned long uin);
protected:
    virtual void fail(unsigned short error_code);
    bool answer(Buffer &b, unsigned short nSubtype);
    string unpack_list(Buffer &b);
    unsigned m_nParts;
    unsigned long m_uin;
    ICQClient *m_client;
};

FullInfoRequest::FullInfoRequest(ICQClient *client, unsigned short id, unsigned long uin)
        : ServerRequest(id)
{
    m_client = client;
    m_nParts = 0;
    m_uin = uin;
}

void FullInfoRequest::fail(unsigned short)
{
    Contact *contact = NULL;
    if (m_nParts){
        if (m_client->data.owner.Uin.value == m_uin){
            Event e(EventClientChanged, m_client);
            e.process();
        }else{
            m_client->findContact(number(m_uin).c_str(), NULL, false, contact);
            if (contact){
                Event e(EventContactChanged, contact);
                e.process();
            }
        }
    }
    if (contact){
        Event e(EventFetchInfoFail, contact);
        e.process();
    }
    m_client->removeFullInfoRequest(m_uin);
}

string FullInfoRequest::unpack_list(Buffer &b)
{
    string res;
    char n;
    b >> n;
    for (; n > 0; n--){
        unsigned short c;
        b.unpack(c);
        string s;
        b >> s;
        if (c == 0) continue;
        if (res.length())
            res += ";";
        res += number(c);
        res += ",";
        res += quoteChars(s.c_str(), ";");
    }
    return res;
}

bool FullInfoRequest::answer(Buffer &b, unsigned short nSubtype)
{
    Contact *contact = NULL;
    ICQUserData *data;
    if (m_client->data.owner.Uin.value == m_uin){
        data = &m_client->data.owner;
    }else{
        data = m_client->findContact(number(m_uin).c_str(), NULL, false, contact);
        if (data == NULL){
            log(L_DEBUG, "Info request %lu not found", m_uin);
            m_client->removeFullInfoRequest(m_uin);
            return true;
        }
    }
    switch (nSubtype){
    case ICQ_SRVxGENERAL_INFO:{
            unsigned short n;
            char TimeZone;
            char authFlag;	/* ??? */
            char webAware;
            char allowDC;
            char hideEmail;
            b
            >> &data->Nick.ptr
            >> &data->FirstName.ptr
            >> &data->LastName.ptr
            >> &data->EMail.ptr
            >> &data->City.ptr
            >> &data->State.ptr
            >> &data->HomePhone.ptr
            >> &data->HomeFax.ptr
            >> &data->Address.ptr
            >> &data->PrivateCellular.ptr
            >> &data->Zip.ptr;
            b.unpack(n);
            data->Country.value = n;
            b
            >> TimeZone
            >> authFlag
            >> webAware
            >> allowDC
            >> hideEmail;
            data->TimeZone.value    = TimeZone;
            data->WebAware.value    = webAware;
            data->bNoDirect.value   = (bool)!allowDC;
            data->HiddenEMail.value = hideEmail;
            break;
        }
    case ICQ_SRVxMORE_INFO:{
            char c;
            b >> c;
            data->Age.value = c;
            b >> c;
            b >> c;
            data->Gender.value = c;
            b >> &data->Homepage.ptr;
            unsigned short year;
            b.unpack(year);
            data->BirthYear.value = year;
            b >> c;
            data->BirthMonth.value = c;
            b >> c;
            data->BirthDay.value = c;
            unsigned char lang[3];
            b
            >> lang[0]
            >> lang[1]
            >> lang[2];
            data->Language.value = (lang[2] << 16) + (lang[1] << 8) + lang[0];
            break;
        }
    case ICQ_SRVxEMAIL_INFO:{
            string mail;
            char c;
            b >> c;
            for (;c > 0;c--){
                char d;
                b >> d;
                string s;
                b >> s;
                s = quoteChars(s.c_str(), ";");
                if (mail.length())
                    mail += ";";
                mail += s;
                mail += '/';
                if (d)
                    mail += '-';
            }
            set_str(&data->EMails.ptr, mail.c_str());
            break;
        }
    case ICQ_SRVxWORK_INFO:{
            unsigned short n;
            b
            >> &data->WorkCity.ptr
            >> &data->WorkState.ptr
            >> &data->WorkPhone.ptr
            >> &data->WorkFax.ptr
            >> &data->WorkAddress.ptr
            >> &data->WorkZip.ptr;
            b.unpack(n);
            data->WorkCountry.value = n;
            b
            >> &data->WorkName.ptr
            >> &data->WorkDepartment.ptr
            >> &data->WorkPosition.ptr;
            b.unpack(n);
            data->Occupation.value = n;
            b >> &data->WorkHomepage.ptr;
            break;
        }
    case ICQ_SRVxABOUT_INFO:
        b >> &data->About.ptr;
        break;
    case ICQ_SRVxINTERESTS_INFO:
        set_str(&data->Interests.ptr, unpack_list(b).c_str());
        break;
    case ICQ_SRVxBACKGROUND_INFO:
        set_str(&data->Backgrounds.ptr, unpack_list(b).c_str());
        set_str(&data->Affilations.ptr, unpack_list(b).c_str());
        break;
    case ICQ_SRVxUNKNOWN_INFO:
        break;
    default:
        log(L_WARN, "Unknwon info type %04X for %lu", nSubtype, m_uin);
    }
    m_nParts++;
    if (m_nParts >= 8){
        data->InfoFetchTime.value = data->InfoUpdateTime.value ? data->InfoUpdateTime.value : 1;
        if (contact != NULL){
            m_client->setupContact(contact, data);
            Event e(EventContactChanged, contact);
            e.process();
        }else{
            int tz;
#ifndef HAVE_TM_GMTOFF
            tz = - timezone;
#else
            time_t now = time(NULL);
            struct tm *tm = localtime(&now);
            tz = tm->tm_gmtoff;
            if (tm->tm_isdst) tz -= (60 * 60);
#endif
            tz = - tz / (30 * 60);
            m_client->setupContact(getContacts()->owner(), data);
            if (data->TimeZone.value != (unsigned)tz){
                data->TimeZone.value = tz;
                m_client->setMainInfo(data);
            }
            Event eContact(EventContactChanged, getContacts()->owner());
            eContact.process();
            Event e(EventClientChanged, m_client);
            e.process();
        }
        m_client->removeFullInfoRequest(m_uin);
        return true;
    }
    return false;
}

unsigned ICQClient::processInfoRequest()
{
    if ((getState() != Connected) || infoRequests.empty())
        return 0;
    for (list<InfoRequest>::iterator it = infoRequests.begin(); it != infoRequests.end(); ++it){
        if ((*it).request_id)
            continue;
        unsigned delay = delayTime(SNAC(ICQ_SNACxFAM_VARIOUS, ICQ_SNACxVAR_REQxSRV));
        if (delay)
            return delay;
        unsigned long uin = (*it).uin;
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ((uin == data.owner.Uin.value) ? ICQ_SRVxREQ_OWN_INFO : ICQ_SRVxREQ_FULL_INFO);
        m_socket->writeBuffer.pack(uin);
        sendServerRequest();
        (*it).request_id = m_nMsgSequence;
        time_t now;
        time(&now);
        (*it).start_time = now;
        varRequests.push_back(new FullInfoRequest(this, m_nMsgSequence, uin));
    }
    return 0;
}

void ICQClient::checkInfoRequest()
{
    time_t now;
    time(&now);
    for (list<InfoRequest>::iterator it = infoRequests.begin(); it != infoRequests.end(); ){
        if (((*it).request_id == 0) || ((time_t)((*it).start_time + INFO_REQUEST_TIMEOUT) < now)){
            ++it;
            continue;
        }
        ServerRequest *req = findServerRequest((*it).request_id);
        if (req){
            req->fail();
        }else{
            infoRequests.erase(it);
        }
        it = infoRequests.begin();
    }
}

void ICQClient::addFullInfoRequest(unsigned long uin)
{
    for (list<InfoRequest>::iterator it = infoRequests.begin(); it != infoRequests.end(); ++it){
        if ((*it).uin == uin)
            return;
    }
    InfoRequest r;
    r.uin = uin;
    r.request_id = 0;
    r.start_time = 0;
    infoRequests.push_back(r);
    processSendQueue();
}

void ICQClient::removeFullInfoRequest(unsigned long uin)
{
    list<InfoRequest>::iterator it;
    for (it = infoRequests.begin(); it != infoRequests.end(); ++it){
        if ((*it).uin == uin){
            infoRequests.erase(it);
            break;
        }
    }
}

// _________________________________________________________________________________________

class SearchWPRequest : public ServerRequest
{
public:
    SearchWPRequest(ICQClient *client, unsigned short id);
protected:
    virtual void fail(unsigned short error_code);
    bool answer(Buffer &b, unsigned short nSubtype);
    ICQClient *m_client;
};

SearchWPRequest::SearchWPRequest(ICQClient *client, unsigned short id)
        : ServerRequest(id)
{
    m_client = client;
}

void SearchWPRequest::fail(unsigned short)
{
    SearchResult res;
    res.id = m_id;
    res.client = m_client;
    load_data(ICQProtocol::icqUserData, &res.data, NULL);
    Event e(EventSearchDone, &res);
    e.process();
    free_data(ICQProtocol::icqUserData, &res.data);
}

bool SearchWPRequest::answer(Buffer &b, unsigned short nSubType)
{
    SearchResult res;
    res.id = m_id;
    res.client = m_client;
    load_data(ICQProtocol::icqUserData, &res.data, NULL);

    unsigned short n;
    b >> n;
    b.unpack(res.data.Uin.value);
    char waitAuth;
    unsigned short state;
    char gender;
    unsigned short age;
    b
    >> &res.data.Nick.ptr
    >> &res.data.FirstName.ptr
    >> &res.data.LastName.ptr
    >> &res.data.EMail.ptr
    >> waitAuth;
    b.unpack(state);
    b >> gender;
    b.unpack(age);

    if (waitAuth)
        res.data.WaitAuth.bValue = true;
    switch (state){
    case SEARCH_STATE_OFFLINE:
        res.data.Status.value = STATUS_OFFLINE;
        break;
    case SEARCH_STATE_ONLINE:
        res.data.Status.value = STATUS_ONLINE;
        break;
    case SEARCH_STATE_DISABLED:
        res.data.Status.value = STATUS_UNKNOWN;
        break;
    }
    res.data.Gender.value = gender;
    res.data.Age.value    = age;

    if (res.data.Uin.value != m_client->data.owner.Uin.value){
        Event e(EventSearch, &res);
        e.process();
    }
    free_data(ICQProtocol::icqUserData, &res.data);

    if (nSubType == 0xAE01){
        unsigned long all;
        b >> all;
        load_data(ICQProtocol::icqUserData, &res.data, NULL);
        res.data.Uin.value = all;
        Event e(EventSearchDone, &res);
        e.process();
        free_data(ICQProtocol::icqUserData, &res.data);
        return true;
    }
    return false;
}

unsigned short ICQClient::findByUin(unsigned long uin)
{
    if (getState() != Connected)
        return (unsigned short)(-1);
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer
    << ICQ_SRVxREQ_WP_UIN;
    m_socket->writeBuffer.tlvLE(TLV_UIN, uin);
    sendServerRequest();
    varRequests.push_back(new SearchWPRequest(this, m_nMsgSequence));
    return m_nMsgSequence;
}

unsigned short ICQClient::findByMail(const char *mail)
{
    if (getState() != Connected)
        return (unsigned short)(-1);
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer
    << ICQ_SRVxREQ_WP_MAIL;
    m_socket->writeBuffer.tlvLE(TLV_EMAIL, mail);
    sendServerRequest();
    varRequests.push_back(new SearchWPRequest(this, m_nMsgSequence));
    return m_nMsgSequence;
}

void ICQClient::packTlv(unsigned short tlv, unsigned short code, const char *keywords)
{
    string k;
    if (keywords)
        k = keywords;
    if ((code == 0) && k.empty())
        return;
    Buffer b;
    b.pack(code);
    b << k;
    m_socket->writeBuffer.tlvLE(tlv, b);
}

unsigned short ICQClient::findWP(const char *szFirst, const char *szLast, const char *szNick,
                                 const char *szEmail, char age, char nGender,
                                 unsigned short nLanguage, const char *szCity, const char *szState,
                                 unsigned short nCountryCode,
                                 const char *szCoName, const char *szCoDept, const char *szCoPos,
                                 unsigned short nOccupation,
                                 unsigned short nPast, const char *szPast,
                                 unsigned short nInterests, const char *szInterests,
                                 unsigned short nAffilation, const char *szAffilation,
                                 unsigned short nHomePage, const char *szHomePage,
                                 const char *szKeyWords, bool bOnlineOnly)
{
    if (getState() != Connected)
        return (unsigned short)(-1);
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer << ICQ_SRVxREQ_WP_FULL;

    unsigned long nMinAge = 0;
    unsigned long nMaxAge = 0;
    switch (age){
    case 1:
        nMinAge = 18;
        nMaxAge = 22;
        break;
    case 2:
        nMinAge = 23;
        nMaxAge = 29;
        break;
    case 3:
        nMinAge = 30;
        nMaxAge = 39;
        break;
    case 4:
        nMinAge = 40;
        nMaxAge = 49;
        break;
    case 5:
        nMinAge = 50;
        nMaxAge = 59;
        break;
    case 6:
        nMinAge = 60;
        nMaxAge = 120;
        break;
    }

    if (szCity && *szCity)
        m_socket->writeBuffer.tlvLE(TLV_CITY, szCity);
    if (szState && *szState)
        m_socket->writeBuffer.tlvLE(TLV_STATE, szState);
    if (szCoName && *szCoName)
        m_socket->writeBuffer.tlvLE(TLV_WORK_COMPANY, szCoName);
    if (szCoDept && *szCoDept)
        m_socket->writeBuffer.tlvLE(TLV_WORK_DEPARTMENT, szCoDept);
    if (szCoPos && *szCoPos)
        m_socket->writeBuffer.tlvLE(TLV_WORK_POSITION, szCoPos);
    if (nMinAge || nMaxAge)
        m_socket->writeBuffer.tlvLE(TLV_AGE_RANGE, (nMaxAge << 16) + nMinAge);
    if (nGender)
        m_socket->writeBuffer.tlvLE(TLV_GENDER, nGender);
    if (nLanguage)
        m_socket->writeBuffer.tlvLE(TLV_LANGUAGE, nLanguage);
    if (nCountryCode)
        m_socket->writeBuffer.tlvLE(TLV_COUNTRY, nCountryCode);
    if (nOccupation)
        m_socket->writeBuffer.tlvLE(TLV_WORK_OCCUPATION, nOccupation);
    packTlv(TLV_PAST, nPast, szPast);
    packTlv(TLV_INTERESTS, nInterests, szInterests);
    packTlv(TLV_AFFILATIONS, nAffilation, szAffilation);
    packTlv(TLV_HOMEPAGE, nHomePage, szHomePage);
    if (szFirst && *szFirst)
        m_socket->writeBuffer.tlvLE(TLV_FIRST_NAME, szFirst);
    if (szLast && *szLast)
        m_socket->writeBuffer.tlvLE(TLV_LAST_NAME, szLast);
    if (szNick && *szNick)
        m_socket->writeBuffer.tlvLE(TLV_NICK, szNick);
    if (szKeyWords && *szKeyWords)
        m_socket->writeBuffer.tlvLE(TLV_KEYWORDS, szKeyWords);
    if (szEmail && *szEmail)
        m_socket->writeBuffer.tlvLE(TLV_EMAIL, szEmail);
    if (bOnlineOnly)
        m_socket->writeBuffer.tlvLE(TLV_SEARCH_ONLINE, (char)1);
    sendServerRequest();
    varRequests.push_back(new SearchWPRequest(this, m_nMsgSequence));
    return m_nMsgSequence;
}

// ______________________________________________________________________________________

class SetMainInfoRequest : public ServerRequest
{
public:
    SetMainInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string m_nick;
    string m_firstName;
    string m_lastName;
    string m_city;
    string m_state;
    string m_address;
    string m_zip;
    string m_email;
    string m_homePhone;
    string m_homeFax;
    string m_privateCellular;
    bool   m_hiddenEMail;
    unsigned m_country;
    unsigned m_tz;
    ICQClient *m_client;
};

SetMainInfoRequest::SetMainInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->Nick.ptr)
        m_nick = data->Nick.ptr;
    if (data->FirstName.ptr)
        m_firstName = data->FirstName.ptr;
    if (data->LastName.ptr)
        m_lastName = data->LastName.ptr;
    if (data->City.ptr)
        m_city = data->City.ptr;
    if (data->State.ptr)
        m_state = data->State.ptr;
    if (data->Address.ptr)
        m_address = data->Address.ptr;
    if (data->Zip.ptr)
        m_zip = data->Zip.ptr;
    if (data->EMail.ptr)
        m_email = data->EMail.ptr;
    if (data->HomePhone.ptr)
        m_homePhone = data->HomePhone.ptr;
    if (data->HomeFax.ptr)
        m_homeFax = data->HomeFax.ptr;
    if (data->PrivateCellular.ptr)
        m_privateCellular = data->PrivateCellular.ptr;
    m_country = data->Country.value;
    m_tz = data->TimeZone.value;
    m_hiddenEMail = data->HiddenEMail.bValue;
}

bool SetMainInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.Nick.ptr, m_nick.c_str());
    set_str(&m_client->data.owner.FirstName.ptr, m_firstName.c_str());
    set_str(&m_client->data.owner.LastName.ptr, m_lastName.c_str());
    set_str(&m_client->data.owner.City.ptr, m_city.c_str());
    set_str(&m_client->data.owner.State.ptr, m_state.c_str());
    set_str(&m_client->data.owner.Address.ptr, m_address.c_str());
    set_str(&m_client->data.owner.Zip.ptr, m_zip.c_str());
    set_str(&m_client->data.owner.EMail.ptr, m_email.c_str());
    set_str(&m_client->data.owner.HomePhone.ptr, m_homePhone.c_str());
    set_str(&m_client->data.owner.HomeFax.ptr, m_homeFax.c_str());
    set_str(&m_client->data.owner.PrivateCellular.ptr, m_privateCellular.c_str());
    m_client->data.owner.Country.value = m_country;
    m_client->data.owner.TimeZone.value = m_tz;
    m_client->data.owner.HiddenEMail.bValue = m_hiddenEMail;
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetWorkInfoRequest : public ServerRequest
{
public:
    SetWorkInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string m_workCity;
    string m_workState;
    string m_workAddress;
    string m_workZip;
    unsigned m_workCountry;
    string m_workName;
    string m_workDepartment;
    string m_workPosition;
    string m_workPhone;
    string m_workFax;
    unsigned m_occupation;
    string m_workHomepage;
    ICQClient *m_client;
};

SetWorkInfoRequest::SetWorkInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->WorkCity.ptr)
        m_workCity = data->WorkCity.ptr;
    if (data->WorkState.ptr)
        m_workState = data->WorkState.ptr;
    if (data->WorkAddress.ptr)
        m_workAddress = data->WorkAddress.ptr;
    if (data->WorkZip.ptr)
        m_workZip = data->WorkZip.ptr;
    m_workCountry = data->WorkCountry.value;
    if (data->WorkName.ptr)
        m_workName = data->WorkName.ptr;
    if (data->WorkDepartment.ptr)
        m_workDepartment = data->WorkDepartment.ptr;
    if (data->WorkPosition.ptr)
        m_workPosition = data->WorkPosition.ptr;
    m_occupation = data->Occupation.value;
    if (data->WorkHomepage.ptr)
        m_workHomepage = data->WorkHomepage.ptr;
    if (data->WorkPhone.ptr)
        m_workPhone = data->WorkPhone.ptr;
    if (data->WorkFax.ptr)
        m_workFax = data->WorkFax.ptr;
}

bool SetWorkInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.WorkCity.ptr, m_workCity.c_str());
    set_str(&m_client->data.owner.WorkState.ptr, m_workState.c_str());
    set_str(&m_client->data.owner.WorkAddress.ptr, m_workAddress.c_str());
    set_str(&m_client->data.owner.WorkZip.ptr, m_workZip.c_str());
    set_str(&m_client->data.owner.WorkName.ptr, m_workName.c_str());
    set_str(&m_client->data.owner.WorkDepartment.ptr, m_workDepartment.c_str());
    set_str(&m_client->data.owner.WorkPosition.ptr, m_workPosition.c_str());
    set_str(&m_client->data.owner.WorkHomepage.ptr, m_workHomepage.c_str());
    set_str(&m_client->data.owner.WorkPhone.ptr, m_workPhone.c_str());
    set_str(&m_client->data.owner.WorkFax.ptr, m_workFax.c_str());
    m_client->data.owner.WorkCountry.value = m_workCountry;
    m_client->data.owner.Occupation.value = m_occupation;
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetMoreInfoRequest : public ServerRequest
{
public:
    SetMoreInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    unsigned m_age;
    unsigned m_gender;
    unsigned m_birthYear;
    unsigned m_birthMonth;
    unsigned m_birthDay;
    unsigned m_language;
    string   m_homepage;
    ICQClient *m_client;
};

SetMoreInfoRequest::SetMoreInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    m_age = data->Age.value;
    m_gender = data->Gender.value;
    m_birthYear = data->BirthYear.value;
    m_birthMonth = data->BirthMonth.value;
    m_birthDay = data->BirthDay.value;
    m_language = data->Language.value;
    string   m_homepage;
    if (data->Homepage.ptr)
        m_homepage = data->Homepage.ptr;
}

bool SetMoreInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.Homepage.ptr, m_homepage.c_str());
    m_client->data.owner.Age.value = m_age;
    m_client->data.owner.Gender.value = m_gender;
    m_client->data.owner.BirthYear.value = m_birthYear;
    m_client->data.owner.BirthMonth.value = m_birthMonth;
    m_client->data.owner.BirthDay.value = m_birthDay;
    m_client->data.owner.Language.value = m_language;
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetAboutInfoRequest : public ServerRequest
{
public:
    SetAboutInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string   m_about;
    ICQClient *m_client;
};

SetAboutInfoRequest::SetAboutInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->About.ptr)
        m_about = data->About.ptr;
}

bool SetAboutInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.About.ptr, m_about.c_str());
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetMailInfoRequest : public ServerRequest
{
public:
    SetMailInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string   m_emails;
    ICQClient *m_client;
};

SetMailInfoRequest::SetMailInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->EMails.ptr)
        m_emails = data->EMails.ptr;
}

bool SetMailInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.EMails.ptr, m_emails.c_str());
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetInterestsInfoRequest : public ServerRequest
{
public:
    SetInterestsInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string   m_interests;
    ICQClient *m_client;
};

SetInterestsInfoRequest::SetInterestsInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->Interests.ptr)
        m_interests = data->Interests.ptr;
}

bool SetInterestsInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.Interests.ptr, m_interests.c_str());
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetBackgroundsInfoRequest : public ServerRequest
{
public:
    SetBackgroundsInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    string   m_backgrounds;
    string   m_affilations;
    ICQClient *m_client;
};

SetBackgroundsInfoRequest::SetBackgroundsInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client = client;
    if (data->Backgrounds.ptr)
        m_backgrounds = data->Backgrounds.ptr;
    if (data->Affilations.ptr)
        m_affilations = data->Affilations.ptr;
}

bool SetBackgroundsInfoRequest::answer(Buffer&, unsigned short)
{
    set_str(&m_client->data.owner.Backgrounds.ptr, m_backgrounds.c_str());
    set_str(&m_client->data.owner.Affilations.ptr, m_affilations.c_str());
    Event e(EventClientChanged, m_client);
    e.process();
    m_client->sendUpdate();
    return true;
}

class SetSecurityInfoRequest : public ServerRequest
{
public:
    SetSecurityInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    bool m_bWebAware;
    bool m_bWaitAuth;
    ICQClient *m_client;
};

SetSecurityInfoRequest::SetSecurityInfoRequest(ICQClient *client, unsigned short id, ICQUserData *data)
        : ServerRequest(id)
{
    m_client  = client;
    m_bWebAware = data->WebAware.bValue;
    m_bWaitAuth = data->WaitAuth.bValue;
}

bool SetSecurityInfoRequest::answer(Buffer&, unsigned short)
{
    if (m_client->data.owner.WebAware.bValue != m_bWebAware){
        m_client->data.owner.WebAware.bValue = m_bWebAware;
        m_client->sendStatus();
    }
    m_client->data.owner.WaitAuth.bValue = m_bWaitAuth;
    Event e(EventClientChanged, m_client);
    e.process();
    return true;
}

void ICQClient::setMainInfo(ICQUserData *d)
{
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer << ICQ_SRVxWP_SET;
    m_socket->writeBuffer.pack(TLV_TIMEZONE);
    m_socket->writeBuffer.pack((unsigned short)1);
    m_socket->writeBuffer << (char)(d->TimeZone.value);
    m_socket->writeBuffer.tlvLE(TLV_NICK, d->Nick.ptr);
    m_socket->writeBuffer.tlvLE(TLV_FIRST_NAME, d->FirstName.ptr);
    m_socket->writeBuffer.tlvLE(TLV_LAST_NAME, d->LastName.ptr);
    m_socket->writeBuffer.tlvLE(TLV_EMAIL, d->EMail.ptr);
    m_socket->writeBuffer << d->HiddenEMail.bValue;
    m_socket->writeBuffer.tlvLE(TLV_CITY, d->City.ptr);
    m_socket->writeBuffer.tlvLE(TLV_STATE, d->State.ptr);
    m_socket->writeBuffer.tlvLE(TLV_PHONE, d->HomePhone.ptr);
    m_socket->writeBuffer.tlvLE(TLV_FAX, d->HomeFax.ptr);
    m_socket->writeBuffer.tlvLE(TLV_STREET, d->Address.ptr);
    m_socket->writeBuffer.tlvLE(TLV_CELLULAR, d->PrivateCellular.ptr);
    m_socket->writeBuffer.tlvLE(TLV_ZIP, d->Zip.ptr);
    m_socket->writeBuffer.tlvLE(TLV_COUNTRY, (unsigned short)(d->Country.value));
    sendServerRequest();

    varRequests.push_back(new SetMainInfoRequest(this, m_nMsgSequence, d));
}

void ICQClient::packInfoList(char *str)
{
    list<unsigned short> category;
    list<string> spec;

    if (str){
        string s = str;
        while (s.length()){
            string item = getToken(s, ';');
            string cat = getToken(item, ',');
            category.push_back((unsigned short)atol(cat.c_str()));
            spec.push_back(item);
        }
    }

    char n = (char)(category.size());
    m_socket->writeBuffer << n;

    list<unsigned short>::iterator itc = category.begin();
    list<string>::iterator its = spec.begin();

    for (; itc != category.end(); ++itc, ++its){
        m_socket->writeBuffer.pack(*itc);
        m_socket->writeBuffer << *its;
    }
}

void ICQClient::setClientInfo(void *_data)
{
    if (getState() != Connected)
        return;

    ICQUserData *d = (ICQUserData*)_data;

    if (m_bAIM){
        d->ProfileFetch.bValue = true;
        set_str(&data.owner.About.ptr, d->About.ptr);
        setAIMInfo(d);
        setProfile(d);
        return;
    }

    set_str(&d->HomePhone.ptr, NULL);
    set_str(&d->HomeFax.ptr, NULL);
    set_str(&d->WorkPhone.ptr, NULL);
    set_str(&d->WorkFax.ptr, NULL);
    set_str(&d->PrivateCellular.ptr, NULL);
    set_str(&d->EMail.ptr, NULL);
    set_str(&d->EMails.ptr, NULL);

    QString phones = getContacts()->owner()->getPhones();
    while (phones.length()){
        QString phoneItem = getToken(phones, ';', false);
        QString phoneValue = getToken(phoneItem, '/', false);
        if (phoneItem.length())
            continue;
        QString number = getToken(phoneValue, ',');
        QString type = getToken(phoneValue, ',');
        if (type == "Home Phone"){
            set_str(&d->HomePhone.ptr, getContacts()->fromUnicode(NULL, number).c_str());
        }else if (type == "Home Fax"){
            set_str(&d->HomeFax.ptr, getContacts()->fromUnicode(NULL, number).c_str());
        }else if (type == "Work Phone"){
            set_str(&d->WorkPhone.ptr, getContacts()->fromUnicode(NULL, number).c_str());
        }else if (type == "Work Fax"){
            set_str(&d->WorkFax.ptr, getContacts()->fromUnicode(NULL, number).c_str());
        }else if (type == "Private Cellular"){
            number += " SMS";
            set_str(&d->PrivateCellular.ptr, getContacts()->fromUnicode(NULL, number).c_str());
        }
    }
    d->HiddenEMail.bValue = false;
    QString mails = getContacts()->owner()->getEMails();
    string s;
    while (mails.length()){
        QString mailItem = getToken(mails, ';', false);
        QString mail = getToken(mailItem, '/');
        if (!s.empty())
            s += ';';
        s += getContacts()->fromUnicode(NULL, mail);
        s += '/';
        if (mailItem.length())
            s += '-';
        if (d->EMail.ptr == NULL){
            set_str(&d->EMail.ptr, getContacts()->fromUnicode(NULL, mail).c_str());
            d->HiddenEMail.bValue = !mailItem.isEmpty();
        }
    }
    set_str(&d->EMails.ptr, s.c_str());

    if ((d->Country.value != data.owner.Country.value) ||
            (d->HiddenEMail.bValue != data.owner.HiddenEMail.bValue) ||
            cmp(d->Nick.ptr, data.owner.Nick.ptr) ||
            cmp(d->FirstName.ptr, data.owner.FirstName.ptr) ||
            cmp(d->LastName.ptr, data.owner.LastName.ptr) ||
            cmp(d->EMail.ptr, data.owner.EMail.ptr) ||
            cmp(d->HomePhone.ptr, data.owner.HomePhone.ptr) ||
            cmp(d->HomeFax.ptr, data.owner.HomeFax.ptr) ||
            cmp(d->PrivateCellular.ptr, data.owner.PrivateCellular.ptr) ||
            cmp(d->City.ptr, data.owner.City.ptr) ||
            cmp(d->State.ptr, data.owner.State.ptr) ||
            cmp(d->Address.ptr, data.owner.Address.ptr) ||
            cmp(d->Zip.ptr, data.owner.Zip.ptr)){
        setMainInfo(d);
        m_nUpdates++;
    }

    if ((d->WorkCountry.value != data.owner.WorkCountry.value) ||
            (d->Occupation.value != data.owner.Occupation.value) ||
            cmp(d->WorkCity.ptr, data.owner.WorkCity.ptr) ||
            cmp(d->WorkState.ptr, data.owner.WorkState.ptr) ||
            cmp(d->WorkAddress.ptr, data.owner.WorkAddress.ptr) ||
            cmp(d->WorkZip.ptr, data.owner.WorkZip.ptr) ||
            cmp(d->WorkName.ptr, data.owner.WorkName.ptr) ||
            cmp(d->WorkDepartment.ptr, data.owner.WorkDepartment.ptr) ||
            cmp(d->WorkPosition.ptr, data.owner.WorkPosition.ptr) ||
            cmp(d->WorkPhone.ptr, data.owner.WorkPhone.ptr) ||
            cmp(d->WorkFax.ptr, data.owner.WorkFax.ptr) ||
            cmp(d->WorkHomepage.ptr, data.owner.WorkHomepage.ptr)){

        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxWP_SET;
        m_socket->writeBuffer.tlvLE(TLV_WORK_CITY, d->WorkCity.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_STATE, d->WorkState.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_PHONE, d->WorkPhone.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_FAX, d->WorkFax.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_STREET, d->WorkAddress.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_ZIP, d->WorkZip.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_COUNTRY, (unsigned short)d->WorkCountry.value);
        m_socket->writeBuffer.tlvLE(TLV_WORK_COMPANY, d->WorkName.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_DEPARTMENT, d->WorkDepartment.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_POSITION, d->WorkPosition.ptr);
        m_socket->writeBuffer.tlvLE(TLV_WORK_OCCUPATION, (unsigned short)d->Occupation.value);
        m_socket->writeBuffer.tlvLE(TLV_WORK_HOMEPAGE, d->WorkHomepage.ptr);
        sendServerRequest();
        varRequests.push_back(new SetWorkInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }
    if ((d->Age.value != data.owner.Age.value) ||
            (d->Gender.value != data.owner.Gender.value) ||
            (d->BirthYear.value != data.owner.BirthYear.value) ||
            (d->BirthMonth.value != data.owner.BirthMonth.value) ||
            (d->BirthDay.value != data.owner.BirthDay.value) ||
            (d->Language.value != data.owner.Language.value) ||
            cmp(d->Homepage.ptr, data.owner.Homepage.ptr)){

        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxWP_SET;
        m_socket->writeBuffer.tlvLE(TLV_AGE, (unsigned short)(d->Age.value));
        m_socket->writeBuffer.pack(TLV_GENDER);
        m_socket->writeBuffer.pack((unsigned short)1);
        m_socket->writeBuffer << (char)(d->Gender.value);
        m_socket->writeBuffer.tlvLE(TLV_HOMEPAGE, d->Homepage.ptr);
        m_socket->writeBuffer.pack(TLV_BIRTHDAY);
        m_socket->writeBuffer.pack((unsigned short)6);
        m_socket->writeBuffer.pack((unsigned short)d->BirthYear.value);
        m_socket->writeBuffer.pack((unsigned short)d->BirthMonth.value);
        m_socket->writeBuffer.pack((unsigned short)d->BirthDay.value);
        m_socket->writeBuffer.tlvLE(TLV_LANGUAGE, (unsigned short)(d->Language.value & 0xFF));
        m_socket->writeBuffer.tlvLE(TLV_LANGUAGE, (unsigned short)((d->Language.value >> 8) & 0xFF));
        m_socket->writeBuffer.tlvLE(TLV_LANGUAGE, (unsigned short)((d->Language.value >> 16) & 0xFF));
        sendServerRequest();
        varRequests.push_back(new SetMoreInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }

    if (cmp(d->About.ptr, data.owner.About.ptr)){
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxWP_SET;
        m_socket->writeBuffer.tlvLE(TLV_NOTES, d->About.ptr);
        sendServerRequest();
        varRequests.push_back(new SetAboutInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }

    if (cmp(d->EMails.ptr, data.owner.EMails.ptr)){
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer   << ICQ_SRVxREQ_MODIFY_MAIL;
        string emails;
        list<string> mails;
        list<bool> hiddens;
        if (d->EMails.ptr){
            emails = d->EMails.ptr;
            while (emails.length()){
                string mailItem = getToken(emails, ';', false);
                string mail = getToken(mailItem, '/');
                mails.push_back(mail);
                hiddens.push_back(mailItem.length() != 0);
            }
        }
        m_socket->writeBuffer << (char)(mails.size());
        list<string>::iterator its = mails.begin();
        list<bool>::iterator ith = hiddens.begin();
        for (; its != mails.end(); ++its, ++ith){
            char hide = (char)((*ith) ? 1 : 0);
            m_socket->writeBuffer << hide << (*its);
        }
        sendServerRequest();
        varRequests.push_back(new SetMailInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }

    if (cmp(d->Interests.ptr, data.owner.Interests.ptr)){
        varRequests.push_back(new SetInterestsInfoRequest(this, m_nMsgSequence, d));
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxREQ_MODIFY_INTERESTS;
        packInfoList(d->Interests.ptr);
        sendServerRequest();
        varRequests.push_back(new SetInterestsInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }

    if (cmp(d->Backgrounds.ptr, data.owner.Backgrounds.ptr) ||
            cmp(d->Affilations.ptr, data.owner.Affilations.ptr)){
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxREQ_MODIFY_BACKGROUND;
        packInfoList(d->Backgrounds.ptr);
        packInfoList(d->Affilations.ptr);
        sendServerRequest();
        varRequests.push_back(new SetBackgroundsInfoRequest(this, m_nMsgSequence, d));
        m_nUpdates++;
    }

    if ((d->WaitAuth.bValue != data.owner.WaitAuth.bValue) ||
            (d->WebAware.bValue != data.owner.WebAware.bValue)){
        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer  << ICQ_SRVxREQ_PERMISSIONS
        << (char)(d->WaitAuth.bValue ? 0 : 0x01)
        << (char)(d->WebAware.bValue ? 0x01 : 0)
        << (char)0x02
        << (char)0;
        sendServerRequest();
        varRequests.push_back(new SetSecurityInfoRequest(this, m_nMsgSequence, d));
    }
    setChatGroup();
    sendStatus();
}

class SetPasswordRequest : public ServerRequest
{
public:
    SetPasswordRequest(ICQClient *client, unsigned short id, const char *pwd);
protected:
    bool answer(Buffer &b, unsigned short nSubtype);
    virtual void fail(unsigned short error_code);
    string m_pwd;
    ICQClient *m_client;
};

SetPasswordRequest::SetPasswordRequest(ICQClient *client, unsigned short id, const char *pwd)
        : ServerRequest(id)
{
    m_client  = client;
    m_pwd     = pwd;
}

bool SetPasswordRequest::answer(Buffer&, unsigned short)
{
    m_client->setPassword(QString::fromUtf8(m_pwd.c_str()));
    return true;
}

void SetPasswordRequest::fail(unsigned short error_code)
{
    log(L_DEBUG, "Password change fail: %X", error_code);
    clientErrorData d;
    d.client  = m_client;
    d.code    = 0;
    d.err_str = I18N_NOOP("Change password fail");
    d.args    = NULL;
    d.flags   = ERR_ERROR;
    d.options = NULL;
    d.id	  = CmdPasswordFail;
    Event e(EventClientError, &d);
    e.process();
}

void ICQClient::changePassword(const char *new_pswd)
{
    QString pwd = QString::fromUtf8(new_pswd);
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer
    << ICQ_SRVxREQ_CHANGE_PASSWD
    << getContacts()->fromUnicode(NULL, pwd);
    sendServerRequest();
    varRequests.push_back(new SetPasswordRequest(this, m_nMsgSequence, new_pswd));
}

class SMSRequest : public ServerRequest
{
public:
    SMSRequest(ICQClient *client, unsigned short id);
    virtual bool answer(Buffer&, unsigned short nSubType);
    virtual void fail(unsigned short error_code);
protected:
    ICQClient *m_client;
};

#if 0
const char *translations[] =
    {
        I18N_NOOP("The Cellular network is currently unable to send your message to the recipient. Please try again later."),
        I18N_NOOP("INVALID NUMBER"),
        I18N_NOOP("RATE LIMIT")
    };
#endif

SMSRequest::SMSRequest(ICQClient *client, unsigned short id)
        : ServerRequest(id)
{
    m_client = client;
}

bool SMSRequest::answer(Buffer &b, unsigned short code)
{
    m_client->m_sendSmsId = 0;
    if (code == 0x0100){
        if (m_client->smsQueue.empty())
            return true;
        string errStr = b.data(b.readPos());
        SendMsg &s = m_client->smsQueue.front();
        SMSMessage *sms = static_cast<SMSMessage*>(s.msg);
        m_client->smsQueue.erase(m_client->smsQueue.begin());
        sms->setError(errStr.c_str());
        Event e(EventMessageSent, sms);
        e.process();
        delete sms;
    }else{
        b.incReadPos(6);
        string provider;
        string answer;
        b.unpackStr(provider);
        b.unpackStr(answer);

        string::iterator s = answer.begin();
        auto_ptr<XmlNode> top(XmlNode::parse(s, answer.end()));
        string error = I18N_NOOP("SMS send fail");
        string network;
        if (top.get()){
            XmlNode *n = top.get();
            if (n && n->isBranch()){
                XmlBranch *msg = static_cast<XmlBranch*>(n);
                XmlLeaf *l = msg->getLeaf("deliverable");
                if (l && (l->getValue() == "Yes")){
                    error = "";
                    l = msg->getLeaf("network");
                    if (l)
                        network = l->getValue();
                }else{
                    XmlBranch *param = msg->getBranch("param");
                    if (param){
                        XmlLeaf *l = param->getLeaf("error");
                        if (l)
                            error = l->getValue();
                    }
                }
            }
        }

        if (error.empty()){
            if (!m_client->smsQueue.empty()){
                SendMsg &s = m_client->smsQueue.front();
                SMSMessage *sms = static_cast<SMSMessage*>(s.msg);
                sms->setNetwork(network.c_str());
                if ((sms->getFlags() & MESSAGE_NOHISTORY) == 0){
                    SMSMessage m;
                    m.setContact(sms->contact());
                    m.setText(s.part);
                    m.setPhone(sms->getPhone());
                    m.setNetwork(network.c_str());
                    Event e(EventSent, &m);
                    e.process();
                }
            }
        }else{
            if (!m_client->smsQueue.empty()){
                SendMsg &s = m_client->smsQueue.front();
                s.msg->setError(error.c_str());
                Event e(EventMessageSent, s.msg);
                e.process();
                delete s.msg;
                m_client->smsQueue.erase(m_client->smsQueue.begin());
            }
        }
    }
    m_client->processSendQueue();
    return true;
}

void SMSRequest::fail(unsigned short)
{
    if (m_client->smsQueue.empty())
        return;
    SendMsg &s = m_client->smsQueue.front();
    Message *sms = s.msg;
    sms->setError(I18N_NOOP("SMS send fail"));
    m_client->smsQueue.erase(m_client->smsQueue.begin());
    Event e(EventMessageSent, sms);
    e.process();
    delete sms;
    m_client->m_sendSmsId = 0;
    m_client->processSendQueue();
}

const unsigned MAX_SMS_LEN_LATIN1	= 160;
const unsigned MAX_SMS_LEN_UNICODE	= 70;

static const char *w_days[] =
    {
        "Sun",
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Say"
    };

static const char *months[] =
    {
        I18N_NOOP("Jan"),
        I18N_NOOP("Feb"),
        I18N_NOOP("Mar"),
        I18N_NOOP("Apr"),
        I18N_NOOP("May"),
        I18N_NOOP("Jun"),
        I18N_NOOP("Jul"),
        I18N_NOOP("Aug"),
        I18N_NOOP("Sep"),
        I18N_NOOP("Oct"),
        I18N_NOOP("Nov"),
        I18N_NOOP("Dec")
    };

unsigned ICQClient::processSMSQueue()
{
    if (m_sendSmsId)
        return 0;
    for (;;){
        if (smsQueue.empty())
            break;
        unsigned delay = delayTime(SNAC(ICQ_SNACxFAM_VARIOUS, ICQ_SNACxVAR_REQxSRV));
        if (delay)
            return delay;
        SendMsg &s = smsQueue.front();
        if (s.text.isEmpty() || (!(s.flags & SEND_1STPART) && (s.msg->getFlags() & MESSAGE_1ST_PART))){
            Event e(EventMessageSent, s.msg);
            e.process();
            delete s.msg;
            smsQueue.erase(smsQueue.begin());
            continue;
        }
        SMSMessage *sms = static_cast<SMSMessage*>(s.msg);
        QString text = s.text;
        QString part = getPart(text, MAX_SMS_LEN_LATIN1);
        if (!isLatin(part)){
            text = s.text;
            part = getPart(text, MAX_SMS_LEN_UNICODE);
        }
        s.text = text;
        s.part = part;

        string	nmb = "+";
        QString	phone = sms->getPhone();
        for (int i = 0; i < (int)(phone.length()); i++){
            char c = phone[i].latin1();
            if ((c >= '0') && (c <= '9'))
                nmb += c;
        }
        XmlBranch xmltree("icq_sms_message");
        xmltree.pushnode(new XmlLeaf("destination",nmb));
        xmltree.pushnode(new XmlLeaf("text",(const char*)(part.utf8())));
        xmltree.pushnode(new XmlLeaf("codepage","1252"));
        xmltree.pushnode(new XmlLeaf("encoding","utf8"));
        xmltree.pushnode(new XmlLeaf("senders_UIN",number(data.owner.Uin.value).c_str()));
        xmltree.pushnode(new XmlLeaf("senders_name",""));
        xmltree.pushnode(new XmlLeaf("delivery_receipt","Yes"));

        char timestr[30];
        time_t t = time(NULL);
        struct tm *tm;
        tm = gmtime(&t);
        snprintf(timestr, sizeof(timestr), "%s, %02u %s %04u %02u:%02u:%02u GMT",
                 w_days[tm->tm_wday], tm->tm_mday, months[tm->tm_mon], tm->tm_year + 1900,
                 tm->tm_hour, tm->tm_min, tm->tm_sec);
        xmltree.pushnode(new XmlLeaf("time",string(timestr)));
        string msg = xmltree.toString(0);

        serverRequest(ICQ_SRVxREQ_MORE);
        m_socket->writeBuffer << ICQ_SRVxREQ_SEND_SMS
        << 0x00010016L << 0x00000000L << 0x00000000L
        << 0x00000000L << 0x00000000L << (unsigned long)(msg.size());
        m_socket->writeBuffer << msg.c_str();
        sendServerRequest();
        varRequests.push_back(new SMSRequest(this, m_nMsgSequence));
        m_sendSmsId = m_nMsgSequence;
        break;
    }
    return 0;
}

void ICQClient::clearSMSQueue()
{
    for (list<SendMsg>::iterator it = smsQueue.begin(); it != smsQueue.end(); ++it){
        (*it).msg->setError(I18N_NOOP("Client go offline"));
        Event e(EventMessageSent, (*it).msg);
        e.process();
        delete (*it).msg;
    }
    smsQueue.clear();
    m_sendSmsId = 0;
}

void ICQClient::setChatGroup()
{
    if ((getState() != Connected) || (getRandomChatGroup() == getRandomChatGroupCurrent()))
        return;
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer << (unsigned short)ICQ_SRVxREQ_SET_CHAT_GROUP;
    if (getRandomChatGroup()){
        m_socket->writeBuffer.pack((unsigned short)getRandomChatGroup());
        m_socket->writeBuffer
        << 0x00000310L
        << 0x00000000L
        << 0x00000000L
        << 0x00000000L
        << (char)4
        << (char)ICQ_TCP_VERSION
        << 0x00000000L
        << 0x00000050L
        << 0x00000003L
        << (unsigned short)0
        << (char)0;
    }else{
        m_socket->writeBuffer << (unsigned short)0;
    }
    sendServerRequest();
    setRandomChatGroupCurrent(getRandomChatGroup());
}

class RandomChatRequest : public ServerRequest
{
public:
    RandomChatRequest(ICQClient *client, unsigned short id);
protected:
    virtual void fail(unsigned short error_code);
    bool answer(Buffer &b, unsigned short nSubtype);
    ICQClient *m_client;
};


RandomChatRequest::RandomChatRequest(ICQClient *client, unsigned short id)
        : ServerRequest(id)
{
    m_client = client;
}

bool RandomChatRequest::answer(Buffer &b, unsigned short)
{
    unsigned long uin;
    b.unpack(uin);
    Event e(EventRandomChat, (void*)uin);
    e.process();
    return true;
}

void RandomChatRequest::fail(unsigned short)
{
    Event e(EventRandomChat, NULL);
    e.process();
}

void ICQClient::searchChat(unsigned short group)
{
    if (getState() != Connected){
        Event e(EventRandomChat, NULL);
        e.process();
        return;
    }
    serverRequest(ICQ_SRVxREQ_MORE);
    m_socket->writeBuffer << (unsigned short)ICQ_SRVxREQ_RANDOM_CHAT;
    m_socket->writeBuffer.pack(group);
    sendServerRequest();
    varRequests.push_back(new RandomChatRequest(this, m_nMsgSequence));
}




