/*
 * iqcontroller.cpp
 */

#include "iqcontroller.h"

IqController::IqController() : m_currentId(0),
	m_socket(nullptr)
{
}

IqController::~IqController()
{
}

void IqController::setSocket(JabberSocket* socket)
{
	m_socket = socket;
}

bool IqController::canHandle(const QString& tagName) const
{
    return tagName == "iq";
}

void IqController::startElement(const QDomElement& root)
{

}

void IqController::endElement(const QString& name)
{
}

void IqController::characters(const QString& ch)
{
}

void IqController::sendRequest(const IqRequestPtr& request)
{
	Q_ASSERT(m_socket);

	request->setId(m_currentId++);

	//m_requests.append( {request->id(), 0, request} );

}

