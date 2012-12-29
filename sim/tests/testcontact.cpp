#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QDomDocument>
#include <QDomElement>

#include "contacts/contact.h"
#include "clients/client.h"
#include "clients/standardclientmanager.h"
#include "stubs/stubimcontact.h"
#include "stubs/stubclient.h"
#include "mocks/mockimcontact.h"
#include "mocks/mockimstatus.h"
#include "clients/clientmanager.h"
#include "messaging/genericmessage.h"
#include "services.h"
#include "simlib-testing.h"

namespace
{
    using namespace SIM;
    using namespace MockObjects;
    using ::testing::NiceMock;
    using ::testing::Return;
    using ::testing::_;

    class TestContact : public ::testing::Test
    {
    protected:
        ClientManager::Ptr clientManager;

        ClientPtr createStubClient(const QString& id)
        {
            return ClientPtr(new StubObjects::StubClient(0, id));
        }

        IMContactPtr createStubIMContact(const ClientPtr& client)
        {
            return IMContactPtr(new StubObjects::StubIMContact(client.data()));
        }

        MessagePtr createGenericMessage(const IMContactPtr& contact)
        {
            return MessagePtr(new GenericMessage(IMContactPtr(), contact, ""));
        }

        void fillContactData(Contact& c)
        {
            c.setName("Foo");
            c.setNotes("Bar");
            c.setGroupId(42);
            c.setFlag(Contact::flIgnore, true);
            c.setLastActive(112);
        }

        Services::Ptr services;
        
        virtual void SetUp()
        {
            services = makeMockServices();
            clientManager = ClientManager::Ptr(new StandardClientManager(services->protocolManager()));
        }
    };

    TEST_F(TestContact, clientContact_IfContactIsAdded_ReturnsAddedContact)
    {
        ClientPtr client =  createStubClient("ICQ.123456");
        IMContactPtr imContact = createStubIMContact(client);
        Contact contact(1);
        contact.addClientContact(imContact);

        IMContactPtr returnedContact = contact.clientContact("ICQ.123456");
        EXPECT_TRUE(returnedContact == imContact);
    }

    TEST_F(TestContact, clientContact_IfContactIsntAdded_ReturnsNullPointer)
    {
        Contact contact(1);

        IMContactPtr returnedContact = contact.clientContact("XMPP.bad@motherfucker.com");
        EXPECT_TRUE(!returnedContact);
    }

    TEST_F(TestContact, clientContactNames_ReturnsNamesOfClients)
    {
        ClientPtr client =  createStubClient("ICQ.123456");
        IMContactPtr imContact = createStubIMContact(client);
        Contact contact(1);
        contact.addClientContact(imContact);

        EXPECT_EQ(1, contact.clientContactNames().size());
        EXPECT_TRUE(contact.clientContactNames().contains("ICQ.123456"));
    }

    TEST_F(TestContact, hasUnreadMessages)
    {
        ClientPtr client =  createStubClient("ICQ.123456");
        auto imContact = MockObjects::NiceMockIMContactPtr(new NiceMock<MockObjects::MockIMContact>());
        EXPECT_CALL(*imContact.data(), hasUnreadMessages()).WillOnce(Return(true));
        Contact contact(1);
        contact.addClientContact(imContact);

        EXPECT_TRUE(contact.hasUnreadMessages());
    }

    TEST_F(TestContact, SerializationAndDeserialization)
    {
        ClientPtr client =  createStubClient("ICQ.123456");
        IMContactPtr imContact = createStubIMContact(client);
        Contact contact(1);
        contact.addClientContact(imContact);
        fillContactData(contact);
        QDomDocument doc;
        QDomElement el = doc.createElement("contact");

        contact.serialize(el);
        Contact deserializedContact(1);
        deserializedContact.deserialize(clientManager, el);

        EXPECT_TRUE(deserializedContact.name() == "Foo");
        EXPECT_TRUE(deserializedContact.notes() == "Bar");
        EXPECT_TRUE(deserializedContact.groupId() == 42);
        EXPECT_TRUE(deserializedContact.lastActive() == 112);
        EXPECT_TRUE(deserializedContact.flag(Contact::flIgnore));
    }

    TEST_F(TestContact, isOnline_emptyContact_offline)
    {
        Contact contact(1);

        ASSERT_FALSE(contact.isOnline());
    }

    TEST_F(TestContact, isOnline_subcontactOnline)
    {
        MockIMStatusPtr imStatus = MockIMStatusPtr(new MockIMStatus());
        auto imContact = MockObjects::NiceMockIMContactPtr(new NiceMock<MockObjects::MockIMContact>());
        ON_CALL(*imContact.data(), status()).WillByDefault(Return(imStatus));
        EXPECT_CALL(*imStatus.data(), flag(IMStatus::flOffline)).WillRepeatedly(Return(false));
        Contact contact(1);
        contact.addClientContact(imContact);

        ASSERT_TRUE(contact.isOnline());
    }

    TEST_F(TestContact, loadSateFromEmptyPropertyHub)
    {
        PropertyHubPtr testHub;
        Contact contact(1);

        EXPECT_FALSE(contact.loadState(clientManager, testHub));
    }

    TEST_F(TestContact, loadState_IncorrectPropertyHub_NoUserData)
    {
        PropertyHubPtr testHub = PropertyHub::create("groups");
        testHub->addPropertyHub(PropertyHub::create("clients"));
        Contact contact(1);

        EXPECT_FALSE(contact.loadState(clientManager, testHub));
    }

    TEST_F(TestContact, loadState_IncorrectPropertyHub_NoClients)
    {
        PropertyHubPtr testHub = PropertyHub::create("groups");
        testHub->addPropertyHub(PropertyHub::create("userdata"));
        Contact contact(1);

        EXPECT_FALSE(contact.loadState(clientManager, testHub));
    }
}

// vim: set expandtab:

