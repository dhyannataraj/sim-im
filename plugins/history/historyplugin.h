/*
 * historyplugin.h
 *
 *  Created on: Dec 3, 2011
 */

#ifndef HISTORYPLUGIN_H_
#define HISTORYPLUGIN_H_

#include "plugins.h"
#include "messaging/message.h"
#include "messaging/messageprocessor.h"
#include "historystorage.h"

class HistoryPlugin : public SIM::Plugin, public SIM::MessageProcessor
{
public:
    HistoryPlugin();
    virtual ~HistoryPlugin();

    virtual QString id() const;
    virtual ProcessResult process(const SIM::MessagePtr& message);

    void setHistoryStorage(const HistoryStoragePtr& storage);

private:
    HistoryStoragePtr m_historyStorage;
};

#endif /* HISTORYPLUGIN_H_ */
