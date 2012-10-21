/*
 * inputstreamdispatcher.h
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#ifndef INPUTSTREAMDISPATCHER_H_
#define INPUTSTREAMDISPATCHER_H_

#include <QIODevice>
#include <QObject>
#include "taghandler.h"
#include <QList>
#include <QXmlContentHandler>
#include <QXmlErrorHandler>
#include <QXmlSimpleReader>
#include <QDomElement>
#include <QDomDocument>

class InputStreamDispatcher : public QObject, public QXmlContentHandler, public QXmlErrorHandler
{
    Q_OBJECT
public:
    InputStreamDispatcher();
    virtual ~InputStreamDispatcher();

    void addTagHandler(const TagHandler::SharedPointer& ptr);
	int currentLevel() const;

    void setDevice(QIODevice* device);

    // QXmlContentHandler interface
    virtual bool characters(const QString& ch);
    virtual bool endDocument();
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool endPrefixMapping(const QString& prefix);
    virtual bool ignorableWhitespace(const QString& ch);
    virtual bool processingInstruction(const QString& target, const QString& data);
    virtual void setDocumentLocator(QXmlLocator* locator);
    virtual bool skippedEntity(const QString& name);
    virtual bool startDocument();
    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool startPrefixMapping(const QString& prefix, const QString& uri);

    // QXmlErrorHandler interface

    virtual bool error(const QXmlParseException& exception);
    virtual QString errorString() const;
    virtual bool fatalError(const QXmlParseException& exception);
    virtual bool warning(const QXmlParseException& exception );


public slots:
    void newData();
	void newStream();

signals:
	void error(const QString& errorMessage);

private:
    QList<TagHandler::SharedPointer> m_handlers;
    QIODevice* m_device;
    QXmlSimpleReader m_reader;
    QXmlInputSource* m_source;
    bool m_parsingStarted;
	bool m_hasTag;
	QDomDocument m_currentDocument;
	QDomElement m_currentRoot;
	QDomElement m_currentTag;
	int m_level;
};

#endif /* INPUTSTREAMDISPATCHER_H_ */
