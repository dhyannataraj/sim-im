/*
 * jabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "jabberauthenticationcontroller.h"
#include "log.h"

#include <QCryptographicHash>
#include <QStringList>

#include <future>

using namespace SIM;

JabberAuthenticationController::JabberAuthenticationController() : m_state(WaitingStreamStart),
    m_encrypted(false)
{
}

JabberAuthenticationController::~JabberAuthenticationController()
{
}

void JabberAuthenticationController::streamOpened()
{
    if(m_state == WaitingStreamStart)
        m_state = WaitingFeatures;

    emit newStream();
}

void JabberAuthenticationController::incomingStanza(const XmlElement::Ptr& element)
{
    switch(m_state)
    {
        case WaitingStreamStart:
            break;
        case WaitingFeatures:
            stateWaitingFeatures(element);
            break;
        case TlsNegotiation:
            stateTlsNegotiation(element);
            break;
		case DigestMd5WaitingChallenge:
            stateDigestMd5WaitingChallenge(element);
            break;
		case DigestMd5WaitingChallengeValidation:
            stateDigestMd5WaitingChallengeValidation(element);
            break;
		case WaitingResourceBinding:
            stateWaitingResourceBinding(element);
            break;
    }
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

void JabberAuthenticationController::setResource(const QString& resource)
{
    m_resource = resource;
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
    m_encrypted = false;
}

void JabberAuthenticationController::connected()
{
    QString stream = QString("<stream:stream xmlns='jabber:client' "
            "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
            arg(m_host);
    m_socket->send(stream.toUtf8());
	m_state = WaitingStreamStart;
}

void JabberAuthenticationController::tlsHandshakeDone()
{
	log(L_DEBUG, "Tls handshake done");
    QString stream = QString("<stream:stream xmlns='jabber:client' "
            "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
            arg(m_host);
    m_socket->send(stream.toUtf8());
	m_state = WaitingStreamStart;
    m_encrypted = true;

	emit newStream();
}

QString JabberAuthenticationController::makeResponseToChallenge(const QString& challengeString)
{
	QMap<QString, QString> map;
	QStringList entries = challengeString.split(',');
	QRegExp rx(R"((\w+)=\"?([^\", ]*)\"?)");
	foreach(const QString& s, entries)
	{
		if(rx.indexIn(s) >= 0)
		{
			map[rx.cap(1)] = rx.cap(2);
			log(L_DEBUG, "%s=%s", qPrintable(rx.cap(1)), qPrintable(rx.cap(2)));
		}
	}
	if(!map.contains("realm"))
		map["realm"] = m_hostname;
	QByteArray cnonce = QByteArray::number(qrand());
	QString nc = "00000001";

	QString authzid = m_username + "@" + m_hostname;

	QByteArray a1 = QCryptographicHash::hash(m_username.toUtf8() + ":" + map["realm"].toUtf8() + ":" + m_password.toUtf8(),
			QCryptographicHash::Md5) + QString(":" + map["nonce"].toUtf8() + ":" + cnonce + ":" + authzid).toUtf8();

	QByteArray a2 = (QString("AUTHENTICATE:") + QString("xmpp/%1").arg(m_hostname)).toAscii();

	//log(L_DEBUG, "username: %s; password: %s", qPrintable(m_username), qPrintable(m_password));
	log(L_DEBUG, "a1: %s; a2: %s", a1.toHex().data(), a2.toHex().data());

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


	QString responseString = QString("username=\"%1\",realm=\"%2\",nonce=\"%3\",cnonce=\"%4\",nc=%5,qop=%6,digest-uri=\"%7\",response=%8,charset=utf-8,authzid=\"%9\"")
        .arg(m_username).arg(map["realm"]).arg(map["nonce"]).arg(QString::fromAscii(cnonce)).
		arg(nc).arg(map["qop"]).arg("xmpp/" + m_hostname).arg(QString::fromAscii(result.result().toHex())).arg(authzid);

	log(L_DEBUG, "Response: %s\n", qPrintable(responseString));
	QString response = QString("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>%1</response>").
		arg(QString::fromAscii(responseString.toUtf8().toBase64()));

	return response;
}

void JabberAuthenticationController::handleFeatures(const QStringList& features)
{
    if(features.contains("tls"))
    {
        m_socket->send("<starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>");
        m_state = TlsNegotiation;
    }
    else if(features.contains("digest-md5"))
    {
        m_socket->send(QByteArray("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>"));
        m_state = DigestMd5WaitingChallenge;
    }
    else if(features.contains("bind"))
    {
        m_socket->send(QString("<iq id=\"bind1\" type=\"set\">"
                    "<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\">"
                    "<resource>%1</resource>"
                    "</bind>"
                    "</iq>").arg(m_resource).toUtf8());
        m_state = WaitingResourceBinding;
    }
    else
    {
        failure();
    }
}

void JabberAuthenticationController::stateWaitingFeatures(const XmlElement::Ptr& root)
{
    if(root->name() == "stream:features")
    {
        QStringList features;
        if(!m_encrypted)
        {
            auto starttls = root->firstChild("starttls");
            if(starttls)
            {
                features.append("tls");
            }
        }
        auto mechs = root->firstChild("mechanisms");
        if(mechs)
        {
            auto children = mechs->children();
            if(std::any_of(children.begin(), children.end(), [](const XmlElement::Ptr& el) { return el->text().contains("DIGEST-MD5"); }))
                features.append("digest-md5");
        }
        auto bind = root->firstChild("bind");
        if(bind)
        {
            features.append("bind");
        }

        handleFeatures(features);
    }
}

void JabberAuthenticationController::stateTlsNegotiation(const XmlElement::Ptr& root)
{
    if(root->name() == "proceed")
        m_socket->startTls();
}

void JabberAuthenticationController::stateDigestMd5WaitingChallenge(const XmlElement::Ptr& root)
{
    if(root->name() == "challenge")
    {
        QString challengeString = QString::fromUtf8(QByteArray::fromBase64(root->text().toAscii()));
        QString response = makeResponseToChallenge(challengeString);
        m_socket->send(response.toAscii());
        m_state = DigestMd5WaitingChallengeValidation;
    }
}

void JabberAuthenticationController::stateDigestMd5WaitingChallengeValidation(const XmlElement::Ptr& root)
{
    if(root->name() == "challenge")
    {
        QString response("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
        m_socket->send(response.toAscii());
    }
    else if(root->name() == "success")
    {
        QString stream = QString("<stream:stream xmlns='jabber:client' "
                "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'>").
            arg(m_host);
        m_socket->send(stream.toUtf8());
        emit newStream();
        m_state = WaitingStreamStart;
    }
}

void JabberAuthenticationController::stateWaitingResourceBinding(const XmlElement::Ptr& root)
{
    if(root->name() == "iq")
    {
        auto type = root->attribute("type");
        log(L_DEBUG, "[%s]", qPrintable(type));
        if(type != "result")
        {
            log(L_WARN, "Resource binding: error");
            return;
        }
        auto el = root->firstChild("bind")->firstChild("jid");
        if(!el || (el->text().isEmpty()))
        {
            log(L_WARN, "Resource binding: error: no jid");
            return;
        }

        m_fullJid = el->text();
        log(L_DEBUG, "Authentication completed, JID: %s", qPrintable(m_fullJid));
        emit authenticationCompleted();
    }
}

void JabberAuthenticationController::failure()
{
    m_state = Error;
}


// vim : set expandtab
