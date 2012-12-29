
#ifndef SIMLIB_TESTING_H
#define SIMLIB_TESTING_H

#include "services.h"
#include "contacts/protocolmanager.h"
#include "mocks/mockclientmanager.h"

namespace SIM
{
    inline Services::Ptr makeMockServices()
    {
        auto s = Services::Ptr(new Services());
        s->setProtocolManager(ProtocolManager::Ptr(new ProtocolManager()));
        s->setClientManager(ClientManager::Ptr(new MockObjects::MockClientManager()));
        return s;
    }
}

#endif 

