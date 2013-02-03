
#ifndef SIM_SERVICES_H
#define SIM_SERVICES_H

#include <memory>
#include <QSharedPointer>
#include "contacts/protocolmanager.h"
#include "clients/clientmanager.h"
#include "profile/profilemanager.h"
#include "misc.h"

namespace SIM {

    class EXPORT Services
    {
    public:
        typedef QSharedPointer<Services> Ptr;

        Services();
        virtual ~Services();

        void setProtocolManager(const ProtocolManager::Ptr& pm);
        ProtocolManager::Ptr protocolManager() const;

        void setClientManager(const ClientManager::Ptr& cm);
        ClientManager::Ptr clientManager() const;

        void setProfileManager(const ProfileManager::Ptr& pm);
        ProfileManager::Ptr profileManager() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;        
    };
}

#endif

