
#ifndef SIM_SERVICES_H
#define SIM_SERVICES_H

#include <memory>
#include <QSharedPointer>
#include "contacts/protocolmanager.h"
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

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;        
    };

    Services::Ptr EXPORT makeMockServices();
}

#endif

