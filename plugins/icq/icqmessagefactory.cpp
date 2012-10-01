/*
 * icqmessagefactory.cpp
 *
 *  Created on: Aug 16, 2011
 */

#include "icqmessagefactory.h"
#include "messaging/genericmessage.h"

ICQMessageFactory::ICQMessageFactory()
{

}

ICQMessageFactory::~ICQMessageFactory()
{
}

SIM::MessagePtr ICQMessageFactory::createMessage(const QString& messageTypeId)//messageTypeId not used!
{
    return SIM::MessagePtr();
}
