/*
 * mockhistorystorage.h
 *
 *  Created on: Dec 4, 2011
 */

#ifndef MOCKHISTORYSTORAGE_H_
#define MOCKHISTORYSTORAGE_H_

#include "historystorage.h"
#include <QSharedPointer>

namespace MockObjects
{
    class MockHistoryStorage : public HistoryStorage
    {
    public:
        MOCK_METHOD1(addMessage, void(const SIM::MessagePtr& message));
        MOCK_METHOD4(getMessages, QList<SIM::MessagePtr>(const QString& sourceContactId, const QString& targetContactId,
                const QDateTime& start, const QDateTime& end));
    };

    typedef QSharedPointer<MockHistoryStorage> MockHistoryStoragePtr;

}


#endif /* MOCKHISTORYSTORAGE_H_ */
