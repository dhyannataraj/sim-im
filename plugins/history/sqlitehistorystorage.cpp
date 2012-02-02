#include "sqlitehistorystorage.h"
#include "profilemanager.h"
#include "sqlite3.h"
#include "log.h"
#include "contacts/imcontact.h"
#include <QDir>
#include <QSqlQuery>

SQLiteHistoryStorage::SQLiteHistoryStorage() : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    init();
}

SQLiteHistoryStorage::~SQLiteHistoryStorage()
{
}

void SQLiteHistoryStorage::addMessage(const SIM::MessagePtr& message)
{
	SIM::log(SIM::L_DEBUG, "Adding message to history");
	QSqlQuery query;
	query.prepare("INSERT INTO messages VALUES(?, ?, ?, ?, ?)");
	SIM::IMContactPtr source = message->sourceContact().toStrongRef();
	SIM::IMContactPtr target = message->sourceContact().toStrongRef();
	if((!source) && (!target))
	{
		SIM::log(SIM::L_WARN, "SQLiteHistoryStorage: unable to add message with nonexistant contact");
		return;
	}
	query.addBindValue(message->originatingClientId());
	query.addBindValue(source->id().toString());
	query.addBindValue(target->id().toString());
	query.addBindValue(message->toXml());
	query.addBindValue(message->timestamp().toTime_t());

	query.exec();

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
        SIM::log(SIM::L_WARN, "SQLiteHistoryStorage: Unable to open database");
        return;
    }
    createTables();
}

void SQLiteHistoryStorage::createTables()
{
	QSqlQuery query;
	query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='messages'");
	if(!query.first())
	{
		query.exec("CREATE TABLE messages (client_id TEXT, source_id TEXT, target_id TEXT, message TEXT, timestamp INTEGER)");
		SIM::log(SIM::L_DEBUG, "Creating message table");
	}
}
