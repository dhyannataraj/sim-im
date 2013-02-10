/*
 * taghandler.h
 *
 *  Created on: Feb 11, 2012
 *      Author: todin
 */

#ifndef TAGHANDLER_H_
#define TAGHANDLER_H_

#include <QSharedPointer>
#include "xmlelement.h"

class TagHandler
{
public:
    typedef QSharedPointer<TagHandler> SharedPointer;
    TagHandler();
    virtual ~TagHandler();
    
    virtual void streamOpened() = 0;
    virtual void incomingStanza(const XmlElement::Ptr& element) = 0;
};

#endif /* TAGHANDLER_H_ */
