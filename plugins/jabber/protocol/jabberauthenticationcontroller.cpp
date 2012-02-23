/*
 * jabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "jabberauthenticationcontroller.h"
#include "log.h"

using namespace SIM;

JabberAuthenticationController::JabberAuthenticationController()
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

QString JabberAuthenticationController::element() const
{
    return "stream";
}

void JabberAuthenticationController::startElement(const QString& name, const QXmlAttributes& attr)
{
    log(L_DEBUG, "startElement(%s)", qPrintable(name));
}

void JabberAuthenticationController::endElement(const QString& name)
{
    log(L_DEBUG, "endElement(%s)", qPrintable(name));
}

void JabberAuthenticationController::characters(const QString& ch)
{

}
