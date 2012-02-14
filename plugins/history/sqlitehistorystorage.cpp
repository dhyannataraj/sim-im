#include "sqlitehistorystorage.h"
#include "profilemanager.h"
//#include "sqlite3.h"
#include "log.h"
#include "contacts/imcontact.h"
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "messaging/message.h"
#include "messaging/genericmessage.h"
#include "clientmanager.h"


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
	SIM::IMContactPtr target = message->targetContact().toStrongRef();
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
	QList<SIM::MessagePtr> result;
	QSqlQuery query;
	SIM::log(SIM::L_DEBUG, "source: %s, target: %s", qPrintable(sourceContactId), qPrintable(targetContactId));
	query.prepare("SELECT * FROM messages WHERE source_id=? AND target_id=?");
	query.bindValue(0, sourceContactId);
	query.bindValue(1, targetContactId);
	if(!query.exec())
	{
		SIM::log(SIM::L_ERROR, "History: unable to retreive messages: %s",
				qPrintable(query.lastError().driverText()));
		return result;
	}
	while(query.next())
	{
		QString clientId = query.value(0).toString();
		SIM::IMContactId sourceId(query.value(1).toString(), 0);
		SIM::IMContactId targetId(query.value(2).toString(), 0);
		QString messageText = query.value(3).toString();
		QDateTime timestamp = QDateTime::fromTime_t(query.value(4).toUInt());

		SIM::IMContactPtr source = SIM::getClientManager()->client(clientId)->getIMContact(sourceId);
		SIM::IMContactPtr target = SIM::getClientManager()->client(clientId)->getIMContact(targetId);

		auto message = new SIM::GenericMessage(source, target, messageText);
		message->setTimestamp(timestamp);
		result.append(SIM::MessagePtr(message));
	}
    return result;
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
