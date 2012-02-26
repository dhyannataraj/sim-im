/*
 * jabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "jabberauthenticationcontroller.h"
#include "log.h"

using namespace SIM;

JabberAuthenticationController::JabberAuthenticationController() : m_state(Initial)
{
}

JabberAuthenticationController::~JabberAuthenticationController()
{
}

void JabberAuthenticationController::setSocket(JabberSocket* socket)
{
    m_socket = socket;
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
		}
	}
	else if((m_state == TlsNegotiation) && (root.tagName() == "proceed"))
	{
		m_socket->startTls();
	}
}

void JabberAuthenticationController::endElement(const QString& name)
{
    log(L_DEBUG, "endElement(%s)", qPrintable(name));
}

void JabberAuthenticationController::characters(const QString& ch)
{

}
