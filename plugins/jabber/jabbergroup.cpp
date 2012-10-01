#include "jabbergroup.h"
#include "jabberclient.h"

JabberGroup::JabberGroup(JabberClient* cl) : SIM::IMGroup(), m_client(cl)
{
}

QString JabberGroup::name()
{
    return QString();
}


SIM::Client* JabberGroup::client()
{
    return m_client;
}

QList<SIM::IMContactPtr> JabberGroup::contacts()
{
    return QList<SIM::IMContactPtr>();
}

bool JabberGroup::serialize(QDomElement& element)//Todo, element not used!
{
    return true;
}

bool JabberGroup::deserialize(QDomElement& element)//Todo, element not used!
{
    return true;
}

bool JabberGroup::deserialize(const QString& data)//Todo, data not used!
{
    return true;
}

SIM::PropertyHubPtr JabberGroup::saveState()
{
    return SIM::PropertyHubPtr();
}

bool JabberGroup::loadState(SIM::PropertyHubPtr state)
{
    if (state.isNull())
        return false;
    Q_UNUSED(state)
    return true;
}
