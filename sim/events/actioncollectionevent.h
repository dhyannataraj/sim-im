/*
 * actioncollectionevent.h
 *
 *  Created on: Feb 3, 2012
 *      Author: todin
 */

#ifndef ACTIONCOLLECTIONEVENT_H_
#define ACTIONCOLLECTIONEVENT_H_

#include "ievent.h"
#include <QSharedPointer>
#include <QAction>

namespace SIM
{
struct ActionList
{
	QString context;
	QList<QAction*> actions;
};


class ActionCollectionEventData;
typedef QSharedPointer<ActionCollectionEventData> ActionCollectionEventDataPtr;

class EXPORT ActionCollectionEventData : public SIM::EventData
{
public:
	virtual ~ActionCollectionEventData();

	virtual QString eventId() const;

	ActionList* actions();

	static ActionCollectionEventDataPtr create(const QString& id, const QString& context);

private:
	ActionCollectionEventData(const QString& id, const QString& context);

	QString m_id;
	ActionList m_list;
};

class EXPORT ActionCollectionEvent : public SIM::IEvent
{
	Q_OBJECT
public:
	ActionCollectionEvent(const QString& eventId);
	virtual ~ActionCollectionEvent();

	virtual QString id();
	virtual bool connectTo(QObject* receiver, const char* receiverSlot);

	static IEventPtr create(const QString& eventId);

signals:
	void eventTriggered(SIM::ActionList* actions);

public slots:
	virtual void triggered(const EventDataPtr& data);

private:
	QString m_id;
};
}

#endif /* ACTIONCOLLECTIONEVENT_H_ */
