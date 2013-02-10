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
#include "historybrowser.h"
#include <QAction>
#include "log.h"
#include "services.h"
#include "plugin/pluginmanager.h"

SIM::Plugin* createHistoryPlugin(const SIM::Services::Ptr& services)
{
    HistoryPlugin* plugin = new HistoryPlugin(services);
    plugin->setHistoryStorage(HistoryStoragePtr(new SQLiteHistoryStorage(services->profileManager(), services->clientManager())));
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

HistoryPlugin::HistoryPlugin(const SIM::Services::Ptr& services) : SIM::Plugin(),
    m_services(services)
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
	SIM::log(SIM::L_DEBUG, "History::contactMenu: contactId: %s", qPrintable(list->context));
	QAction* history = new QAction(SIM::getImageStorage()->icon("history"), tr("History"), NULL);
	history->setProperty("contact_id", list->context.toInt());
	connect(history, SIGNAL(triggered()), this, SLOT(contactHistoryRequest()));
	list->actions.append(history);
}

void HistoryPlugin::contactHistoryRequest()
{
	QObject* s = sender();
	if(!s)
	{
		SIM::log(SIM::L_WARN, "!sender");
		return;
	}
	int contactId = s->property("contact_id").toInt();
	if(contactId == 0)
		return;

	SIM::log(SIM::L_DEBUG, "contactId = %d", contactId);

	HistoryBrowser browser(m_historyStorage, contactId);
	browser.exec();
}
