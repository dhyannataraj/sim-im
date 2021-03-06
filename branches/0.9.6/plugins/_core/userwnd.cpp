/***************************************************************************
                          userwnd.cpp  -  description
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

#include "log.h"

#include "userwnd.h"
#include "msgedit.h"
#include "msgview.h"
#include "simgui/toolbtn.h"
#include "userlist.h"
#include "core.h"
#include "container.h"
#include "history.h"
#include "contacts/contact.h"
#include "contacts/client.h"

#include <QToolBar>
#include <QApplication>
#include <QTimer>
#include <QCloseEvent>
#include <QByteArray>
#include <QList>
#include <QApplication>

using namespace std;
using namespace SIM;

static DataDef userWndData[] =
    {
        { "EditHeight", DATA_ULONG, 1, 0 },
        { "EditBar", DATA_LONG, 7, 0 },
        { "MessageType", DATA_ULONG, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

//FIXME: Obsolete?
//static void copyData(SIM::Data *dest, const SIM::Data *src, unsigned count)
//{
//    for(unsigned i = 0; i < count; i++)
//        dest[i] = src[i];
//}

UserWnd::UserWnd(unsigned long id, Buffer *cfg, bool bReceived, bool bAdjust)
        : QSplitter(Qt::Horizontal, NULL)
{
    load_data(userWndData, &data, cfg);
    m_id = id;
    m_bResize = false;
    m_bClosed = false;
    m_bTyping = false;
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_splitter = new QSplitter(Qt::Vertical, this);
    m_list = NULL;
    m_view = NULL;
    /* Fixme Todin
    if (cfg == NULL)
        copyData(data.editBar, CorePlugin::instance()->data.EditBar, 7);
    */

    m_bBarChanged = true;
    if (CorePlugin::instance()->getContainerMode())
        bReceived = false;
    addWidget(m_splitter);
    m_edit = new MsgEdit(m_splitter, this);
    setFocusProxy(m_edit);
    restoreToolbar(m_edit->m_bar, data.editBar);
    m_bBarChanged = false;
    m_splitter->addWidget(m_edit);

    connect(m_edit->m_bar, SIGNAL(movableChanged(bool)), this, SLOT(toolbarChanged(bool)));
    connect(CorePlugin::instance(), SIGNAL(modeChanged()), this, SLOT(modeChanged()));
    connect(m_edit, SIGNAL(heightChanged(int)), this, SLOT(editHeightChanged(int)));
    modeChanged();

    if (!bAdjust && getMessageType() == 0)
        return;

    if (!m_edit->adjustType())
    {
        unsigned type = getMessageType();
        Message *msg = new Message(MessageGeneric);
        setMessage(msg);
        delete msg;
        setMessageType(type);
    }
}

UserWnd::~UserWnd()
{
    emit closed(this);
    free_data(userWndData, &data);
    Contact *contact = getContacts()->contact(id());
    if (contact && (contact->getFlags() & CONTACT_TEMPORARY)){
        m_id = 0;
        delete contact;
    }
}

QByteArray UserWnd::getConfig()
{
    return save_data(userWndData, &data);
}

QString UserWnd::getName()
{
    Contact *contact = getContacts()->contact(m_id);
    return contact ? contact->getName() : QString::null;
}

QString UserWnd::getLongName()
{
    QString res;
    if (CorePlugin::instance()->value("ShowOwnerName").toBool() && !getContacts()->owner()->getName().isEmpty())
        res += getContacts()->owner()->getName();
    if (!res.isEmpty())
        res += " - ";
    Contact *contact = getContacts()->contact(m_id);
    if (contact)
        res += contact->getName();
    else
        return QString::null;
    void *data;
    Client *client = m_edit->client(data, false, true, id());
    if (client && data){
        res += ' ';
        res += client->contactName(data);
        if (!m_edit->m_resource.isEmpty())
		{
            res += '/';
            res += m_edit->m_resource;
        }
        bool bFrom = false;
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            Client *pClient = getContacts()->getClient(i);
            if (pClient == client)
                continue;
            Contact *contact;
            clientData *data1 = (clientData*)data;
            if (pClient->isMyData(data1, contact)){
                bFrom = true;
                break;
            }
        }
        if (bFrom){
            res += ' ';
            if (m_edit->m_bReceived){
                res += i18n("to %1") .arg(client->name());
            }else{
                res += i18n("from %1") .arg(client->name());
            }
        }
    }
    return res;
}

QString UserWnd::getIcon()
{
    Contact *contact = getContacts()->contact(m_id);
    if(!contact) {
        log(L_ERROR, "Contact %lu not found!", m_id);
        return QString::null;
    }
    unsigned long status = STATUS_UNKNOWN;
    unsigned style;
    QString statusIcon;
    void *data;
    Client *client = m_edit->client(data, false, true, id());
    if (client){
        client->contactInfo(data, status, style, statusIcon);
    }else{
        contact->contactInfo(style, statusIcon);
    }
    return statusIcon;
}

void UserWnd::modeChanged()
{
    if (CorePlugin::instance()->getContainerMode())
    {
        if (m_view == NULL)
            m_view = new MsgView(m_splitter, m_id);
        m_splitter->insertWidget(0, m_view);
        m_splitter->setStretchFactor(0, 0);
        m_view->show();
        int editHeight = getEditHeight();
        if (editHeight == 0)
            editHeight = CorePlugin::instance()->value("EditHeight").toInt(); //getEditHeight();
        if (editHeight){
            QList<int> s;
            s.append(1);
            s.append(editHeight);
            m_bResize = true;
            m_splitter->setSizes(s);
            m_bResize = false;
        }
    }
    else
    {
        if (m_view){
            delete m_view;
            m_view = NULL;
        }
    }
}

void UserWnd::editHeightChanged(int h)
{
    if (!m_bResize && CorePlugin::instance()->getContainerMode()){
        setEditHeight(h);
        CorePlugin::instance()->setValue("EditHeight", h);
    }
}

void UserWnd::toolbarChanged(bool)
{
    if (m_bBarChanged)
        return;
    //saveToolbar(m_edit->m_bar, data.editBar);
    //copyData(CorePlugin::instance()->data.EditBar, data.editBar, 7);
}

unsigned UserWnd::type()
{
    return m_edit->type();
}

void UserWnd::setMessage(Message *msg)
{
    bool bSetFocus = false;

    Container *container = NULL;
    if (topLevelWidget() && topLevelWidget()->inherits("Container"))
	{
        container = static_cast<Container*>(topLevelWidget());
        if (container->wnd() == this)
            bSetFocus = true;
    }
    if (!m_edit->setMessage(msg, bSetFocus))
	{
        // if this does not work as expected, we have to go back
        // to EventOpenMessage with Message** :(
        *msg = Message(MessageGeneric);
        m_edit->setMessage(msg, bSetFocus);
    }
    if (container)
	{
        container->setMessageType(msg->baseType());
        container->contactChanged(getContacts()->contact(m_id));
    }

    if ((m_view == NULL) || (msg->id() == 0))
        return;
    if (m_view->findMessage(msg))
        return;
    m_view->addMessage(msg);
}

void UserWnd::setStatus(const QString &status)
{
    m_status = status;
    emit statusChanged(this);
}

void UserWnd::showListView(bool bShow)
{
    if(bShow)
    {
        if (m_list == NULL)
        {
            m_list = new UserList(this);
            setStretchFactor(indexOf(m_list), 1);
            connect(m_list, SIGNAL(selectChanged()), this, SLOT(selectChanged()));

            if(topLevelWidget()->inherits("Container"))
            {
                Container *c = qobject_cast<Container*>(topLevelWidget());
                list<UserWnd*> wnd = c->windows();
                for (list<UserWnd*>::iterator it = wnd.begin(); it != wnd.end(); ++it)
                    m_list->select((*it)->id());
            }
        }
        m_list->show();
        emit multiplyChanged();
        return;
    }
    if (m_list == NULL)
        return;
    delete m_list;
    m_list = NULL;
    emit multiplyChanged();
}

void UserWnd::selectChanged()
{
    emit multiplyChanged();
}

void UserWnd::closeEvent(QCloseEvent *e)
{
    QSplitter::closeEvent(e);
    m_bClosed = true;
    QTimer::singleShot(0, topLevelWidget(), SLOT(wndClosed()));
}

void UserWnd::markAsRead()
{
    if (m_view == NULL)
        return;
    for (list<msg_id>::iterator it = CorePlugin::instance()->unread.begin(); it != CorePlugin::instance()->unread.end(); )
    {
        if (it->contact != m_id) 
        {
            ++it;
            continue;
        }
        Message *msg = History::load(it->id, it->client, it->contact);
        CorePlugin::instance()->unread.erase(it);
        if (msg){
            EventMessageRead(msg).process();
            delete msg;
        }
        it = CorePlugin::instance()->unread.begin();
    }
}

