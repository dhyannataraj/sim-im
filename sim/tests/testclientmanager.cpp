
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stubs/stubclient.h"

#include "clients/client.h"
#include "clients/clientmanager.h"
#include "contacts/contactlist.h"
#include "events/eventhub.h"
#include "services.h"
#include "simlib-testing.h"
#include "clients/standardclientmanager.h"

namespace
{
    using namespace SIM;

    class TestClientManager : public ::testing::Test
    {
    protected:
        ClientManager::Ptr clientManager;
        void SetUp()
        {
            auto s = makeMockServices();
            SIM::createEventHub();
            clientManager = ClientManager::Ptr(new StandardClientManager(s->profileManager(), s->protocolManager()));
            SIM::createContactList(s->profileManager(), clientManager);
        }

        void TearDown()
        {
            SIM::destroyContactList();
            SIM::destroyEventHub();
        }

        void testClientManipulation();
    };

    TEST_F(TestClientManager, ClientManipulation)
    {
        SIM::ClientPtr icqclient = SIM::ClientPtr(new StubObjects::StubClient(0, "ICQ.666666666"));
        SIM::ClientPtr jabberclient = SIM::ClientPtr(new StubObjects::StubClient(0, "Jabber.loh@jabber.org"));
        EXPECT_TRUE(icqclient->name() == QString("ICQ.666666666"));
        clientManager->addClient(icqclient);
        clientManager->addClient(jabberclient);
        SIM::ClientPtr client2 = clientManager->client("ICQ.666666666");
        EXPECT_TRUE(client2->name() == QString("ICQ.666666666"));

        SIM::ClientPtr nonexistant = clientManager->client("Nothing");
        EXPECT_TRUE(nonexistant.isNull());
    }
}

// vim: set expandtab:

