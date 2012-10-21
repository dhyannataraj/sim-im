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
#include <QCryptographicHash>

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

            QDomDocument startAuth()
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

                return doc;
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
        QDomDocument doc = startAuth();

        EXPECT_CALL(*sock, send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>")));

        auth.startElement(doc.childNodes().at(0).toElement());
    }

	static void sendChallengeTag(JabberAuthenticationController& auth, const QString& s)
	{
		QString challenge = QString("<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>%1</challenge>").arg(QString::fromAscii(s.toAscii().toBase64()));
        QDomDocument doc;
		doc.setContent(challenge);
        QDomElement challengeElement = doc.elementsByTagName("challenge").at(0).toElement();
        auth.startElement(challengeElement);
	}

    TEST_F(TestJabberAuthenticationController, digestMd5_serverChallenge)
    {
        QDomDocument doc = startAuth();
        EXPECT_CALL(*sock, send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>")));
        auth.startElement(doc.childNodes().at(0).toElement());

        auth.setUsername("testusername");
        auth.setPassword("testpassword");
        auth.setHostname("test.com");


        EXPECT_CALL(*sock, send(Truly([&] (const QByteArray& arr)
                        {
                            QRegExp responseRx("<response[^>]*>(.*)</response>");
                            if(!responseRx.exactMatch(QString::fromUtf8(arr)))
                                return false;

                            QMap<QString, QString> map; QString s = QString::fromUtf8(
                                QByteArray::fromBase64(responseRx.cap(1).toAscii()));

                            QStringList entries = s.split(',');
                            QRegExp rx(R":(([\w\-]+)="?([^"-, ]*)"?):");
                            foreach(const QString& s, entries)
                            {
                                if(rx.exactMatch(s))
                                {
                                    map[rx.cap(1)] = rx.cap(2);
                                }
                            }

                            if(!map.keys().contains("cnonce"))
                                return false;
                            if(map["nonce"] != "OA6MG9tEQGm2hh")
                                return false;
                            if(map["realm"] != "somerealm")
                                return false;
                            if(map["nc"] != "00000001")
                                return false;
                            if(map["qop"] != "auth")
                                return false;

                            QByteArray a1 = QCryptographicHash::hash("testusername:somerealm:testpassword",
                                    QCryptographicHash::Md5) + ":OA6MG9tEQGm2hh:" + map["cnonce"].toAscii();

                            QByteArray a2 = (QString("AUTHENTICATE:") + map["digest-uri"]).toAscii();
                         
                            QCryptographicHash result(QCryptographicHash::Md5);
                            result.addData(QCryptographicHash::hash(a1, QCryptographicHash::Md5).toHex());
                            result.addData(":", 1);
                            result.addData(map["nonce"].toAscii());
                            result.addData(":", 1);
                            result.addData(map["nc"].toAscii());
                            result.addData(":", 1);
                            result.addData(map["cnonce"].toAscii());
                            result.addData(":", 1);
                            result.addData(map["qop"].toAscii());
                            result.addData(":", 1);
                            result.addData(QCryptographicHash::hash(a2, QCryptographicHash::Md5).toHex());


                            if(result.result().toHex() != map["response"])
                                return false;

                            return true;

                        })));

		sendChallengeTag(auth, R"(realm="somerealm",nonce="OA6MG9tEQGm2hh",qop="auth",charset=utf-8,algorithm=md5-sess)");

    }

    TEST_F(TestJabberAuthenticationController, digestMd5_secondServerChallenge)
    {
        QDomDocument doc = startAuth();
        EXPECT_CALL(*sock, send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>")));
        auth.startElement(doc.childNodes().at(0).toElement());

        auth.setUsername("testusername");
        auth.setPassword("testpassword");
        auth.setHostname("test.com");

		{
			InSequence seq;
			EXPECT_CALL(*sock, send(_));

			EXPECT_CALL(*sock, send(StartsWith("<response")));
		}
		sendChallengeTag(auth, R"(realm="somerealm",nonce="OA6MG9tEQGm2hh",qop="auth",charset=utf-8,algorithm=md5-sess)");
		sendChallengeTag(auth, R"(rspauth=blahblahblah)");
	}

}

// vim : set expandtab

