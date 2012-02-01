#ifndef SQLITEHISTORYSTORAGE_H
#define SQLITEHISTORYSTORAGE_H

#include "historystorage.h"
#include <qsqldatabase.h>

class SQLiteHistoryStorage : public HistoryStorage
{
public:
    explicit SQLiteHistoryStorage();
    virtual ~SQLiteHistoryStorage();

    virtual void addMessage(const SIM::MessagePtr& message);
    virtual QList<SIM::MessagePtr> getMessages(const QString& sourceContactId, const QString& targetContactId,
            const QDateTime& start, const QDateTime& end);

private:
    void init();

    QSqlDatabase m_db;
};

#endif // SQLITEHISTORYSTORAGE_H
