#ifndef SQLITEHISTORYSTORAGE_H
#define SQLITEHISTORYSTORAGE_H

#include "historystorage.h"
#include <qsqldatabase.h>
#include "clients/clientmanager.h"

class SQLiteHistoryStorage : public HistoryStorage
{
public:
    explicit SQLiteHistoryStorage(const SIM::ClientManager::Ptr& clientManager);
    virtual ~SQLiteHistoryStorage();

    virtual void addMessage(const SIM::MessagePtr& message);
    virtual QList<SIM::MessagePtr> getMessages(const QString& sourceContactId, const QString& targetContactId,
            const QDateTime& start, const QDateTime& end);

private:
    void init();
    void createTables();

    QSqlDatabase m_db;
    SIM::ClientManager::Ptr m_clientManager;
};

#endif // SQLITEHISTORYSTORAGE_H
