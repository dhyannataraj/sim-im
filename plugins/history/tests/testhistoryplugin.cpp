/*
 * testhistoryplugin.cpp
 *
 *  Created on: Dec 3, 2011
 */


#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "messaging/messagepipe.h"
#include "messaging/messageoutpipe.h"
#include "messaging/genericmessage.h"
#include "tests/mocks/mockmessagepipe.h"
#include "mocks/mockhistorystorage.h"

#include "historyplugin.h"

namespace 
{
    using ::testing::_;
    using ::testing::NiceMock;
    class TestHistoryPlugin : public ::testing::Test
    {
    public:
        virtual void SetUp()
        {
            SIM::createMessagePipe();
            SIM::createOutMessagePipe();
        }

        virtual void TearDown()
        {
            SIM::destroyMessagePipe();
            SIM::destroyOutMessagePipe();
        }
        NiceMock<MockObjects::MockMessagePipe>* inPipe;
        NiceMock<MockObjects::MockMessagePipe>* outPipe;

        void createMockPipes()
        {
            inPipe = new NiceMock<MockObjects::MockMessagePipe>();
            SIM::setMessagePipe(inPipe);

            outPipe = new NiceMock<MockObjects::MockMessagePipe>();
            SIM::setOutMessagePipe(outPipe);
        }

        SIM::MessagePtr createMessage()
        {
            return SIM::MessagePtr(new SIM::GenericMessage("foo", "bar", "clientId", "body"));
        }
    };

    TEST_F(TestHistoryPlugin, constructor_installs_itself_to_message_in_pipe)
    {
        createMockPipes();
        EXPECT_CALL(*inPipe, addMessageProcessor(_));
        HistoryPlugin history;
    }

    TEST_F(TestHistoryPlugin, constructor_installs_itself_to_message_out_pipe)
    {
        createMockPipes();
        EXPECT_CALL(*outPipe, addMessageProcessor(_));
        HistoryPlugin history;
    }

    TEST_F(TestHistoryPlugin, destructor_removes_itself_from_message_in_pipe)
    {
        createMockPipes();
        EXPECT_CALL(*inPipe, removeMessageProcessor(QString("history")));
        HistoryPlugin history;
    }

    TEST_F(TestHistoryPlugin, destructor_removes_itself_from_message_out_pipe)
    {
        createMockPipes();
        EXPECT_CALL(*outPipe, removeMessageProcessor(QString("history")));
        HistoryPlugin history;
    }

    TEST_F(TestHistoryPlugin, stores_incoming_messages)
    {
        HistoryPlugin history;
        MockObjects::MockHistoryStoragePtr storage = MockObjects::MockHistoryStoragePtr(new MockObjects::MockHistoryStorage());
        EXPECT_CALL(*storage.data(), addMessage(_));
        history.setHistoryStorage(storage);

        SIM::getMessagePipe()->pushMessage(createMessage());
    }

    TEST_F(TestHistoryPlugin, stores_outcoming_messages)
    {
        HistoryPlugin history;
        MockObjects::MockHistoryStoragePtr storage = MockObjects::MockHistoryStoragePtr(new MockObjects::MockHistoryStorage());
        EXPECT_CALL(*storage.data(), addMessage(_));
        history.setHistoryStorage(storage);

        SIM::getOutMessagePipe()->pushMessage(createMessage());
    }
}
