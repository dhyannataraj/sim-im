/*
 * testinputstreamdispatcher.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "../protocol/inputstreamdispatcher.h"
#include "mocks/mocktaghandler.h"
#include "gtest/gtest.h"

#include <QBuffer>

namespace
{
    using namespace testing;

    class TestInputStreamDispatcher : public Test
    {
    public:
        virtual void SetUp()
        {
            buffer.open(QIODevice::ReadWrite);
            handler = MockObjects::MockTagHandler::NiceSharedPointer(new NiceMock<MockObjects::MockTagHandler>);
			iq_handler = MockObjects::MockTagHandler::NiceSharedPointer(new NiceMock<MockObjects::MockTagHandler>);
            dispatcher.addTagHandler(handler);
            dispatcher.setDevice(&buffer);
        }

        virtual void TearDown()
        {

        }
        MockObjects::MockTagHandler::NiceSharedPointer handler;
        MockObjects::MockTagHandler::NiceSharedPointer iq_handler;
        InputStreamDispatcher dispatcher;
        QBuffer buffer;
    };

    TEST_F(TestInputStreamDispatcher, incomingXmlTag)
    {
        QXmlAttributes attrs;
        EXPECT_CALL(*handler, element()).WillRepeatedly(Return("stream"));
        EXPECT_CALL(*handler, startElement(QString("stream:stream"), _));
        buffer.write("<stream:stream to='somedomain.com'>");
        buffer.seek(0);

        dispatcher.newData();
    }

    TEST_F(TestInputStreamDispatcher, nestedTagsInStream)
    {
        QXmlAttributes attrs;
        EXPECT_CALL(*handler, element()).WillRepeatedly(Return("stream"));
        EXPECT_CALL(*handler, startElement(QString("stream:stream"), _));
        EXPECT_CALL(*handler, startElement(QString("stream:features"), _));
        EXPECT_CALL(*handler, startElement(QString("starttls"), _));
        EXPECT_CALL(*handler, startElement(QString("required"), _));
        EXPECT_CALL(*handler, startElement(QString("mechanisms"), _));
        EXPECT_CALL(*handler, startElement(QString("mechanism"), _)).Times(2);

		buffer.write(R"(<stream:stream to='somedomain.com'>
			<stream:features>
			<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'>
			<required/>
			</starttls>
			<mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
			<mechanism>DIGEST-MD5</mechanism>
			<mechanism>PLAIN</mechanism>
			</mechanisms>
			</stream:features>)");
        buffer.seek(0);

        dispatcher.newData();
    }

    TEST_F(TestInputStreamDispatcher, nestedTagsInStream_whenReturnsFromLevel2_UsesNewHandler)
    {
		// We will need another handler for this test
        dispatcher.addTagHandler(iq_handler);

        QXmlAttributes attrs;
        EXPECT_CALL(*handler, element()).WillRepeatedly(Return("stream"));
        EXPECT_CALL(*iq_handler, element()).WillRepeatedly(Return("iq"));
		{
			InSequence seq;
			// First tags should be handled by stream handler
			EXPECT_CALL(*handler, startElement(QString("stream:stream"), _));
			EXPECT_CALL(*handler, startElement(QString("stream:features"), _));
			EXPECT_CALL(*handler, startElement(QString("starttls"), _));
			EXPECT_CALL(*handler, startElement(QString("required"), _));
			EXPECT_CALL(*handler, startElement(QString("mechanisms"), _));
			EXPECT_CALL(*handler, startElement(QString("mechanism"), _)).Times(2);

			// And next (children of iq tag, and iq tag itself), should be handled by iq_handler
			EXPECT_CALL(*iq_handler, startElement(QString("iq"), _));
			EXPECT_CALL(*iq_handler, startElement(QString("session"), _));
			EXPECT_CALL(*iq_handler, startElement(QString("error"), _));
			EXPECT_CALL(*iq_handler, startElement(QString("internal-server-error"), _));
		}

		buffer.write(R"(<stream:stream to='somedomain.com'>
			<stream:features>
			<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'>
			<required/>
			</starttls>
			<mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
			<mechanism>DIGEST-MD5</mechanism>
			<mechanism>PLAIN</mechanism>
			</mechanisms>
			</stream:features>

			<iq from='example.com' type='error' id='sess_1'>
			<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>
			<error type='wait'>
			<internal-server-error
			xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'/>
			</error>
			</iq>
			)");
        buffer.seek(0);

        dispatcher.newData();
    }
}
