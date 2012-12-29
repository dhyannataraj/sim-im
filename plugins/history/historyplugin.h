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
#include "events/actioncollectionevent.h"
#include "services.h"

class HistoryPlugin : public QObject, public SIM::Plugin, public SIM::MessageProcessor
{
	Q_OBJECT
public:
    HistoryPlugin(const SIM::Services::Ptr& services);
    virtual ~HistoryPlugin();

    virtual QString id() const;
    virtual ProcessResult process(const SIM::MessagePtr& message);

    void setHistoryStorage(const HistoryStoragePtr& storage);

protected slots:
	void contactMenu(SIM::ActionList* list);
	void contactHistoryRequest();
	void save();

private:
    HistoryStoragePtr m_historyStorage;
    SIM::Services::Ptr m_services;
};

#endif /* HISTORYPLUGIN_H_ */
