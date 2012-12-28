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
#include <QSignalSpy>

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

    TEST_F(TestInputStreamDispatcher, ifFirstStanzaIsNotStream_signalsAnError)
    {
		QSignalSpy spy(&dispatcher, SIGNAL(error(QString)));
		
		buffer.write("<obviously-not-stream-stanza>");
        buffer.seek(0);

        dispatcher.newData();

		ASSERT_EQ(1, spy.count());
	}

    TEST_F(TestInputStreamDispatcher, nestedTagsInStream)
    {
        EXPECT_CALL(*handler, incomingStanza(Truly(
			[&](const XmlElement::Ptr& root)
			{
				if(root->name() != "stream:features")	
					return false;
				if(root->children().count() != 2)
					return false;
				if(root->children().at(0)->name() != "starttls")
					return false;
				if(root->children().at(0)->children().at(0)->name() != "required")
					return false;
				if(root->children().at(1)->name() != "mechanisms")
					return false;
				return true;
			}
			)));

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

    TEST_F(TestInputStreamDispatcher, newStream_resetsLevel)
    {
        EXPECT_CALL(*handler, incomingStanza(_));

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

		ASSERT_EQ(1, dispatcher.currentLevel());

		dispatcher.newStream();

		ASSERT_EQ(0, dispatcher.currentLevel());
    }
}
