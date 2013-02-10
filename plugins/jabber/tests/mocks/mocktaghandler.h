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
        MOCK_METHOD0(streamOpened, void());
        MOCK_METHOD1(incomingStanza, void(const XmlElement::Ptr& element));
    };
}


#endif /* MOCKTAGHANDLER_H_ */
