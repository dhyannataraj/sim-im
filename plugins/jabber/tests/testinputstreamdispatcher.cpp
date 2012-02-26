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
        EXPECT_CALL(*handler, canHandle(QString("obviously-not-stream-stanza"))).WillRepeatedly(Return(false));
		
		buffer.write("<obviously-not-stream-stanza>");
        buffer.seek(0);

        dispatcher.newData();

		ASSERT_EQ(1, spy.count());
	}

    TEST_F(TestInputStreamDispatcher, nestedTagsInStream)
    {
        EXPECT_CALL(*handler, canHandle(QString("stream:features"))).WillRepeatedly(Return(true));
        EXPECT_CALL(*handler, startElement(Truly(
			[&](const QDomElement& root)
			{
				if(root.tagName() != "stream:features")	
					return false;
				if(root.childNodes().count() != 2)
					return false;
				if(root.childNodes().at(0).toElement().tagName() != "starttls")
					return false;
				if(root.childNodes().at(0).childNodes().at(0).toElement().tagName() != "required")
					return false;
				if(root.childNodes().at(1).toElement().tagName() != "mechanisms")
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

   TEST_F(TestInputStreamDispatcher, nestedTagsInStream_whenReturnsFromLevel2_UsesNewHandler)
   {
		// We will need another handler for this test
	   dispatcher.addTagHandler(iq_handler);

	   QXmlAttributes attrs;
	   // Order is important:
	   EXPECT_CALL(*handler, canHandle(_)).WillRepeatedly(Return(false));
	   EXPECT_CALL(*handler, canHandle(QString("stream:features"))).WillRepeatedly(Return(true));
	   EXPECT_CALL(*iq_handler, canHandle(_)).WillRepeatedly(Return(false));
	   EXPECT_CALL(*iq_handler, canHandle(QString("iq"))).WillRepeatedly(Return(true));

	   EXPECT_CALL(*handler, startElement(Truly(
			   [&](const QDomElement& root)
			   {
				   if(root.tagName() != "stream:features")	
					   return false;
				   return true;
			   }
			   )));

	   EXPECT_CALL(*iq_handler, startElement(Truly(
			   [&](const QDomElement& root)
			   {
				   if(root.tagName() != "iq")	
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

    TEST_F(TestInputStreamDispatcher, newStream_resetsLevel)
    {
        EXPECT_CALL(*handler, canHandle(QString("stream:features"))).WillRepeatedly(Return(true));
        EXPECT_CALL(*handler, startElement(_));

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
