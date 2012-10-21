/*
 * taghandler.h
 *
 *  Created on: Feb 11, 2012
 *      Author: todin
 */

#ifndef TAGHANDLER_H_
#define TAGHANDLER_H_

#include <QXmlAttributes>
#include <QSharedPointer>
#include <QDomElement>

class TagHandler
{
public:
    typedef QSharedPointer<TagHandler> SharedPointer;
    TagHandler();
    virtual ~TagHandler();

    virtual bool canHandle(const QString& tagName) const = 0;

    virtual void startElement(const QDomElement& root) = 0;
    virtual void endElement(const QString& name) = 0;
    virtual void characters(const QString& ch) = 0;
};

#endif /* TAGHANDLER_H_ */
