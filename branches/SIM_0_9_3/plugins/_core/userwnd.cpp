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

#include "userwnd.h"
#include "msgedit.h"
#include "msgview.h"
#include "toolbtn.h"
#include "userlist.h"
#include "core.h"
#include "container.h"
#include "history.h"

#include <qtoolbar.h>
#include <qtimer.h>

static DataDef userWndData[] =
    {
        { "EditHeight", DATA_ULONG, 1, 0 },
        { "EditBar", DATA_ULONG, 8, 0 },
        { "MessageType", DATA_ULONG, 1, 0 },
        { NULL, 0, 0, 0 }
    };

UserWnd::UserWnd(unsigned id, const char *cfg, bool bReceived, bool bAdjust)
        : QSplitter(Horizontal, NULL)
{
    load_data(userWndData, &data, cfg);
    m_id = id;
    m_bResize = false;
    m_bClosed = false;
    m_bTyping = false;
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_hSplitter = new QSplitter(Horizontal, this);
    m_splitter = new QSplitter(Vertical, m_hSplitter);
    m_list = NULL;
    m_view = NULL;

    if (cfg == NULL)
        memcpy(data.editBar, CorePlugin::m_plugin->data.editBar, sizeof(data.editBar));

    m_bBarChanged = true;
    if (CorePlugin::m_plugin->getContainerMode())
        bReceived = false;
    m_edit = new MsgEdit(m_splitter, this);
    setFocusProxy(m_edit);
    restoreToolbar(m_edit->m_bar, data.editBar);
    m_edit->m_bar->show();
    m_bBarChanged = false;

    connect(m_edit, SIGNAL(toolBarPositionChanged(QToolBar*)), this, SLOT(toolbarChanged(QToolBar*)));
    connect(CorePlugin::m_plugin, SIGNAL(modeChanged()), this, SLOT(modeChanged()));
    connect(m_edit, SIGNAL(heightChanged(int)), this, SLOT(editHeightChanged(int)));
    modeChanged();

    if (!bAdjust && getMessageType() == 0)
        return;

    if (!m_edit->adjustType()){
        unsigned type = getMessageType();
        Message *msg = new Message(MessageGeneric);
        setMessage(&msg);
        delete msg;
        setMessageType(type);
    }
}

UserWnd::~UserWnd()
{
    emit closed(this);
    free_data(userWndData, &data);
    Contact *contact = getContacts()->contact(id());
    if (contact && contact->getTemporary()){
        m_id = 0;
        delete contact;
    }
}

string UserWnd::getConfig()
{
    return save_data(userWndData, &data);
}

QString UserWnd::getName()
{
    Contact *contact = getContacts()->contact(m_id);
    return contact->getName();
}

QString UserWnd::getLongName()
{
    QString res;
    Contact *contact = getContacts()->contact(m_id);
    res = contact->getName();
    void *data;
    Client *client = m_edit->client(data, false, true, id());
    if (client && data){
        res += " ";
        res += client->contactName(data);
        if (!m_edit->m_resource.isEmpty()){
            res += "/";
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
            res += " ";
            if (m_edit->m_bReceived){
                res += i18n("to %1") .arg(client->name().c_str());
            }else{
                res += i18n("from %1") .arg(client->name().c_str());
            }
        }
    }
    return res;
}

const char *UserWnd::getIcon()
{
    Contact *contact = getContacts()->contact(m_id);
    unsigned long status = STATUS_UNKNOWN;
    unsigned style;
    const char *statusIcon = NULL;
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
    if (CorePlugin::m_plugin->getContainerMode()){
        if (m_view == NULL)
            m_view = new MsgView(m_splitter, m_id);
        m_splitter->moveToFirst(m_view);
        m_splitter->setResizeMode(m_edit, QSplitter::KeepSize);
        m_view->show();
        int editHeight = getEditHeight();
        if (editHeight == 0)
            editHeight = CorePlugin::m_plugin->getEditHeight();
        if (editHeight){
            QValueList<int> s;
            s.append(1);
            s.append(editHeight);
            m_bResize = true;
            m_splitter->setSizes(s);
            m_bResize = false;
        }
    }else{
        if (m_view){
            delete m_view;
            m_view = NULL;
        }
    }
}

void UserWnd::editHeightChanged(int h)
{
    if (!m_bResize && CorePlugin::m_plugin->getContainerMode()){
        setEditHeight(h);
        CorePlugin::m_plugin->setEditHeight(h);
    }
}

void UserWnd::toolbarChanged(QToolBar*)
{
    if (m_bBarChanged)
        return;
    saveToolbar(m_edit->m_bar, data.editBar);
    memcpy(CorePlugin::m_plugin->data.editBar, data.editBar, sizeof(data.editBar));
}

unsigned UserWnd::type()
{
    return m_edit->type();
}

void UserWnd::setMessage(Message **msg)
{
    bool bSetFocus = false;

    Container *container = NULL;
    if (topLevelWidget() && topLevelWidget()->inherits("Container")){
        container = static_cast<Container*>(topLevelWidget());
        if (container->wnd() == this)
            bSetFocus = true;
    }
    if (!m_edit->setMessage(*msg, bSetFocus)){
        delete *msg;
        *msg = new Message(MessageGeneric);
        m_edit->setMessage(*msg, bSetFocus);
    }
    if (container){
        container->setMessageType((*msg)->baseType());
        container->contactChanged(getContacts()->contact(m_id));
    }

    if ((m_view == NULL) || ((*msg)->id() == 0))
        return;
    if (m_view->findMessage(*msg))
        return;
    m_view->addMessage(*msg);
    if (!m_view->hasSelectedText())
        m_view->scrollToBottom();
}

void UserWnd::setStatus(const QString &status)
{
    m_status = status;
    emit statusChanged(this);
}

void UserWnd::showListView(bool bShow)
{
    if (bShow){
        if (m_list == NULL){
            m_list = new UserList(m_hSplitter);
            m_hSplitter->setResizeMode(m_list, QSplitter::Stretch);
            connect(m_list, SIGNAL(selectChanged()), this, SLOT(selectChanged()));
            if (topLevelWidget()->inherits("Container")){
                Container *c = static_cast<Container*>(topLevelWidget());
                list<UserWnd*> wnd = c->windows();
                for (list<UserWnd*>::iterator it = wnd.begin(); it != wnd.end(); ++it)
                    m_list->selected.push_back((*it)->id());
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
    for (list<msg_id>::iterator it = CorePlugin::m_plugin->unread.begin(); it != CorePlugin::m_plugin->unread.end(); ){
        if ((*it).contact != m_id){
            ++it;
            continue;
        }
        Message *msg = History::load((*it).id, (*it).client.c_str(), (*it).contact);
        CorePlugin::m_plugin->unread.erase(it);
        if (msg){
            Event e(EventMessageRead, msg);
            e.process();
            delete msg;
        }
        it = CorePlugin::m_plugin->unread.begin();
    }
}

#ifndef WIN32
#include "userwnd.moc"
#endif

