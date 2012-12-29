
#include "services.h"

namespace SIM {

    class Services::Impl
    {
    public:
        Impl() {}

        ProtocolManager::Ptr protocolManager;
    };

    Services::Services() : m_impl(new Services::Impl())
    {
    }
    
    Services::~Services()
    {
    }

    void Services::setProtocolManager(const ProtocolManager::Ptr& pm)
    {
        m_impl->protocolManager = pm;
    }

    ProtocolManager::Ptr Services::protocolManager() const
    {
        return m_impl->protocolManager;
    }

    Services::Ptr EXPORT makeMockServices()
    {
        auto s = Services::Ptr(new Services());
        s->setProtocolManager(ProtocolManager::Ptr(new ProtocolManager()));
        return s;
    }

}
