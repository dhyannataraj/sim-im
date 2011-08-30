/*
 * widgetcollectionevent.cpp
 *
 *  Created on: Aug 29, 2011
 */

#include "widgetcollectionevent.h"

namespace SIM
{

WidgetCollectionEventData::WidgetCollectionEventData(const QString& id) : m_eventId(id)
{
    m_root = new WidgetHierarchy();
    m_root->nodeName = "Root";
    m_root->widget = nullptr;
}

WidgetCollectionEventData::~WidgetCollectionEventData()
{
    delete m_root;
}

QString WidgetCollectionEventData::eventId() const
{
    return m_eventId;
}

WidgetHierarchy* WidgetCollectionEventData::hierarchyRoot() const
{
    return m_root;
}

WidgetCollectionEventDataPtr WidgetCollectionEventData::create(const QString& id)
{
    return WidgetCollectionEventDataPtr(new WidgetCollectionEventData(id));
}

WidgetCollectionEvent::WidgetCollectionEvent(const QString& eventId) : m_id(eventId)
{
}

WidgetCollectionEvent::~WidgetCollectionEvent()
{
}

QString WidgetCollectionEvent::id()
{
    return m_id;
}

bool WidgetCollectionEvent::connectTo(QObject* receiver, const char* receiverSlot)
{
    return connect(this, SIGNAL(eventTriggered(SIM::WidgetHierarchy*)), receiver, receiverSlot);
}

IEventPtr WidgetCollectionEvent::create(const QString& eventId)
{
    return IEventPtr(new WidgetCollectionEvent(eventId));
}

void WidgetCollectionEvent::triggered(const EventDataPtr& data)
{
    WidgetCollectionEventDataPtr ourData = data.dynamicCast<WidgetCollectionEventData>();
    if(!ourData)
        return;

    emit eventTriggered(ourData->hierarchyRoot());
}

} /* namespace SIM */
