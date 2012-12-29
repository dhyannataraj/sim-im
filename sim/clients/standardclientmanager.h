#ifndef STANDARDCLIENTMANAGER_H
#define STANDARDCLIENTMANAGER_H

#include "clientmanager.h"
#include "contacts/protocolmanager.h"

namespace SIM {

class StandardClientManager : public ClientManager
{
public:
    StandardClientManager(const ProtocolManager::Ptr& protocolManager);
    virtual ~StandardClientManager();

    virtual void addClient(ClientPtr client);
	virtual void deleteClient(const QString& name);
	ClientPtr getClientByProfileName(const QString& name);
    virtual ClientPtr client(const QString& name);
    virtual ClientPtr client(int index);
    virtual QStringList clientList();
    virtual QList<ClientPtr> allClients() const;

    virtual bool load();
    virtual bool sync();

    virtual ConfigPtr config();

protected:
    bool load_old();
    bool load_new();

private:
    ClientPtr createClient(const QString& name);
    typedef QList<ClientPtr> ClientMap;
    ClientMap m_clients;
    QStringList m_sortedClientNamesList;
    ConfigPtr m_config;
    QString m_loadedProfile;
    ProtocolManager::Ptr m_protocolManager;
};

} // namespace SIM

#endif // STANDARDCLIENTMANAGER_H
