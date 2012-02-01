#include "sqlitehistorystorage.h"
#include "profilemanager.h"
#include "sqlite3.h"
#include "log.h"
#include <QDir>

SQLiteHistoryStorage::SQLiteHistoryStorage() : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    init();
}

SQLiteHistoryStorage::~SQLiteHistoryStorage()
{
}

void SQLiteHistoryStorage::addMessage(const SIM::MessagePtr& message)
{

}

QList<SIM::MessagePtr> SQLiteHistoryStorage::getMessages(const QString& sourceContactId, const QString& targetContactId,
        const QDateTime& start, const QDateTime& end)
{
    return QList<SIM::MessagePtr>();
}

void SQLiteHistoryStorage::init()
{
    QString profileRoot = SIM::getProfileManager()->profilePath();
    m_db.setDatabaseName(profileRoot + QDir::separator() + "history.sqlitedb");

    bool ok = m_db.open();
    if(!ok)
    {
        SIM::log(SIM::L_DEBUG, "SQLiteHistoryStorage: Unable to open database");
        return;
    }
}
