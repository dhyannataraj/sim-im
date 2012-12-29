
#include <QDir>
#include <QDomElement>
#include "clientmanager.h"
#include "profile/profilemanager.h"
#include "contacts/protocolmanager.h"
#include "standardclientmanager.h"
#include "log.h"

namespace SIM
{
    static ClientManager* g_clientManager = 0;

    ClientManager* getClientManager()
    {
        return g_clientManager;
    }

    void EXPORT setClientManager(ClientManager* manager)
    {
        if(g_clientManager)
            delete g_clientManager;
        g_clientManager = manager;
    }

    void createClientManager(const ProtocolManager::Ptr& protocolManager)
    {
        if(!g_clientManager)
            g_clientManager = new StandardClientManager(protocolManager);
    }

    void destroyClientManager()
    {
        if(g_clientManager)
        {
            delete g_clientManager;
            g_clientManager = 0;
        }
    }
}

// vim: set expandtab:

