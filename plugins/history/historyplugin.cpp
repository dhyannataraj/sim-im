/*
 * historyplugin.cpp
 *
 *  Created on: Dec 3, 2011
 */

#include "historyplugin.h"
#include "messaging/messagepipe.h"
#include "sqlitehistorystorage.h"
#include "imagestorage/imagestorage.h"
#include "events/eventhub.h"
#include <QAction>

SIM::Plugin* createHistoryPlugin()
{
    HistoryPlugin* plugin = new HistoryPlugin();
    plugin->setHistoryStorage(HistoryStoragePtr(new SQLiteHistoryStorage()));
    return plugin;
}

static SIM::PluginInfo info =
    {
        NULL,
        NULL,
        VERSION,
        SIM::PLUGIN_DEFAULT,
        createHistoryPlugin
    };

EXPORT_PROC SIM::PluginInfo* GetPluginInfo()
{
    return &info;
}

HistoryPlugin::HistoryPlugin() : SIM::Plugin()
{
    SIM::getMessagePipe()->addMessageProcessor(this);
    SIM::getOutMessagePipe()->addMessageProcessor(this);

    SIM::getEventHub()->getEvent("contact_menu")->connectTo(this, SLOT(contactMenu(SIM::ActionList*)));
}

HistoryPlugin::~HistoryPlugin()
{
    SIM::getMessagePipe()->removeMessageProcessor(id());
    SIM::getOutMessagePipe()->removeMessageProcessor(id());
}

QString HistoryPlugin::id() const
{
    return "history";
}

SIM::MessageProcessor::ProcessResult HistoryPlugin::process(const SIM::MessagePtr& message)
{
    m_historyStorage->addMessage(message);
    return Success;
}

void HistoryPlugin::setHistoryStorage(const HistoryStoragePtr& storage)
{
    m_historyStorage = storage;
}

void HistoryPlugin::save()
{
}

void HistoryPlugin::contactMenu(SIM::ActionList* list)
{
	QAction* history = new QAction(SIM::getImageStorage()->icon("history"), tr("History"), NULL);
	history->setProperty("contact_id", list->context);
	list->actions.append(history);
}
