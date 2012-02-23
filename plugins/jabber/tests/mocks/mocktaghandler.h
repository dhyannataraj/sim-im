/*
 * mocktaghandler.h
 *
 *  Created on: Feb 22, 2012
 *      Author: todin
 */

#ifndef MOCKTAGHANDLER_H_
#define MOCKTAGHANDLER_H_

#include "gmock/gmock.h"
#include "protocol/taghandler.h"

#include <QSharedPointer>

namespace MockObjects
{
    class MockTagHandler : public TagHandler
    {
    public:
        typedef QSharedPointer<MockTagHandler> SharedPointer;
		typedef QSharedPointer<testing::NiceMock<MockTagHandler>> NiceSharedPointer;
        MOCK_CONST_METHOD0(element, QString());
        MOCK_METHOD2(startElement, void(const QString& name, const QXmlAttributes&));
        MOCK_METHOD1(endElement, void(const QString& name));
        MOCK_METHOD1(characters, void(const QString& ch));
    };
}


#endif /* MOCKTAGHANDLER_H_ */
