#ifndef HISTORYSTORAGE_H
#define HISTORYSTORAGE_H

#include "messaging/message.h"
#include <QSharedPointer>

class HistoryStorage
{
public:
    virtual ~HistoryStorage();
    virtual void addMessage(const SIM::MessagePtr& message) = 0;
    virtual QList<SIM::MessagePtr> getMessages(const QString& sourceContactId, const QString& targetContactId,
            const QDateTime& start, const QDateTime& end) = 0;
};

typedef QSharedPointer<HistoryStorage> HistoryStoragePtr;

#endif // HISTORYSTORAGE_H
