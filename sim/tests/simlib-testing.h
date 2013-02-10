
#ifndef SIMLIB_TESTING_H
#define SIMLIB_TESTING_H

#include "services.h"
#include "contacts/protocolmanager.h"
#include "mocks/mockclientmanager.h"
#include "mocks/mockprofilemanager.h"

namespace SIM
{
    inline Services::Ptr makeMockServices()
    {
        auto s = Services::Ptr(new Services());
        s->setProtocolManager(ProtocolManager::Ptr(new ProtocolManager()));
        s->setClientManager(ClientManager::Ptr(new testing::NiceMock<MockObjects::MockClientManager>()));
        s->setProfileManager(ProfileManager::Ptr(new testing::NiceMock<MockObjects::MockProfileManager>()));
        return s;
    }
}

#endif 

