/*
 * jabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "jabberauthenticationcontroller.h"
#include "log.h"

#include <QCryptographicHash>

using namespace SIM;

JabberAuthenticationController::JabberAuthenticationController() : m_state(Initial)
{
}

JabberAuthenticationController::~JabberAuthenticationController()
{
}

void JabberAuthenticationController::setUsername(const QString& username)
{
    m_username = username;
}

void JabberAuthenticationController::setPassword(const QString& password)
{
    m_password = password;
}

void JabberAuthenticationController::setHostname(const QString& hostname)
{
    m_hostname = hostname;
}

void JabberAuthenticationController::setSocket(JabberSocket* socket)
{
    m_socket = socket;
	connect(socket, SIGNAL(tlsHandshakeDone()), this, SLOT(tlsHandshakeDone()));
}

void JabberAuthenticationController::startAuthentication(const QString& host, int port)
{
    m_host = host;
    m_socket->connectToHost(host, port);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
}

void JabberAuthenticationController::connected()
{
    QString stream = QString("<stream:stream xmlns='jabber:client' "
            "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
            arg(m_host);
    m_socket->send(stream.toUtf8());
}

void JabberAuthenticationController::tlsHandshakeDone()
{
	log(L_DEBUG, "Tls handshake done");
    QString stream = QString("<stream:stream xmlns='jabber:client' "
            "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
            arg(m_host);
    m_socket->send(stream.toUtf8());
	m_state = ReadyToAuthenticate;

	emit newStream();
}

bool JabberAuthenticationController::canHandle(const QString& tagName) const
{
	if(tagName == "stream:features")
		return true;
	if((m_state == TlsNegotiation) && (tagName == "proceed"))
		return true;
	if(tagName == "challenge")
		return true;
	if(tagName == "success")
	    return true;
	return false;
}

void JabberAuthenticationController::startElement(const QDomElement& root)
{
	if(root.tagName() == "stream:features")
	{
		if(m_state == Initial)
		{
			bool tlsRequired = false;
			QDomElement starttls = root.elementsByTagName("starttls").at(0).toElement();
			if(!starttls.isNull())
			{
				tlsRequired = true;
			}

			if(tlsRequired)
			{
				m_socket->send("<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>");
				m_state = TlsNegotiation;
			}
			// TODO: auth without TLS
		}
		else if(m_state == ReadyToAuthenticate)
		{
			// TODO: implement various,mechanism
			m_socket->send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>"));
            m_state = DigestMd5WaitingChallenge;
		}
		else if(m_state == RestartingStream)
		{
		    log(L_DEBUG, "Features");
		}
	}
	else if((m_state == TlsNegotiation) && (root.tagName() == "proceed"))
	{
		m_socket->startTls();
	}
    else if((m_state == DigestMd5WaitingChallenge) && (root.tagName() == "challenge"))
    {
        QString challengeString = QString::fromUtf8(QByteArray::fromBase64(root.text().toAscii()));
		printf("Challenge: %s\n", qPrintable(challengeString));
		QString response = makeResponseToChallenge(challengeString);
        m_socket->send(response.toAscii());
		m_state = DigestMd5WaitingSecondChallenge;
    }
    else if((m_state == DigestMd5WaitingSecondChallenge) && (root.tagName() == "challenge"))
    {
		QString response("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
        m_socket->send(response.toAscii());
        m_state = DigestMd5WaitingSuccess;
    }
    else if((m_state == DigestMd5WaitingSuccess) && (root.tagName() == "success"))
    {
        log(L_DEBUG, "Successful authentication");
        m_state = RestartingStream;
        QString stream = QString("<stream:stream xmlns='jabber:client' "
                "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
                arg(m_host);
        m_socket->send(stream.toUtf8());
        emit newStream();
    }
}

void JabberAuthenticationController::endElement(const QString& name)
{
    log(L_DEBUG, "endElement(%s)", qPrintable(name));
}

void JabberAuthenticationController::characters(const QString& ch)
{

}

QString JabberAuthenticationController::makeResponseToChallenge(const QString& challengeString)
{
	QMap<QString, QString> map;
	QStringList entries = challengeString.split(',');
	QRegExp rx(R":((\w+)="?([^"-, ]*)"?):");
	foreach(const QString& s, entries)
	{
		if(rx.exactMatch(s))
		{
			map[rx.cap(1)] = rx.cap(2);
			log(L_DEBUG, "%s=%s", qPrintable(rx.cap(1)), qPrintable(rx.cap(2)));
		}
	}
	if(!map.contains("realm"))
		map["realm"] = m_hostname;
	QByteArray cnonce = QByteArray::number(qrand());
	QString nc = "00000001";

	QByteArray a1 = QCryptographicHash::hash(m_username.toUtf8() + ":" + map["realm"].toUtf8() + ":" + m_password.toUtf8(),
			QCryptographicHash::Md5) + QString(":" + map["nonce"].toUtf8() + ":" + cnonce).toUtf8();

	QByteArray a2 = (QString("AUTHENTICATE:") + QString("xmpp/%1").arg(m_hostname)).toAscii();

	log(L_DEBUG, "a1: %s; a2: %s", a1.data(), a2.data());

	QCryptographicHash result(QCryptographicHash::Md5);
	result.addData(QCryptographicHash::hash(a1, QCryptographicHash::Md5).toHex());
	result.addData(":", 1);
	result.addData(map["nonce"].toAscii());
	result.addData(":", 1);
	result.addData(nc.toAscii());
	result.addData(":", 1);
	result.addData(cnonce);
	result.addData(":", 1);
	result.addData(map["qop"].toAscii());
	result.addData(":", 1);
	result.addData(QCryptographicHash::hash(a2, QCryptographicHash::Md5).toHex());


	QString responseString = QString(R"(username="%1",realm="%2",nonce="%3",cnonce="%4",nc=%5,qop=%6,digest-uri="%7",response=%8,charset=utf-8)").arg(m_username).arg(map["realm"]).arg(map["nonce"]).arg(QString::fromAscii(cnonce)).
		arg(nc).arg(map["qop"]).arg("xmpp/" + m_hostname).arg(QString::fromAscii(result.result().toHex()));

	printf("Response: %s\n", qPrintable(responseString));
	QString response = QString("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>%1</response>").
		arg(QString::fromAscii(responseString.toUtf8().toBase64()));

	return response;
}

// vim : set expandtab
