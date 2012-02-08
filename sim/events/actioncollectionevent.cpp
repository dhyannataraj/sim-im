/*
 * actioncollectionevent.cpp
 *
 *  Created on: Feb 3, 2012
 *      Author: todin
 */

#include "actioncollectionevent.h"

namespace SIM
{
ActionCollectionEvent::ActionCollectionEvent(const QString & eventId) : m_id(eventId)
{
}

ActionCollectionEvent::~ActionCollectionEvent()
{
}

QString ActionCollectionEvent::id()
{
	return m_id;
}

bool ActionCollectionEvent::connectTo(QObject *receiver, const char *receiverSlot)
{
	return connect(this, SIGNAL(eventTriggered(SIM::ActionList*)), receiver, receiverSlot);
}

IEventPtr ActionCollectionEvent::create(const QString & eventId)
{
	return IEventPtr(new ActionCollectionEvent(eventId));
}


void ActionCollectionEvent::triggered(const EventDataPtr & data)
{
	ActionCollectionEventDataPtr ourData = data.dynamicCast<ActionCollectionEventData>();
	if(!ourData)
		return;

	emit eventTriggered(ourData->actions());
}



ActionCollectionEventData::~ActionCollectionEventData()
{
}



QString ActionCollectionEventData::eventId() const
{
	return m_id;
}

ActionList* ActionCollectionEventData::actions()
{
	return &m_list;
}

ActionCollectionEventDataPtr ActionCollectionEventData::create(const QString & id, const QString & context)
{
	return ActionCollectionEventDataPtr(new ActionCollectionEventData(id, context));
}

ActionCollectionEventData::ActionCollectionEventData(const QString & id, const QString & context) : m_id(id)
{
	m_list.context = context;
}

}
