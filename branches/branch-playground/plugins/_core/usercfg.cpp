/***************************************************************************
                          usercfg.cpp  -  description
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

#include "icons.h"
#include "usercfg.h"
#include "prefcfg.h"
#include "maininfo.h"
#include "core.h"
#include "arcfg.h"
#include "log.h"
#include "contacts/clientdataiterator.h"
#include "contacts/contact.h"
#include "contacts/group.h"
#include "contacts/client.h"

#include <QPixmap>
#include <q3listview.h>
#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <q3widgetstack.h>
#include <q3header.h>
#include <QRegExp>
#include <QIcon>
//Added by qt3to4:
#include <QResizeEvent>

using namespace std;
using namespace SIM;

class ConfigItem : public Q3ListViewItem
{
public:
    ConfigItem(Q3ListViewItem *item, unsigned id, bool bShowUpdate = false);
    ConfigItem(Q3ListView *view, unsigned id, bool bShowUpdate = false);
    ~ConfigItem();
    void show();
    unsigned id() { return m_id; }
    static unsigned curIndex;
protected:
    unsigned m_id;
    bool m_bShowUpdate;
    static unsigned defId;
    void init(unsigned id);
    virtual QWidget *getWidget(UserConfig *dlg);
    QWidget *m_widget;
};

unsigned ConfigItem::defId = 0x10000;
unsigned ConfigItem::curIndex;

ConfigItem::ConfigItem(Q3ListView *view, unsigned id, bool bShowUpdate)
        : Q3ListViewItem(view)
{
    m_bShowUpdate = bShowUpdate;
    init(id);
}

ConfigItem::ConfigItem(Q3ListViewItem *item, unsigned id, bool bShowUpdate)
        : Q3ListViewItem(item)
{
    m_bShowUpdate = bShowUpdate;
    init(id);
}

ConfigItem::~ConfigItem()
{
    if (m_widget)
        delete m_widget;
}

void ConfigItem::init(unsigned id)
{
    m_widget = NULL;
    m_id = id;
    QString key = QString::number(++curIndex);
    while (key.length() < 4)
        key = '0' + key;
    setText(1, key);
}

void ConfigItem::show()
{
    UserConfig *dlg = qobject_cast<UserConfig*>(listView()->topLevelWidget());
	if(!dlg)
		return;
	log(L_DEBUG, "ConfigItem::show()");
    if(m_widget == NULL)
	{
        m_widget = getWidget(dlg);
        if (m_widget == NULL)
            return;
        dlg->wnd->addWidget(m_widget, id() ? id() : defId++);
        dlg->wnd->setMinimumSize(dlg->wnd->sizeHint());
        QObject::connect(dlg, SIGNAL(applyChanges()), m_widget, SLOT(apply()));
    }
    dlg->showUpdate(m_bShowUpdate);
    dlg->wnd->raiseWidget(m_widget);
}

QWidget *ConfigItem::getWidget(UserConfig*)
{
    return NULL;
}

class PrefItem : public ConfigItem
{
public:
    PrefItem(Q3ListViewItem *parent, CommandDef *cmd);
protected:
    virtual QWidget *getWidget(UserConfig *dlg);
    CommandDef *m_cmd;
};

PrefItem::PrefItem(Q3ListViewItem *parent, CommandDef *cmd)
        : ConfigItem(parent, cmd->id)
{
    m_cmd = cmd;
    QString title = i18n(cmd->text);
    title = title.remove('&');
    setText(0, title);
    setPixmap(0, Pict(cmd->icon));
}

QWidget *PrefItem::getWidget(UserConfig *dlg)
{
    return new PrefConfig(dlg->wnd, m_cmd, dlg->m_contact, dlg->m_group);
}

class ClientItem : public ConfigItem
{
public:
    ClientItem(Q3ListView *view, Client *client, void *_data, CommandDef *cmd);
    ClientItem(Q3ListViewItem *item, Client *client, void *_data, CommandDef *cmd);
protected:
    void init(CommandDef *cmd);
    virtual QWidget *getWidget(UserConfig *dlg);
    Client *m_client;
    void   *m_data;
    CommandDef *m_cmd;
};

ClientItem::ClientItem(Q3ListView *view, Client *client, void *data, CommandDef *cmd)
        : ConfigItem(view, 0, true)
{
    m_client = client;
    m_data   = data;
    init(cmd);
}

ClientItem::ClientItem(Q3ListViewItem *item, Client *client, void *data, CommandDef *cmd)
        : ConfigItem(item, 0, true)
{
    m_client = client;
    m_data   = data;
    init(cmd);
}

void ClientItem::init(CommandDef *cmd)
{
    m_cmd = cmd;
    if (!cmd->text_wrk.isEmpty()){
        setText(0, cmd->text_wrk);
        cmd->text_wrk = QString::null;
    }else{
        setText(0, i18n(cmd->text));
    }
    if (!cmd->icon.isEmpty())
        setPixmap(0, Pict(cmd->icon));
}

QWidget *ClientItem::getWidget(UserConfig *dlg)
{
    void *data = m_data;
    Client *client = dlg->m_contact->clientData.activeClient(data, m_client);
    if (client == NULL)
        return NULL;
    return client->infoWindow(dlg, dlg->m_contact, data, m_cmd->id);
}

class MainInfoItem : public ConfigItem
{
public:
    MainInfoItem(Q3ListView *view, unsigned id);
protected:
    virtual QWidget *getWidget(UserConfig *dlg);
};

MainInfoItem::MainInfoItem(Q3ListView *view, unsigned id)
        : ConfigItem(view, id)
{
    setText(0, i18n("User info"));
    setPixmap(0, Pict("info"));
}

QWidget *MainInfoItem::getWidget(UserConfig *dlg)
{
    return new MainInfo(dlg, dlg->m_contact);
}

class ARItem : public ConfigItem
{
public:
    ARItem(Q3ListViewItem *item, const CommandDef *def);
protected:
    virtual QWidget *getWidget(UserConfig *dlg);
    unsigned m_status;
};

ARItem::ARItem(Q3ListViewItem *item, const CommandDef *def)
        : ConfigItem(item, 0)
{
    QString icon;

    m_status = def->id;
    setText(0, i18n(def->text));
    switch (def->id){
    case STATUS_ONLINE: 
        icon="SIM_online";
        break;
    case STATUS_AWAY:
        icon="SIM_away";
        break;
    case STATUS_NA:
        icon="SIM_na";
        break;
    case STATUS_DND:
        icon="SIM_dnd";
        break;
    case STATUS_OCCUPIED:
        icon="SIM_occupied";
        break;
    case STATUS_FFC:
        icon="SIM_ffc";
        break;
    case STATUS_OFFLINE:
        icon="SIM_offline";
        break;
    default:
        icon=def->icon;
        break;
    }
    setPixmap(0, Pict(icon));
}

QWidget *ARItem::getWidget(UserConfig *dlg)
{
    return new ARConfig(dlg, m_status, text(0), dlg->m_contact);
}

static unsigned itemWidth(Q3ListViewItem *item, QFontMetrics &fm)
{
    unsigned w = fm.width(item->text(0)) + 64;
    for (Q3ListViewItem *child = item->firstChild(); child ; child = child->nextSibling()){
        w = QMAX(w, itemWidth(child, fm));
    }
    return w;
}

UserConfig::UserConfig(Contact *contact, Group *group)
  : QDialog(NULL)
{
    setupUi(this);
    setObjectName("userconfig");
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    m_contact  = contact;
    m_group    = group;
    m_nUpdates = 0;

    SET_WNDPROC("configure")
    setWindowIcon(Icon(contact ? "info" : "configure"));
    setButtonsPict(this);
    setTitle();
    btnUpdate->setIcon(Icon("webpress"));
    btnUpdate->hide();

    lstBox->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    lstBox->setSorting(1);
    lstBox->header()->hide();

    fill();

    connect(lstBox, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(itemSelected(Q3ListViewItem*)));
    connect(buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(btnUpdate, SIGNAL(clicked()), this, SLOT(updateInfo()));

    lstBox->setCurrentItem(lstBox->firstChild());
    itemSelected(lstBox->firstChild());
}

UserConfig::~UserConfig()
{
    if (m_contact && (m_contact->getFlags() & CONTACT_TEMPORARY)){
        Contact *contact = m_contact;
        m_contact = NULL;
        delete contact;
    }
}

void UserConfig::setTitle()
{
    QString title;
    if (m_contact){
        if (m_contact->id()){
            title = i18n("User info '%1'") .arg(m_contact->getName());
        }else{
            title = i18n("New contact");
        }
    }else{
        QString groupName;
        if (m_group && m_group->id()){
            groupName = m_group->getName();
        }else{
            groupName = i18n("Not in list");
        }
        title = i18n("Setting for group '%1'") .arg(groupName);
    }
    if (m_nUpdates){
        title += ' ';
        title += i18n("[Update info]");
    }
    setWindowTitle(title);
}

void UserConfig::fill()
{
    ConfigItem::curIndex = 1;
    lstBox->clear();
    Q3ListViewItem *parentItem;
    if (m_contact){
        parentItem = new MainInfoItem(lstBox, CmdInfo);
        ClientDataIterator it(m_contact->clientData);
        void *data;
        while ((data = ++it) != NULL){
            Client *client = m_contact->clientData.activeClient(data, it.client());
            if (client == NULL)
                continue;
            CommandDef *cmds = client->infoWindows(m_contact, data);
            if (cmds){
                parentItem = NULL;
                for (; !cmds->text.isEmpty(); cmds++){
                    if (parentItem){
                        new ClientItem(parentItem, it.client(), data, cmds);
                    }else{
                        parentItem = new ClientItem(lstBox, it.client(), data, cmds);
                        parentItem->setOpen(true);
                    }
                }
            }
        }
    }

    parentItem = NULL;
    ClientUserData* data;
    if (m_contact) {
        data = &m_contact->clientData;
    } else {
        data = &m_group->clientData;
    }
    ClientDataIterator it(*data);
    list<unsigned> st;
    while (++it){
        if ((it.client()->protocol()->description()->flags & PROTOCOL_AR_USER) == 0)
            continue;
        if (parentItem == NULL){
            parentItem = new ConfigItem(lstBox, 0);
            parentItem->setText(0, i18n("Autoreply"));
            parentItem->setOpen(true);
        }
        for (const CommandDef *d = it.client()->protocol()->statusList(); !d->text.isEmpty(); d++){
            if ((d->id == STATUS_ONLINE) || (d->id == STATUS_OFFLINE))
                continue;
            list<unsigned>::iterator it;
            for (it = st.begin(); it != st.end(); ++it)
                if ((*it) == d->id)
                    break;
            if (it != st.end())
                continue;
            st.push_back(d->id);
            new ARItem(parentItem, d);
        }
    }

    parentItem = new ConfigItem(lstBox, 0);
    parentItem->setText(0, i18n("Settings"));
    parentItem->setPixmap(0, Pict("configure"));
    parentItem->setOpen(true);
    CommandDef *cmd;
    CommandsMapIterator itc(CorePlugin::m_plugin->preferences);
    m_defaultPage = 0;
    while((cmd = ++itc) != NULL)
	{
        new PrefItem(parentItem, cmd);
        if (m_defaultPage == 0)
            m_defaultPage = cmd->id;
    }

    QFontMetrics fm(lstBox->font());
    unsigned w = 0;
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        w = QMAX(w, itemWidth(item, fm));
    }
    lstBox->setFixedWidth(w);
    lstBox->setColumnWidth(0, w - 2);
}

bool UserConfig::raisePage(unsigned id)
{
    Q3ListViewItem *item;
    for (item = lstBox->firstChild(); item; item = item->nextSibling()){
        if (raisePage(id, item))
            return true;
    }
    return false;
}

bool UserConfig::raiseDefaultPage()
{
    return raisePage(m_defaultPage);
}

bool UserConfig::raisePage(unsigned id, Q3ListViewItem *item)
{
    unsigned item_id = static_cast<ConfigItem*>(item)->id();
    if (item_id && ((item_id == id) || (id == 0))){
        lstBox->setCurrentItem(item);
        return true;
    }
    for (item = item->firstChild(); item; item = item->nextSibling()){
        if (raisePage(id, item))
            return true;
    }
    return false;
}

void UserConfig::apply()
{
    emit applyChanges();
    if (m_contact)
        getContacts()->addContact(m_contact);
    EventSaveState e;
    e.process();
}

void UserConfig::itemSelected(Q3ListViewItem *item)
{
    static_cast<ConfigItem*>(item)->show();
}

bool UserConfig::processEvent(Event *e)
{
    switch (e->type()){
    case eEventGroup:{
        EventGroup *ev = static_cast<EventGroup*>(e);
        Group *group = ev->group();
        switch(ev->action()) {
        case EventGroup::eDeleted:
            if (group == m_group)
                close();
            return false;
        case EventGroup::eChanged:
            if (group == m_group)
                setTitle();
            return false;
        case EventGroup::eAdded:
            return false;
        }
        break;
    }
    case eEventContact: {
        EventContact *ec = static_cast<EventContact*>(e);
        Contact *contact = ec->contact();
        if (contact != m_contact)
            break;
        switch(ec->action()) {
            case EventContact::eCreated:
                if (m_nUpdates)
                    m_nUpdates--;
                btnUpdate->setEnabled(m_nUpdates == 0);
                setTitle();
            case EventContact::eDeleted:
                close();
                break;
            case EventContact::eChanged:
                if (m_nUpdates)
                    m_nUpdates--;
                btnUpdate->setEnabled(m_nUpdates == 0);
                setTitle();
                break;
            case EventContact::eFetchInfoFailed:
                if (m_nUpdates){
                    if (--m_nUpdates == 0){
                        btnUpdate->setEnabled(true);
                        setTitle();
                    }
                }
                break;
            default:
                break;
        }
        break;
    }
    case eEventCommandRemove: {
        EventCommandRemove *ecr = static_cast<EventCommandRemove*>(e);
        removeCommand(ecr->id());
        return false;
    }
    case eEventLanguageChanged:
    case eEventPluginChanged:
    case eEventClientsChanged:
        fill();
        return false;
    default:
        break;
    }
    return false;
}

void UserConfig::removeCommand(unsigned id)
{
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling())
        removeCommand(id, item);
}

bool UserConfig::removeCommand(unsigned id, Q3ListViewItem *item)
{
    if (item->text(1).toUInt() == id){
        delete item;
        return true;
    }
    for (item = item->firstChild(); item; item = item->nextSibling()){
        if (removeCommand(id, item))
            return true;
    }
    return false;
}

void UserConfig::updateInfo()
{
    if (m_nUpdates || (m_contact == NULL))
        return;
    ClientDataIterator it(m_contact->clientData);
    void *data;
    while ((data = ++it) != NULL){
        Client *client = m_contact->clientData.activeClient(data, it.client());
        if (client == NULL)
            continue;
        m_nUpdates++;
        client->updateInfo(m_contact, data);
    }
    btnUpdate->setEnabled(m_nUpdates == 0);
    setTitle();
}

void UserConfig::showUpdate(bool bShow)
{
    if (bShow){
        btnUpdate->show();
        btnUpdate->setEnabled(m_nUpdates == 0);
    }else{
        btnUpdate->hide();
    }
}

void UserConfig::accept()
{
    apply();
    QDialog::accept();
}

void UserConfig::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
	/* Fixme Todin
    if (isVisible()){
        CorePlugin::m_plugin->data.CfgGeometry[WIDTH].asLong() = width();
        CorePlugin::m_plugin->data.CfgGeometry[HEIGHT].asLong() = height();
    }
	*/
}
