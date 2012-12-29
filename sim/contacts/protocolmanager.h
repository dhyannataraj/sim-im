
#ifndef SIM_PROTOCOLMANAGER_H
#define SIM_PROTOCOLMANAGER_H

#include "protocol.h"
#include "simapi.h"
#include <list>
#include <QSharedPointer>

namespace SIM
{
    class EXPORT ProtocolManager
    {
    public:
        ProtocolManager();
        virtual ~ProtocolManager();

        void addProtocol(ProtocolPtr protocol);
        ProtocolPtr protocol(int index);
        ProtocolPtr protocol(const QString& name);
        int protocolCount();
        void removeProtocol(ProtocolPtr protocol);

    private:
        QList<ProtocolPtr> m_protocols;
    };

    EXPORT ProtocolManager* getProtocolManager();
    void EXPORT createProtocolManager();
    void EXPORT destroyProtocolManager();
}

#endif

// vim: set expandtab:

