
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "contact.h"
#include "group.h"
#include "log.h"
#include "contacts/imcontact.h"
#include "clients/client.h"
#include "clients/clientmanager.h"

namespace SIM
{
    Contact::Contact(int id)
        : m_id(id)
    {
        m_userData = UserData::create();
        m_flags.resize(flMaxFlag);
    }

    Contact::~Contact()
    {
    }

    int Contact::groupId() const
    {
        return m_groupId;
    }

    void Contact::setGroupId(int g)
    {
        m_groupId = g;
    }

    bool Contact::flag(Flag fl) const
    {
        return m_flags.at(fl);
    }

    void Contact::setFlag(Flag fl, bool value)
    {
        m_flags.setBit(fl, value);
    }

    QString Contact::name() const
    {
        return m_name;
    }

    void Contact::setName(const QString& s)
    {
        m_name = s;
    }

    time_t Contact::lastActive() const
    {
        return m_lastActive;
    }

    void Contact::setLastActive(time_t la)
    {
        m_lastActive = la;
    }

    QString Contact::notes() const
    {
        return m_notes;
    }

    void Contact::setNotes(const QString& n)
    {
        m_notes = n;
    }

    void Contact::addClientContact(const IMContactPtr& contact)
    {
        contact->setParentContactId(this->id());
        m_imContacts.append(contact);
    }

    IMContactPtr Contact::clientContact(const QString& clientId) const
    {
        foreach(const IMContactPtr& contact, m_imContacts)
        {
            Client* client = contact->client();
            if(!client)
                continue;
            if(client->name() == clientId)
                return contact;
        }
        return IMContactPtr();
    }

    IMContactPtr Contact::clientContact(int num) const
    {
        if((num < 0) || (num >= clientContactCount()))
            return IMContactPtr();
        return m_imContacts.at(num);
    }

    QStringList Contact::clientContactNames() const
    {
        QStringList result;
        foreach(const IMContactPtr& contact, m_imContacts)
        {
            Client* client = contact->client();
            if(!client)
                continue;
            result.append(client->name());
        }
        return result;
    }

    int Contact::clientContactCount() const
    {
        return m_imContacts.size();
    }

    bool Contact::isOnline() const
    {
        if(m_imContacts.size() == 0)
            return false;
        foreach(const IMContactPtr& contact, m_imContacts)
        {
            IMStatusPtr status = contact->status();
            if(!status)
                continue;
            if(status->flag(IMStatus::flOffline))
                return false;
        }
        return true;
    }

    bool Contact::hasUnreadMessages()
    {
        foreach(const IMContactPtr& contact, m_imContacts)
        {
            if(contact->hasUnreadMessages())
                return true;
        }
        return false;
    }

    void Contact::join(const ContactPtr& /*contact*/)
    {

    }

    QString Contact::toolTipText()
    {
        return QString("IMPLEMENT ME!!! Contact::toolTipText()");
    }

    bool Contact::serializeMainInfo(QDomElement& element)
    {
        PropertyHubPtr hub = PropertyHub::create();
        hub->setValue("Group", groupId());
        hub->setValue("Name", name());
        hub->setValue("Ignore", flag(flIgnore));
        hub->setValue("Temporary", flag(flTemporary));
        hub->setValue("LastActive", (qlonglong)lastActive());
        hub->setValue("Notes", notes());
        return hub->serialize(element);
    }

    bool Contact::deserializeMainInfo(const QDomElement& element)
    {
        PropertyHubPtr hub = PropertyHub::create();
        if(!hub->deserialize(element))
            return false;
        setGroupId(hub->value("Group").toInt());
        setName(hub->value("Name").toString());
        setFlag(flIgnore, hub->value("Ignore").toBool());
        setFlag(flTemporary, hub->value("Temporary").toBool());
        setLastActive(hub->value("LastActive").toLongLong());
        setNotes(hub->value("Notes").toString());
        return true;
    }

    bool Contact::serialize(QDomElement& element)
    {
        userdata()->serialize(element);
        QStringList clients = clientContactNames();
        QDomElement maininfo = element.ownerDocument().createElement("main");
        serializeMainInfo(maininfo);
        element.appendChild(maininfo);
        foreach(const QString& clname, clients) {
            IMContactPtr imc = clientContact(clname);
            QDomElement clientElement = element.ownerDocument().createElement("clientdata");
            Client* client = imc->client();
            clientElement.setAttribute("clientname", client->name());
            imc->serialize(clientElement);
            element.appendChild(clientElement);
        }
        return true;
    }

    bool Contact::deserialize(const ClientManager::Ptr& clientManager, const QDomElement& element)
    {
        userdata()->deserialize(element);
        QDomElement main = element.elementsByTagName("main").at(0).toElement();
        if(!main.isNull())
        {
            deserializeMainInfo(main);
        }
        QDomNodeList cldatalist = element.elementsByTagName("clientdata");
        for(int j = 0; j < cldatalist.size(); j++) {
            QDomElement clientElement = cldatalist.at(j).toElement();
            ClientPtr client = clientManager->client(clientElement.attribute("clientname"));
            if(!client)
                continue;
            IMContactPtr imc = clientContact(client->name());
            if(!imc)
                imc = client->createIMContact();
            imc->deserialize(clientElement);
        }
        return true;
    }

    bool Contact::deserialize(const QString& data)
    {
        QStringList list = data.split('\n');
        foreach(const QString& s, list)
        {
            QStringList keyval = s.split('=');
            if(keyval.size() != 2)
                continue;
            if(keyval.at(1).startsWith('\"') && keyval.at(1).endsWith('\"'))
                deserializeLine(keyval.at(0), keyval.at(1).mid(1, keyval.at(1).size() - 2));
            else
                deserializeLine(keyval.at(0), keyval.at(1));
        }
        return true;
    }

    bool Contact::deserializeLine(const QString& key, const QString& value)
    {
        if(key == "Ignore")
            setFlag(flIgnore, value == "true");
        else if(key == "Name")
            setName(value);
        else if(key == "Group")
            setGroupId(value.toUInt());
        return true;
    }

    PropertyHubPtr Contact::saveState()
    {
        PropertyHubPtr contactHub = PropertyHub::create(QString::number(id()));

        contactHub->addPropertyHub(userdata()->saveState());

        PropertyHubPtr mainInfoHub = PropertyHub::create("main");
        contactHub->addPropertyHub(mainInfoHub);
        mainInfoHub->setValue("Group", groupId());
        mainInfoHub->setValue("Name", name());
        mainInfoHub->setValue("Ignore", flag(flIgnore));
        mainInfoHub->setValue("Temporary", flag(flTemporary));
        mainInfoHub->setValue("LastActive", (qlonglong)lastActive());
        mainInfoHub->setValue("Notes", notes());

        PropertyHubPtr clientsHub = PropertyHub::create("clients");
        contactHub->addPropertyHub(clientsHub);
        QStringList clients = clientContactNames();
        foreach(const QString& clname, clients)
        {
            IMContactPtr imc = clientContact(clname);
//            Client* client = imc->client();
//            clientElement.setAttribute("clientname", client->name());
            clientsHub->addPropertyHub(imc->saveState());
        }
        return contactHub;
    }

    bool Contact::loadState(const ClientManager::Ptr& clientManager, PropertyHubPtr state)
    {
        if (state.isNull())
            return false;

        PropertyHubPtr userDataHub = state->propertyHub("userdata");
        if (!userdata()->loadState(userDataHub))
            return false;

        PropertyHubPtr mainInfoHub = state->propertyHub("main");
        if (!mainInfoHub.isNull())
        {
            setGroupId(mainInfoHub->value("Group").toInt());
            setName(mainInfoHub->value("Name").toString());
            setFlag(flIgnore, mainInfoHub->value("Ignore").toBool());
            setFlag(flTemporary, mainInfoHub->value("Temporary").toBool());
            setLastActive(mainInfoHub->value("LastActive").toLongLong());
            setNotes(mainInfoHub->value("Notes").toString());
        }

        PropertyHubPtr clientsHub = state->propertyHub("clients");
        if (clientsHub.isNull())
            return false;
        QStringList clientsList = clientsHub->propertyHubNames();
        foreach (const QString& clientName, clientsList)
        {
            PropertyHubPtr curClientHub = clientsHub->propertyHub(clientName);
            ClientPtr client = clientManager->client(clientName);
            if (!client)
                continue;
            IMContactPtr imc = clientContact(client->name());
            if (!imc)
                imc = client->createIMContact();
            if (!imc->loadState(curClientHub))
                return false;
        }

        return true;
    }
}

// vim: set expandtab:

