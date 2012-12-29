
#ifndef SIM_CLIENTMANAGER_H
#define SIM_CLIENTMANAGER_H

#include <QString>
#include <QMap>
#include "simapi.h"
#include "clients/client.h"
#include "cfg.h"
#include "contacts/protocolmanager.h"

namespace SIM
{
    class EXPORT ClientManager
    {
    public:
        virtual ~ClientManager() {}

        virtual void addClient(ClientPtr client) = 0;
        virtual void deleteClient(const QString& name) = 0;
        virtual ClientPtr client(const QString& name) = 0;
        virtual ClientPtr client(int index) = 0;
        virtual QList<ClientPtr> allClients() const = 0;
        virtual QStringList clientList() = 0;

        virtual bool load() = 0;
        virtual bool sync() = 0;

        virtual ConfigPtr config() = 0;
    };

    EXPORT ClientManager* getClientManager();
    void EXPORT setClientManager(ClientManager* manager);
    void EXPORT createClientManager(const ProtocolManager::Ptr& protocolManager);
    void EXPORT destroyClientManager();
}

#endif

// vim: set expandtab:

