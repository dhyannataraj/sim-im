/***************************************************************************
                          forward.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "forward.h"
#include "forwardcfg.h"
#include "core.h"
#include "moc_core.cpp"

#include "contacts/clientdataiterator.h"
#include "contacts/contact.h"
#include "contacts/group.h"
#include "contacts/client.h"

using namespace SIM;

Plugin *createForwardPlugin(unsigned base, bool, Buffer*)
{
    Plugin *plugin = new ForwardPlugin(base);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Forward"),
        I18N_NOOP("Plugin provides messages forwarding on cellular"),
        VERSION,
        createForwardPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

static DataDef forwardUserData[] =
    {
        { "Phone", DATA_UTF, 1, 0 },
        { "Send1st", DATA_BOOL, 1, 0 },
        { "Translit", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static ForwardPlugin *forwardPlugin = NULL;

static QWidget *getForwardSetup(QWidget *parent, void *data)
{
    return new ForwardConfig(parent, data, forwardPlugin);
}

ForwardPlugin::ForwardPlugin(unsigned base)
        : Plugin(base), EventReceiver(DefaultPriority - 1)
{
    forwardPlugin = this;
    user_data_id = getContacts()->registerUserData(info.title, forwardUserData);
    Command cmd;
    cmd->id		  = user_data_id;
    cmd->text	  = I18N_NOOP("&Forward");
    cmd->icon	  = "cell";
    cmd->param	 = (void*)getForwardSetup;
    EventAddPreferences(cmd).process();
}

ForwardPlugin::~ForwardPlugin()
{
    EventRemovePreferences(user_data_id).process();
    getContacts()->unregisterUserData(user_data_id);
}

bool ForwardPlugin::processEvent(Event *e)
{
    if (e->type() == eEventMessageReceived){
        EventMessage *em = static_cast<EventMessage*>(e);
        Message *msg = em->msg();
        if (msg->type() == MessageStatus)
            return false;
        QString text = msg->getPlainText();
        if (text.isEmpty())
            return false;
        if (msg->type() == MessageSMS){
            SMSMessage *sms = static_cast<SMSMessage*>(msg);
            QString phone = sms->getPhone();
            bool bMyPhone;
            ForwardUserData *data = (ForwardUserData*)(getContacts()->getUserData(user_data_id));
            bMyPhone = ContactList::cmpPhone(phone, data->Phone.str());
            if (!bMyPhone){
                Group *grp;
                ContactList::GroupIterator it;
                while ((grp = ++it) != NULL){
                    data = (ForwardUserData*)(grp->getUserData(user_data_id, false));
                    if (data && !data->Phone.str().isEmpty()){
                        bMyPhone = ContactList::cmpPhone(phone, data->Phone.str());
                        break;
                    }
                }
            }
            if (!bMyPhone){
                Contact *contact;
                ContactList::ContactIterator it;
                while ((contact = ++it) != NULL){
                    data = (ForwardUserData*)(contact->getUserData(user_data_id, false));
                    if (data && !data->Phone.str().isEmpty()){
                        bMyPhone = ContactList::cmpPhone(phone, data->Phone.str());
                        break;
                    }
                }
            }
            if (bMyPhone){
                int n = text.indexOf(": ");
                if (n > 0){
                    QString name = text.left(n);
                    QString msg_text = text.mid(n + 2);
                    Contact *contact;
                    ContactList::ContactIterator it;
                    while ((contact = ++it) != NULL){
                        if (contact->getName() == name){
                            Message *msg = new Message(MessageGeneric);
                            msg->setContact(contact->id());
                            msg->setText(msg_text);
                            void *data;
                            ClientDataIterator it(contact->clientData);
                            while ((data = ++it) != NULL){
                                if (it.client()->send(msg, data))
                                    break;
                            }
                            if (data == NULL)
                                delete msg;
                            return true;
                        }
                    }
                }
            }
        }
        Contact *contact = getContacts()->contact(msg->contact());
        if (contact == NULL)
            return false;
        ForwardUserData *data = (ForwardUserData*)(contact->getUserData(user_data_id));
        if ((data == NULL) || (data->Phone.str().isEmpty()))
            return false;
        unsigned status = CorePlugin::instance()->getManualStatus();
        if ((status == STATUS_AWAY) || (status == STATUS_NA)){
            text = contact->getName() + ": " + text;
            unsigned flags = MESSAGE_NOHISTORY;
            if (data->Send1st.toBool())
                flags |= MESSAGE_1ST_PART;
            if (data->Translit.toBool())
                flags |= MESSAGE_TRANSLIT;
            SMSMessage *m = new SMSMessage;
            m->setPhone(data->Phone.str());
            m->setText(text);
            m->setFlags(flags);
            unsigned i;
            for (i = 0; i < getContacts()->nClients(); i++){
                Client *client = getContacts()->getClient(i);
                if (client->send(m, NULL))
                    break;
            }
            if (i >= getContacts()->nClients())
                delete m;
        }
    }
    return false;
}

QWidget *ForwardPlugin::createConfigWindow(QWidget *parent)
{
    return new ForwardConfig(parent, getContacts()->getUserData(user_data_id), this);
}
