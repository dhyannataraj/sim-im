#include "servicesnachandler.h"
#include "bytearraybuilder.h"
#include "bytearrayparser.h"
#include "icqclient.h"
#include "log.h"

using SIM::log;
using SIM::L_DEBUG;
using SIM::L_WARN;
using SIM::L_ERROR;

ServiceSnacHandler::ServiceSnacHandler(ICQClient* client) : SnacHandler(client, ICQ_SNACxFOOD_SERVICE)
{
}

bool ServiceSnacHandler::process(unsigned short subtype, const QByteArray& data, int flags, unsigned int requestId)
{
    // TODO move to ICQClient
    QByteArray realData = data;
    if(flags & 0x8000) // FIXME hardcoded
    {
        ByteArrayParser parser(data);
        int length = parser.readWord();
        parser.readBytes(length);
        realData = parser.readAll();
    }
    switch(subtype)
    {
    case SnacServiceServerReady:
        return sendServices(realData);

    case SnacServiceAvailable:
        return handleServiceResponse(realData);

    case SnacServiceSelfInfoReply:
        return handleSelfInfoReply(realData);

    case SnacServiceCapabilitiesAck:
        return requestRateInfo();

    case SnacServiceRateInfo:
        parseRateInfo(data);
        emit initiateLoginStep2();
        return requestSelfInfo();

    case SnacServiceStatus:
        return parseServiceStatus(realData);
    }

    return true;
}

void ServiceSnacHandler::disconnect()
{
    m_rateInfoList.clear();
}

bool ServiceSnacHandler::sendServices(const QByteArray& data)
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);
    ByteArrayBuilder builder;
    if(client()->isAim()) {
        builder.appendDword(0x00010003L);
        builder.appendDword(0x00130003L);
        builder.appendDword(0x00020001L);
        builder.appendDword(0x00030001L);
        builder.appendDword(0x00040001L);
        builder.appendDword(0x00060001L);
        builder.appendDword(0x00080001L);
        builder.appendDword(0x00090001L);
        builder.appendDword(0x000A0001L);
        builder.appendDword(0x000B0001L);
    } else {
        builder.appendDword(0x00010004L);
        builder.appendDword(0x00130004L);
        builder.appendDword(0x00020001L);
        builder.appendDword(0x00030001L);
        builder.appendDword(0x00150001L);
        builder.appendDword(0x00040001L);
        builder.appendDword(0x00060001L);
        builder.appendDword(0x00090001L);
        builder.appendDword(0x000A0001L);
        builder.appendDword(0x000B0001L);
    }
    socket->snac(ICQ_SNACxFOOD_SERVICE, SnacServiceCapabilities, 0, builder.getArray());
    return true;
}

bool ServiceSnacHandler::requestRateInfo()
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);
    socket->snac(getType(), SnacServiceRateInfoRequest, 0, QByteArray());
    return true;
}

bool ServiceSnacHandler::parseRateInfo(const QByteArray& data)
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);

    m_rateInfoList.clear();
    ByteArrayParser parser(data);
    int classes = parser.readWord();
    for(int i = 0; i < classes; i++)
    {
        RateInfoPtr info = readNextRateInfoClass(parser);
        m_rateInfoList.append(info);
    }

    ByteArrayBuilder builder;
    for(int classnum = 0; classnum < classes; classnum++)
    {
        int classid = readNextRateInfoGroup(parser);
        if(classid < 0)
            return false;
        builder.appendWord(classid);
    }

    socket->snac(getType(), SnacServiceRateInfoAck, 0, builder.getArray());
    return true;
}

bool ServiceSnacHandler::handleServiceResponse(const QByteArray& data)
{
    log(L_DEBUG, "ServiceSnacHandler::handleServiceResponse: %s", data.toHex().data());
    TlvList tlvs = TlvList::fromByteArray(data);

    Tlv serviceIdTlv = tlvs.firstTlv(TlvServiceId);
    if(!serviceIdTlv.isValid())
    {
        log(L_WARN, "ServiceSnacHandler::handleServiceResponse: No service Tlv");
        return false;
    }
    int serviceId = serviceIdTlv.toUint16();

    Tlv serviceAddressTlv = tlvs.firstTlv(TlvServiceAddress);
    if(!serviceAddressTlv.isValid())
    {
        log(L_WARN, "ServiceSnacHandler::handleServiceResponse: No address Tlv");
        return false;
    }
    QString serviceAddress = QString(serviceAddressTlv.data());

    Tlv authCookieTlv = tlvs.firstTlv(TlvAuthCookie);
    if(!authCookieTlv.isValid())
    {
        log(L_WARN, "ServiceSnacHandler::handleServiceResponse: No cookie Tlv");
        return false;
    }
    QByteArray authCookie = authCookieTlv.data();

    log(L_DEBUG, "ServiceSnacHandler::handleServiceResponse emit");

    emit serviceAvailable(serviceId, serviceAddress, authCookie);

    return true;
}

bool ServiceSnacHandler::handleSelfInfoReply(const QByteArray& data)
{
    emit statusTransitionComplete();
    return true;
}

RateInfoPtr ServiceSnacHandler::readNextRateInfoClass(ByteArrayParser& parser)
{
    int id = parser.readWord();
    int windowSize = parser.readDword();
    int clearLevel = parser.readDword();
    int alertLevel = parser.readDword();
    int limitLevel = parser.readDword();
    int disconnectLevel = parser.readDword();
    int currentLevel = parser.readDword();
    int maxLevel = parser.readDword();
    int lastTime = parser.readDword();
    Q_UNUSED(lastTime);
    int currentState = parser.readByte();
    Q_UNUSED(currentState);
    log(L_DEBUG, "Rate info class: %04x", id);

    RateInfoPtr info = RateInfoPtr(new RateInfo(id));
    info->setWindowSize(windowSize);
    info->setLevels(clearLevel, alertLevel, limitLevel, disconnectLevel, maxLevel);
    info->setCurrentLevel(currentLevel);
    return info;
}

int ServiceSnacHandler::readNextRateInfoGroup(ByteArrayParser& parser)
{
    int id = parser.readWord();
    int entries = parser.readWord();
    log(L_DEBUG, "Rate info group: %04x, entries: %04x", id, entries);
    RateInfoPtr info = rateInfo(id);
    if(!info)
    {
        log(L_ERROR, "Invalid rate info packet: %d/%d", id, entries);
        return -1;
    }
    for(int entrynum = 0; entrynum < entries; entrynum++)
    {
        int type = parser.readWord();
        int subtype = parser.readWord();
        info->addSnac(type, subtype);
        //log(L_DEBUG, "  type: %04x, subtype: %04x", type, subtype);
    }
    return id;
}

bool ServiceSnacHandler::requestSelfInfo()
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);

    socket->snac(ICQ_SNACxFOOD_SERVICE, SnacServiceSelfInfoRequest, 0, QByteArray());
    return true;
}

void ServiceSnacHandler::sendLoginStatus()
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);
    m_loginSince = QDateTime::currentDateTime();

    ICQStatusPtr status = client()->currentStatus().dynamicCast<ICQStatus>();

    TlvList list;
    list.append(Tlv::fromUint32(TlvOnlineStatus, status->icqId()));

    socket->snac(getType(), SnacServiceSetStatus, 0, list.toByteArray());
}

void ServiceSnacHandler::sendClientReady()
{
    OscarSocket* socket = client()->oscarSocket();
    Q_ASSERT(socket);

    ByteArrayBuilder builder;
    builder.appendWord(0x0001);
    builder.appendWord(0x0004);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0013);
    builder.appendWord(0x0004);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0002);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0003);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0015);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0004);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0006);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x0009);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    builder.appendWord(0x000b);
    builder.appendWord(0x0001);
    builder.appendDword(0x0110164f);

    socket->snac(getType(), SnacServiceReady, 0, builder.getArray());
}

RateInfoPtr ServiceSnacHandler::rateInfo(int group) const
{
    foreach(const RateInfoPtr& info, m_rateInfoList)
    {
        if(info->group() == group)
            return info;
    }
    return RateInfoPtr();
}

bool ServiceSnacHandler::parseServiceStatus(const QByteArray& data)
{
    // TODO: parse this packet
    return true;
}
