/*
 * testjabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "../protocol/jabberauthenticationcontroller.h"

#include "gtest/gtest.h"

#include "mocks/mockjabbersocket.h"

#include <QSignalSpy>

namespace
{
    using namespace testing;

    class TestJabberAuthenticationController : public Test
    {
    public:
        virtual void SetUp()
        {
            sock = new MockObjects::MockJabberSocket();
            auth.setSocket(sock);
        }

        virtual void TearDown()
        {
            delete sock;
        }

		void startAuth()
		{
			EXPECT_CALL(*sock, send(StartsWith("<stream:stream")));

			auth.tlsHandshakeDone();

			QDomDocument doc;
			doc.setContent(QByteArray(R"(
				<stream:features>
				<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'>
				<required/>
				</starttls>
				<mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
				<mechanism>DIGEST-MD5</mechanism>
				<mechanism>PLAIN</mechanism>
				</mechanisms>
				</stream:features>
				)"));


		}

        JabberAuthenticationController auth;
        MockObjects::MockJabberSocket* sock;
    };

    TEST_F(TestJabberAuthenticationController, afterConnect_startsStream)
    {
        EXPECT_CALL(*sock, send(StartsWith("<stream:stream")));

        auth.connected();
    }

   TEST_F(TestJabberAuthenticationController, afterConnect_ifReceivedStartTlsRequest_sendsStartTls)
   {
	   EXPECT_CALL(*sock, send(StartsWith("<stream:stream")));
	   auth.connected();

	   EXPECT_CALL(*sock, send(StartsWith("<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>")));

	   QDomDocument doc;
	   doc.setContent(QByteArray(R"(
		   <stream:features>
			   <starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'>
				   <required/>
			   </starttls>
			   <mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>
				   <mechanism>DIGEST-MD5</mechanism>
				   <mechanism>PLAIN</mechanism>
			   </mechanisms>
		   </stream:features>
	   		   )"));

	   auth.startElement(doc.childNodes().at(0).toElement());
   }

   TEST_F(TestJabberAuthenticationController, tlsHandshakeDone_startsNewStream)
   {
	   EXPECT_CALL(*sock, send(StartsWith("<stream:stream")));

	   auth.tlsHandshakeDone();
   }

   TEST_F(TestJabberAuthenticationController, tlsHandshakeDone_emitsNewStreamSignal)
   {
	   EXPECT_CALL(*sock, send(StartsWith("<stream:stream")));
	   QSignalSpy spy(&auth, SIGNAL(newStream()));

	   auth.tlsHandshakeDone();

	   ASSERT_EQ(1, spy.count());
   }

   TEST_F(TestJabberAuthenticationController, newStream_ifDigestMd5Mechanism_useIt)
   {
	   startAuth();

	   EXPECT_CALL(*sock, send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>")));

	   auth.startElement(doc.childNodes().at(0).toElement());
   }

   TEST_F(TestJabberAuthenticationController, digestMd5_serverChallenge)
   {
	   startAuth();
	   EXPECT_CALL(*sock, send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>")));
	   auth.startElement(doc.childNodes().at(0).toElement());


	   QDomDocument doc;
	   QDomElement challenge = doc.createElementNS("urn:ietf:params:xml:ns:xmpp-sasl", "challenge");
	   doc.appendChild(challenge);
	   QByteArray challenge(R"(realm="somerealm",nonce="" )");
	   QDomText text = doc.createTextNode(QString::fromAscii(QByteArray::toBase64()));

   }

}

