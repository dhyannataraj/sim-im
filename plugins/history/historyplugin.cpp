/*
 * historyplugin.cpp
 *
 *  Created on: Dec 3, 2011
 */

#include "historyplugin.h"
#include "messaging/messagepipe.h"

SIM::Plugin* createHistoryPlugin()
{
    return new HistoryPlugin();
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
