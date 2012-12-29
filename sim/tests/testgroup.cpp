#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QDomDocument>
#include <QDomElement>

#include "contacts/group.h"
#include "stubs/stubimgroup.h"
#include "stubs/stubclient.h"
#include "clients/clientmanager.h"
#include "services.h"

namespace
{
    using namespace SIM;
    class TestGroup : public ::testing::Test
    {
    protected:

        ClientPtr createStubClient(const QString& id)
        {
            return ClientPtr(new StubObjects::StubClient(0, id));
        }

        IMGroupPtr createStubIMGroup(const ClientPtr& client)
        {
            return IMGroupPtr(new StubObjects::StubIMGroup(client.data()));
        }

        Services::Ptr services;
        
        virtual void SetUp()
        {
            services = makeMockServices();
            SIM::createClientManager(services->protocolManager());
        }
    };

    TEST_F(TestGroup, clientGroup_IfGroupIsAdded_ReturnsGroup)
    {
        ClientPtr client = createStubClient("ICQ.123456");
        IMGroupPtr imGroup = createStubIMGroup(client);
        Group gr(1);

        gr.addClientGroup(imGroup);

        IMGroupPtr returnedGroup = gr.clientGroup("ICQ.123456");
        ASSERT_TRUE(imGroup == returnedGroup);
    }

    TEST_F(TestGroup, clientGroup_IfGroupIsntAdded_ReturnsNullPointer)
    {
        Group gr(1);

        IMGroupPtr returnedGroup = gr.clientGroup("XMPP.bad@motherfucker.com");
        ASSERT_TRUE(!returnedGroup);
    }

    TEST_F(TestGroup, clientIds_ReturnsNamesOfClients)
    {
        ClientPtr client = createStubClient("ICQ.123456");
        IMGroupPtr imGroup = createStubIMGroup(client);
        Group gr(1);

        gr.addClientGroup(imGroup);

        EXPECT_EQ(1, gr.clientIds().size());
        EXPECT_TRUE(gr.clientIds().contains("ICQ.123456"));
    }

    TEST_F(TestGroup, LoadSaveState)
    {
        ClientPtr client = createStubClient("ICQ.123456");
        IMGroupPtr imGroup = createStubIMGroup(client);
        Group gr(1);
        gr.setName("Foo");

        gr.addClientGroup(imGroup);
        PropertyHubPtr groupState = gr.saveState();
        Group deserializedGroup(1);
        deserializedGroup.loadState(groupState);

        ASSERT_TRUE(deserializedGroup.name() == "Foo");
    }

    TEST_F(TestGroup, loadStateFromEmptyPropertyHub)
    {
        PropertyHubPtr testHub;
        Group gr(1);

        ASSERT_FALSE(gr.loadState(testHub));
    }

    TEST_F(TestGroup, loadState_IncorrectPropertyHub_NoUserData)
    {
        PropertyHubPtr testHub = PropertyHub::create("groups");
        testHub->addPropertyHub(PropertyHub::create("clients"));
        Group gr(1);

        ASSERT_FALSE(gr.loadState(testHub));
    }

    TEST_F(TestGroup, loadState_IncorrectPropertyHub_NoClients)
    {
        PropertyHubPtr testHub = PropertyHub::create("groups");
        testHub->addPropertyHub(PropertyHub::create("userdata"));
        Group gr(1);

        ASSERT_FALSE(gr.loadState(testHub));
    }

}
