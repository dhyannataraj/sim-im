
#ifndef SIM_SERVICES_H
#define SIM_SERVICES_H

#include <memory>
#include <QSharedPointer>
#include "contacts/protocolmanager.h"
#include "clients/clientmanager.h"
#include "misc.h"

namespace SIM {

    class Services
    {
    public:
        typedef QSharedPointer<Services> Ptr;

        Services();
        virtual ~Services();

        void setProtocolManager(const ProtocolManager::Ptr& pm);
        ProtocolManager::Ptr protocolManager() const;

        void setClientManager(const ClientManager::Ptr& cm);
        ClientManager::Ptr clientManager() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;        
    };
}

#endif

