/*
 * inputstreamdispatcher.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "inputstreamdispatcher.h"
#include "log.h"
#include <cstdio>

using namespace SIM;

InputStreamDispatcher::InputStreamDispatcher() : m_source(0),
        m_parsingStarted(false), m_level(0), m_hasTag(false)
{
    m_reader.setContentHandler(this);
    m_reader.setErrorHandler(this);
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
    }
}

void InputStreamDispatcher::newData()
{
    if(!m_parsingStarted)
    {
        bool rc = m_reader.parse(m_source, true);
        m_parsingStarted = true;
    }
    else
    {
        bool rc = m_reader.parseContinue();
    }
}

void InputStreamDispatcher::newStream()
{
	m_level = 0;
}

void InputStreamDispatcher::addTagHandler(const TagHandler::SharedPointer& ptr)
{
    m_handlers.append(ptr);
}

int InputStreamDispatcher::currentLevel() const
{
	return m_level;
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
    //printf("endElement(%s, %s, %s)\n", qPrintable(namespaceURI), qPrintable(localName), qPrintable(qName));
	m_level--;
	if(m_level == 1)
	{
		QString currentTag = m_currentRoot.tagName();
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			if((*it)->canHandle(currentTag))
			{
				(*it)->startElement(m_currentRoot);
				return true;
			}
		}
	}
	m_currentTag = m_currentTag.parentNode().toElement();
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
    log(L_DEBUG, "startElement(%s, %s, %s / %d)", qPrintable(namespaceURI), qPrintable(localName), qPrintable(qName), m_level);
	
	// m_level tracks current nesting level. The logic is as follows:
	// 0th level is global
	// 1st is stream:stream which should be hanlded by 'stream' tag handler
	// 2nd level may be: stream:features and it's children or other xmpp tags
	// If we encounter a tag at 2nd level, we store it in the m_currentTag
	// and if we encounter a child of this tag, we will call a handler of this tag,
	// with child's tag name
	
	m_level++;
	if(m_level == 2)
	{
		for(auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
		{
			if((*it)->canHandle(qName))
			{
				m_currentDocument.clear();
				m_hasTag = true;
				m_currentRoot = m_currentDocument.createElement(qName);
				m_currentTag = m_currentRoot;
				m_currentDocument.appendChild(m_currentRoot);
				return true;
			}
		}
		log(L_WARN, "Unknown level2 tag: %s", qPrintable(qName));
	}
	else if(m_level > 2)
	{
		if(!m_hasTag)
			return true;

		QDomElement newChild = m_currentDocument.createElement(qName);
		m_currentTag.appendChild(newChild);
		m_currentTag = newChild;
	}
	else if(m_level == 1)
	{
		// Opening tag
		if(qName != "stream:stream")
		{
			emit error(tr("Invalid stream start"));
			return false;
		}
	}
    return true;
}

bool InputStreamDispatcher::startPrefixMapping(const QString& prefix, const QString& uri)
{
    return true;
}

bool InputStreamDispatcher::error(const QXmlParseException& exception)
{
    log(L_DEBUG, "Error: %s", qPrintable(exception.message()));
    return true;
}

bool InputStreamDispatcher::fatalError(const QXmlParseException& exception)
{
    log(L_DEBUG, "fatal Error: %s", qPrintable(exception.message()));
    return true;
}

bool InputStreamDispatcher::warning(const QXmlParseException& exception)
{
    log(L_DEBUG, "warning: %s", qPrintable(exception.message()));
    return true;
}

