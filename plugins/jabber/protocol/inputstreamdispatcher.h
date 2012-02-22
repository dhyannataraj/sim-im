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
#include <QXmlSimpleReader>

class InputStreamDispatcher : public QObject, public QXmlContentHandler
{
    Q_OBJECT
public:
    InputStreamDispatcher();
    virtual ~InputStreamDispatcher();

    void addTagHandler(const TagHandler::SharedPointer& ptr);

    void setDevice(QIODevice* device);

    // QXmlContentHandler interface
    virtual bool characters(const QString& ch);
    virtual bool endDocument();
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool endPrefixMapping(const QString& prefix);
    virtual QString errorString() const;
    virtual bool ignorableWhitespace(const QString& ch);
    virtual bool processingInstruction(const QString& target, const QString& data);
    virtual void setDocumentLocator(QXmlLocator* locator);
    virtual bool skippedEntity(const QString& name);
    virtual bool startDocument();
    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool startPrefixMapping(const QString& prefix, const QString& uri);

private slots:
    void newData();

private:
    QList<TagHandler::SharedPointer> m_handlers;
    QString m_currentTag;
    QIODevice* m_device;
    QXmlSimpleReader m_reader;
    QXmlInputSource* m_source;
};

#endif /* INPUTSTREAMDISPATCHER_H_ */
