/*
 * inputstreamdispatcher.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "inputstreamdispatcher.h"

InputStreamDispatcher::InputStreamDispatcher() : m_source(0)
{
}

InputStreamDispatcher::~InputStreamDispatcher()
{
}

void InputStreamDispatcher::setDevice(QIODevice* device)
{
    m_device = device;
    if(m_source)
    {
        delete m_source;
        m_source = nullptr;
    }
    if(device)
    {
        m_source = new QXmlInputSource(device);
        m_reader.parse(m_source, true);
    }
}

void InputStreamDispatcher::newData()
{

}

void InputStreamDispatcher::addTagHandler(const TagHandler::SharedPointer& ptr)
{
    m_handlers.append(ptr);
}

bool InputStreamDispatcher::characters(const QString& ch)
{
    return true;
}

bool InputStreamDispatcher::endDocument()
{
    return true;
}

bool InputStreamDispatcher::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
    {
        if((*it)->element() == localName)
        {
            (*it)->endElement(qName);
            return true;
        }
    }
    return true;
}

bool InputStreamDispatcher::endPrefixMapping(const QString& prefix)
{
    return true;
}

QString InputStreamDispatcher::errorString() const
{
    return QString();
}

bool InputStreamDispatcher::ignorableWhitespace(const QString& ch)
{
    return true;
}

bool InputStreamDispatcher::processingInstruction(const QString& target, const QString& data)
{
    return true;
}

void InputStreamDispatcher::setDocumentLocator(QXmlLocator* locator)
{

}

bool InputStreamDispatcher::skippedEntity(const QString& name)
{
    return true;
}

bool InputStreamDispatcher::startDocument()
{
    return true;
}

bool InputStreamDispatcher::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
    for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
    {
        if((*it)->element() == localName)
        {
            (*it)->startElement(qName, atts);
            return true;
        }
    }
    return true;
}

bool InputStreamDispatcher::startPrefixMapping(const QString& prefix, const QString& uri)
{
    return true;
}
