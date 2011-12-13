
#ifndef SIM_USERDATA_H
#define SIM_USERDATA_H

#include "simapi.h"
#include "cfg.h"
#include "buffer.h"
#include <QByteArray>
#include <QSharedPointer>
#include "propertyhub.h"

namespace SIM
{
    class EXPORT UserData;
    typedef QSharedPointer<UserData> UserDataPtr;
    class EXPORT UserData
    {
    public:
        virtual ~UserData();
        static UserDataPtr create();

        PropertyHubPtr getUserData(const QString& id); 
        PropertyHubPtr createUserData(const QString& id);
        QStringList userDataIds() const;
        void destroyUserData(const QString& id);

        PropertyHubPtr root();

        bool serialize(QDomElement element);
        bool deserialize(QDomElement element);

        bool loadState(PropertyHubPtr state);
        PropertyHubPtr saveState();
    protected:
        UserData();

    private:
        PropertyHubPtr m_root;
        QString m_namespace;
        COPY_RESTRICTED(UserData)
    };

}

#endif

// vim: set expandtab:

