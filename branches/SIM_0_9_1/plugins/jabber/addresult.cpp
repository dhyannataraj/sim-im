/***************************************************************************
                          addresult.h  -  description
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

#include "addresult.h"
#include "listview.h"
#include "jabberclient.h"
#include "jabber.h"
#include "core.h"

#include <qlayout.h>
#include <qlabel.h>

AddResult::AddResult(JabberClient *client)
{
    m_client = client;
    tblUser->addColumn(i18n("Nick"));
    tblUser->addColumn(i18n("First name"));
    tblUser->addColumn(i18n("Last name"));
    tblUser->addColumn(i18n("Email"));
    tblUser->addColumn(i18n("ID"));
    connect(tblUser, SIGNAL(dragStart()), this, SLOT(dragStart()));
    connect(tblUser, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClicked(QListViewItem*)));
}

AddResult::~AddResult()
{
    emit finished();
}

void AddResult::showSearch(bool bShow)
{
    if (bShow){
        tblUser->show();
    }else{
        tblUser->hide();
    }
    layout()->invalidate();
    resize(width() - 1, height());
    resize(width() + 1, height());
}

void AddResult::showEvent(QShowEvent *e)
{
    AddResultBase::showEvent(e);
    emit search();
}

void AddResult::hideEvent(QHideEvent *e)
{
    AddResultBase::hideEvent(e);
    m_id = "";
    m_host = "";
    m_searchId = "";
}

QString AddResult::foundStatus()
{
    QString res;
    if (m_nFound){
        res += " / ";
        res += i18n("Found 1 contact", "Found %n contacts", m_nFound);
    }
    return res;
}

void *AddResult::processEvent(Event *e)
{
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->menu_id == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->MenuSearchResult){
            Contact *contact;
            if (cmd->id == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->CmdSendMessage){
                contact = createContact(CONTACT_TEMP);
                if (!contact) return NULL;
                Message msg(MessageGeneric);
                msg.setContact(contact->id());
                Event e(EventOpenMessage, &msg);
                e.process();
            }
            if (cmd->id == CmdInfo){
                JabberUserData *data;
                contact = createContact(CONTACT_TEMP, &data);
                if (!contact) return NULL;
                m_client->updateInfo(contact, data);
                Command cmd;
                cmd->id = CmdInfo;
                cmd->menu_id = MenuContact;
                cmd->param   = (void*)contact->id();
                Event e(EventCommandExec, cmd);
                e.process();
            }
            return e->param();
        }
        if (cmd->menu_id == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->MenuGroups){
            Contact *contact = createContact(0);
            if (!contact) return NULL;
            contact->setGroup(cmd->id - 1);
            Event eContact(EventContactChanged, contact);
            eContact.process();
            return e->param();
        }
    }
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if ((cmd->menu_id == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->MenuGroups) &&
                (cmd->id == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->CmdGroups)){
            unsigned n = 1;
            ContactList::GroupIterator it;
            while (++it)
                n++;
            CommandDef *cmds = new CommandDef[n];
            memset(cmds, 0, sizeof(CommandDef) * n);
            n = 0;
            it.reset();
            Group *grp;
            while ((grp = ++it) != NULL){
                cmds[n].id		 = grp->id() + 1;
                cmds[n].text	 = "_";
                QString name = grp->getName();
                if (grp->id() == 0)
                    name = i18n("Not in list");
                cmds[n].text_wrk = strdup(name.utf8());
                n++;
            }
            cmd->param = cmds;
            cmd->flags |= COMMAND_RECURSIVE;
            return e->param();
        }
    }
    if (e->type() == EventSearch){
        JabberSearchData *data = (JabberSearchData*)(e->param());
        if (m_searchId == data->ID){
            m_nFound++;
            lblStatus->setText(i18n("Search") + foundStatus());
            QListViewItem *item = new QListViewItem(tblUser);
            if (data->Nick)
                item->setText(0, QString::fromUtf8(data->Nick));
            if (data->First)
                item->setText(1, QString::fromUtf8(data->First));
            if (data->Last)
                item->setText(2, QString::fromUtf8(data->Last));
            if (data->EMail)
                item->setText(3, QString::fromUtf8(data->EMail));
            if (data->JID)
                item->setText(4, QString::fromUtf8(data->JID));
        }
    }
    if ((e->type() == EventSearchDone) && (m_searchId == (const char*)(e->param()))){
        lblStatus->setText(i18n("Search done") + foundStatus());
        m_searchId = "";
    }
    return NULL;
}

void AddResult::setText(const QString &text)
{
    lblStatus->setText(text);
    tblUser->hide();
}

void AddResult::setSearch(JabberClient *client, const char *search_id)
{
    m_client = client;
    JabberPlugin *plugin = static_cast<JabberPlugin*>(m_client->protocol()->plugin());
    EventSearch = plugin->EventSearch;
    EventSearchDone = plugin->EventSearchDone;
    tblUser->setMenu(static_cast<JabberPlugin*>(m_client->protocol()->plugin())->MenuSearchResult);
    tblUser->clear();
    tblUser->show();
    m_nFound = 0;
    m_searchId = search_id;
    m_id = "";
    m_host = "";
    lblStatus->setText(i18n("Search"));
}

Contact *AddResult::createContact(unsigned tmpFlags, JabberUserData **data)
{
    QListViewItem *item = tblUser->currentItem();
    if (item == NULL)
        return NULL;
    JabberUserData *d;
    if (data == NULL)
        data = &d;
    Contact *contact;
    *data = m_client->findContact(item->text(4).utf8(), item->text(0).utf8(), false, contact);
    if (*data == NULL){
        *data = m_client->findContact(item->text(4).utf8(), item->text(0).utf8(), true, contact);
        contact->setTemporary(tmpFlags);
        Event e(EventContactChanged, contact);
        e.process();
    }
    return contact;
}

void AddResult::dragStart()
{
    Contact *contact = createContact(CONTACT_TEMP | CONTACT_DRAG);
    if (contact == NULL)
        return;
    tblUser->startDrag(new ContactDragObject(tblUser, contact));
}

void AddResult::doubleClicked(QListViewItem *item)
{
    Contact *contact;
    m_client->findContact(item->text(4).utf8(), item->text(0).utf8(), true, contact);
}

#ifndef WIN32
#include "addresult.moc"
#endif

