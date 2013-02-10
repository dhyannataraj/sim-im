
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
        typedef QSharedPointer<ProtocolManager> Ptr;

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
}

#endif

// vim: set expandtab:

