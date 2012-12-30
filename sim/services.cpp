
#include "services.h"
#include "tests/mocks/mockclientmanager.h"

namespace SIM {

    class Services::Impl
    {
    public:
        Impl() {}

        ProtocolManager::Ptr protocolManager;
        ClientManager::Ptr clientManager;
        ProfileManager::Ptr profileManager;
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

    void Services::setClientManager(const ClientManager::Ptr& cm)
    {
        m_impl->clientManager = cm;
    }

    ClientManager::Ptr Services::clientManager() const
    {
        return m_impl->clientManager;
    }

    void Services::setProfileManager(const ProfileManager::Ptr& pm)
    {
        m_impl->profileManager = pm;
    }

    ProfileManager::Ptr Services::profileManager() const
    {
        return m_impl->profileManager;
    }
}
