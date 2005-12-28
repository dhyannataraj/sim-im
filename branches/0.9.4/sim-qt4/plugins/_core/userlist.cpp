/***************************************************************************
                          userlist.cpp  -  description
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

#include "userlist.h"
#include "core.h"

#include <Q3Header>
#include <QTimer>
#include <QBitmap>
#include <QStyle>
#include <QStyleOption>
#include <Q3Button>
#include <QPainter>

#include <QPixmap>
#include <QMouseEvent>

UserViewItemBase::UserViewItemBase(UserListBase *parent)
        : Q3ListViewItem(parent)
{
}

UserViewItemBase::UserViewItemBase(UserViewItemBase *parent)
        : Q3ListViewItem(parent)
{
}

void UserViewItemBase::setup()
{
    UserListBase *view = static_cast<UserListBase*>(listView());
    setHeight(view->heightItem(this));
}

void UserViewItemBase::paintFocus(QPainter*, const QColorGroup&, const QRect & )
{
}

void UserViewItemBase::paintCell(QPainter *p, const QColorGroup &cg, int, int width, int)
{
    UserListBase *view = static_cast<UserListBase*>(listView());
    width = view->width() - 4;
    QScrollBar *vBar = view->verticalScrollBar();
    if (vBar->isVisible())
        width -= vBar->width();
    if (width < 1)
        width = 1;
    QPixmap bg(width, height());
    QPainter pp(&bg);
    int margin = 0;
    pp.fillRect(QRect(0, 0, width, height()), cg.base());
    PaintView pv;
    pv.p        = &pp;
    pv.pos      = view->viewport()->mapToParent(view->itemRect(this).topLeft());
    pv.size		= QSize(width, height());
    pv.win      = view;
    pv.isStatic = false;
    pv.height   = height();
    pv.margin   = 0;
    pv.isGroup  = (type() == GRP_ITEM);
    if (CorePlugin::m_plugin->getUseSysColors()){
        pp.setPen(cg.text());
    }else{
        pp.setPen(QColor(CorePlugin::m_plugin->getColorOnline()));
    }
    Event e(EventPaintView, &pv);
    e.process();
    view->setStaticBackground(pv.isStatic);
    margin = pv.margin;
    if (isSelected() && view->hasFocus() && CorePlugin::m_plugin->getUseDblClick()){
        pp.fillRect(QRect(0, 0, width, height()), cg.highlight());
        pp.setPen(cg.highlightedText());
    }
    view->drawItem(this, &pp, cg, width, margin);
    pp.end();
    if (view->m_pressedItem == this){
        p->drawPixmap(QPoint(1, 1), bg);
        if (CorePlugin::m_plugin->getUseSysColors()){
            p->setPen(cg.text());
        }else{
            p->setPen(QColor(CorePlugin::m_plugin->getColorOnline()));
        }
//        p->moveTo(0, 0);
        p->drawLine(0, 0, width - 1, 0);
        p->drawLine(0, 0, width - 1, height() - 1);
        p->drawLine(0, 0, 0, height() - 1);
//        p->drawLine(0, 0);
        p->setPen(cg.shadow());
//        p->moveTo(width - 2, 1);
        p->drawLine(width - 2, 1, 1, 1);
        p->drawLine(width - 2, 1, 1, height() - 2);
    }else{
        p->drawPixmap(QPoint(0, 0), bg);
    }
}

int UserViewItemBase::drawText(QPainter *p, int x, int width, const QString &text)
{
    QRect br;
    p->drawText(x, 0, width, height(), Qt::AlignLeft | Qt::AlignVCenter, text, -1, &br);
    return br.right() + 5;
}

void UserViewItemBase::drawSeparator(QPainter *p, int x, int width)
{
    const QStyleOption *option;
//    if (x < width - 6){
//        listView()->style()->drawPrimitive(QStyle::PE_Q3Separator, option, p, NULL);
//    }
}

DivItem::DivItem(UserListBase *view, unsigned type)
        : UserViewItemBase(view)
{
    m_type = type;
    setText(0, QString::number(m_type));
    setExpandable(true);
    setSelectable(false);
}

GroupItem::GroupItem(UserListBase *view, Group *grp, bool bOffline)
        : UserViewItemBase(view)
{
    m_id = grp->id();
    m_bOffline = bOffline;
    init(grp);
}

GroupItem::GroupItem(UserViewItemBase *view, Group *grp, bool bOffline)
        : UserViewItemBase(view)
{
    m_id = grp->id();
    m_bOffline = bOffline;
    init(grp);
}

void GroupItem::init(Group *grp)
{
    m_unread = 0;
    m_nContacts = 0;
    m_nContactsOnline = 0;
    setExpandable(true);
    setSelectable(true);
    ListUserData *data = (ListUserData*)(grp->getUserData(CorePlugin::m_plugin->list_data_id, false));
    if (data == NULL){
        setOpen(true);
    }else{
        if (m_bOffline){
            setOpen(data->OfflineOpen.bValue);
        }else{
            setOpen(data->OnlineOpen.bValue);
        }
    }
    update(grp, true);
}

void GroupItem::update(Group *grp, bool bInit)
{
    QString s;
    s = "A";
    if (grp->id()){
        s = QString::number(getContacts()->groupIndex(grp->id()));
        while (s.length() < 12){
            s = QString("0") + s;
        }
    }
    if (s == text(0))
        return;
    setText(0, s);
    if (bInit)
        return;
    Q3ListViewItem *p = parent();
    if (p){
        p->sort();
        return;
    }
    listView()->sort();
}

void GroupItem::setOpen(bool bOpen)
{
    UserViewItemBase::setOpen(bOpen);
    Group *grp = getContacts()->group(m_id);
    if (grp){
        ListUserData *data = (ListUserData*)(grp->getUserData(CorePlugin::m_plugin->list_data_id, !bOpen));
        if (data){
            if (m_bOffline){
                data->OfflineOpen.bValue = bOpen;
            }else{
                data->OnlineOpen.bValue  = bOpen;
            }
        }
    }
}

ContactItem::ContactItem(UserViewItemBase *view, Contact *contact, unsigned status, unsigned style, const char *icons, unsigned unread)
        : UserViewItemBase(view)
{
    m_id = contact->id();
    init(contact, status, style, icons, unread);
#if COMPAT_QT_VERSION >= 0x030000    
    setDragEnabled(true);
#endif
}

void ContactItem::init(Contact *contact, unsigned status, unsigned style, const char *icons, unsigned unread)
{
    m_bOnline    = false;
    m_bBlink	 = false;
    setSelectable(true);
    update(contact, status, style, icons, unread);
}

bool ContactItem::update(Contact *contact, unsigned status, unsigned style, const char *icons, unsigned unread)
{
    m_unread = unread;
    m_style  = style;
    m_status = status;
    QString icons_str;
    if (icons)
        icons_str = icons;
    QString name = contact->getName();
    QString active;
    active.sprintf("%08lX", 0xFFFFFFFF - contact->getLastActive());
    setText(CONTACT_TEXT, name);
    setText(CONTACT_ICONS, icons_str);
    setText(CONTACT_ACTIVE, active);
    setText(CONTACT_STATUS, QString::number(9 - status));
    setup();
    return true;
}

QString ContactItem::key(int column, bool ascending ) const
{
    if (column == 0){
        unsigned mode = CorePlugin::m_plugin->getSortMode();
        QString res;
        for (;;){
            int n = 0;
            switch (mode & 0xFF){
            case SORT_STATUS:
                n = CONTACT_STATUS;
                break;
            case SORT_ACTIVE:
                n = CONTACT_ACTIVE;
                break;
            case SORT_NAME:
                n = CONTACT_TEXT;
                break;
            }
            if (n == 0)
                break;
            res += text(n).lower();
            mode = mode >> 8;
        }
        return res;
    }
    return UserViewItemBase::key(column, ascending);
}

UserListBase::UserListBase(QWidget *parent)
        : ListView(parent)
{
    m_bInit  = false;
    m_bDirty = false;
    m_groupMode = 1;
    m_bShowOnline = false;
    m_bShowEmpty  = false;

    header()->hide();
    addColumn("", -1);
    setHScrollBarMode(AlwaysOff);
    setVScrollBarMode(Auto);
    setSorting(0);

    updTimer = new QTimer(this);
    connect(updTimer, SIGNAL(timeout()), this, SLOT(drawUpdates()));

    setExpandingColumn(0);
}

UserListBase::~UserListBase()
{
}

void UserListBase::drawUpdates()
{
    m_bDirty = false;
    updTimer->stop();
    Q3ListViewItem *item;
    int x = contentsX();
    int y = contentsY();
    viewport()->setUpdatesEnabled(false);
    bool bChanged = false;
    list<unsigned long>::iterator it;
    for (it = updGroups.begin(); it != updGroups.end(); ++it){
        Group *group = getContacts()->group(*it);
        if (group == NULL)
            continue;
        switch (m_groupMode){
        case 1:
            item = findGroupItem(group->id());
            if (item){
                if (!m_bShowEmpty && (item->firstChild() == NULL)){
                    delete item;
                    bChanged = true;
                }else{
                    static_cast<GroupItem*>(item)->update(group);
                    addUpdatedItem(item);
                }
            }else{
                if (m_bShowEmpty){
                    new GroupItem(this, group, true);
                    bChanged = true;
                }
            }
            break;
        case 2:
            for (item = firstChild(); item; item = item->nextSibling()){
                UserViewItemBase *i = static_cast<UserViewItemBase*>(item);
                if (i->type() != DIV_ITEM) continue;
                DivItem *divItem = static_cast<DivItem*>(i);
                GroupItem *grpItem = findGroupItem(group->id(), divItem);
                if (grpItem){
                    if (!m_bShowEmpty && (item->firstChild() == NULL)){
                        delete grpItem;
                        bChanged = true;
                    }else{
                        grpItem->update(group);
                        addUpdatedItem(grpItem);
                    }
                }else{
                    if (m_bShowEmpty){
                        new GroupItem(divItem, group, divItem->state() == DIV_OFFLINE);
                        bChanged = true;
                    }
                }
            }
            break;
        }
    }
    updGroups.clear();
    DivItem *itemOnline  = NULL;
    DivItem *itemOffline = NULL;
    if (updContacts.size()){
        if (m_groupMode != 1){
            for (item = firstChild(); item != NULL; item = item->nextSibling()){
                UserViewItemBase *i = static_cast<UserViewItemBase*>(item);
                if (i->type() != DIV_ITEM) continue;
                DivItem *divItem = static_cast<DivItem*>(i);
                if (divItem->state() == DIV_ONLINE)
                    itemOnline = divItem;
                if (divItem->state() == DIV_OFFLINE)
                    itemOffline = divItem;
            }
        }
    }
    for (it = updContacts.begin(); it != updContacts.end(); ++it){
        Contact *contact = getContacts()->contact(*it);
        if (contact == NULL)
            continue;
        ContactItem *contactItem;
        GroupItem *grpItem;
        unsigned style;
        string icons;
        unsigned status = getUserStatus(contact, style, icons);
        unsigned unread = getUnread(contact->id());
        bool bShow = false;
        ListUserData *data = (ListUserData*)(contact->getUserData(CorePlugin::m_plugin->list_data_id));
        if (data && data->ShowAlways.bValue)
            bShow = true;
        switch (m_groupMode){
        case 0:
            if (status <= STATUS_OFFLINE){
                if (itemOnline){
                    contactItem = findContactItem(contact->id(), itemOnline);
                    if (contactItem){
                        deleteItem(contactItem);
                        bChanged = true;
                        if (itemOnline->firstChild() == NULL){
                            deleteItem(itemOnline);
                            itemOnline = NULL;
                        }
                    }
                }
                if ((unread == 0) && !bShow && m_bShowOnline){
                    if (itemOffline){
                        contactItem = findContactItem(contact->id(), itemOffline);
                        if (contactItem){
                            deleteItem(contactItem);
                            bChanged = true;
                            if (itemOffline->firstChild() == NULL){
                                deleteItem(itemOffline);
                                itemOffline = NULL;
                            }
                        }
                    }
                    break;
                }
                if (itemOffline == NULL){
                    itemOffline = new DivItem(this, DIV_OFFLINE);
                    setOpen(itemOffline, true);
                    bChanged = true;
                }
                contactItem = findContactItem(contact->id(), itemOffline);
                if (contactItem){
                    if (contactItem->update(contact, status, style, icons.c_str(), unread))
                        addSortItem(itemOffline);
                    addUpdatedItem(contactItem);
                }else{
                    contactItem = new ContactItem(itemOffline, contact, status, style, icons.c_str(), unread);
                    bChanged = true;
                }
            }else{
                if (itemOffline){
                    contactItem = findContactItem(contact->id(), itemOffline);
                    if (contactItem){
                        deleteItem(contactItem);
                        bChanged = true;
                        if (itemOffline->firstChild() == NULL){
                            deleteItem(itemOffline);
                            itemOffline = NULL;
                        }
                    }
                }
                if (itemOnline == NULL){
                    itemOnline = new DivItem(this, DIV_ONLINE);
                    setOpen(itemOnline, true);
                    bChanged = true;
                }
                contactItem = findContactItem(contact->id(), itemOnline);
                if (contactItem){
                    if (contactItem->update(contact, status, style, icons.c_str(), unread))
                        addSortItem(itemOnline);
                    addUpdatedItem(contactItem);
                }else{
                    contactItem = new ContactItem(itemOnline, contact, status, style, icons.c_str(), unread);
                    bChanged = true;
                }
            }
            break;
        case 1:
            contactItem = findContactItem(contact->id());
            grpItem = NULL;
            if (contactItem){
                grpItem = static_cast<GroupItem*>(contactItem->parent());
                if (((status <= STATUS_OFFLINE) && (unread == 0) && !bShow && m_bShowOnline) ||
                        (contact->getGroup() != grpItem->id())){
                    grpItem->m_nContacts--;
                    if (contactItem->m_bOnline)
                        grpItem->m_nContactsOnline--;
                    addGroupForUpdate(grpItem->id());
                    deleteItem(contactItem);
                    bChanged = true;
                    if (!m_bShowEmpty && (grpItem->firstChild() == NULL))
                        delete grpItem;
                    contactItem = NULL;
                    grpItem = NULL;
                }
            }
            if ((status > STATUS_OFFLINE) || unread || bShow || !m_bShowOnline){
                if (grpItem == NULL){
                    grpItem = findGroupItem(contact->getGroup());
                    if (grpItem == NULL){
                        Group *grp = getContacts()->group(contact->getGroup());
                        if (grp){
                            grpItem = new GroupItem(this, grp, true);
                            bChanged = true;
                        }
                    }
                }
                if (grpItem){
                    if (contactItem){
                        if (contactItem->update(contact, status, style, icons.c_str(), unread))
                            addSortItem(grpItem);
                        addUpdatedItem(contactItem);
                        if (!m_bShowOnline &&
                                (contactItem->m_bOnline != (status > STATUS_OFFLINE))){
                            if (status <= STATUS_OFFLINE){
                                grpItem->m_nContactsOnline--;
                                contactItem->m_bOnline = false;
                            }else{
                                grpItem->m_nContactsOnline++;
                                contactItem->m_bOnline = true;
                            }
                            addGroupForUpdate(grpItem->id());
                        }
                    }else{
                        bChanged = true;
                        contactItem = new ContactItem(grpItem, contact, status, style, icons.c_str(), unread);
                        grpItem->m_nContacts++;
                        if (!m_bShowOnline && (status > STATUS_OFFLINE)){
                            grpItem->m_nContactsOnline++;
                            contactItem->m_bOnline = true;
                        }
                        addGroupForUpdate(grpItem->id());
                    }
                }
            }
            break;
        case 2:
            contactItem = findContactItem(contact->id(), itemOnline);
            grpItem = NULL;
            if (contactItem){
                grpItem = static_cast<GroupItem*>(contactItem->parent());
                if ((status <= STATUS_OFFLINE) || (grpItem->id() != contact->getGroup())){
                    grpItem->m_nContacts--;
                    addGroupForUpdate(grpItem->id());
                    deleteItem(contactItem);
                    bChanged = true;
                    if (!m_bShowEmpty && (grpItem->firstChild() == NULL))
                        delete grpItem;
                    grpItem = NULL;
                    contactItem = NULL;
                }
            }
            if (itemOffline){
                contactItem = findContactItem(contact->id(), itemOffline);
                grpItem = NULL;
                if (contactItem){
                    grpItem = static_cast<GroupItem*>(contactItem->parent());
                    if ((status > STATUS_OFFLINE) || (grpItem->id() != contact->getGroup())){
                        grpItem->m_nContacts--;
                        addGroupForUpdate(grpItem->id());
                        deleteItem(contactItem);
                        contactItem = NULL;
                        bChanged = true;
                        if (m_bShowOnline && (grpItem->firstChild() == NULL)){
                            deleteItem(grpItem);
                            grpItem = NULL;
                            if (itemOffline->firstChild() == NULL){
                                deleteItem(itemOffline);
                                itemOffline = NULL;
                            }
                        }
                    }
                }
            }
            if ((unread == 0) && !bShow && (status <= STATUS_OFFLINE) && m_bShowOnline)
                break;
            DivItem *divItem;
            if (status <= STATUS_OFFLINE){
                if (itemOffline == NULL){
                    bChanged = true;
                    itemOffline = new DivItem(this, DIV_OFFLINE);
                    setOpen(itemOffline, true);
                }
                divItem = itemOffline;
            }else{
                divItem = itemOnline;
            }
            grpItem = findGroupItem(contact->getGroup(), divItem);
            if (grpItem == NULL){
                Group *grp = getContacts()->group(contact->getGroup());
                if (grp == NULL)
                    break;
                bChanged = true;
                grpItem = new GroupItem(divItem, grp, true);
                addSortItem(divItem);
            }
            contactItem = findContactItem(contact->id(), grpItem);
            if (contactItem){
                if (contactItem->update(contact, status, style, icons.c_str(), unread))
                    addSortItem(grpItem);
            }else{
                bChanged = true;
                new ContactItem(grpItem, contact, status, style, icons.c_str(), unread);
                grpItem->m_nContacts++;
                addGroupForUpdate(grpItem->id());
            }
        }
    }
    updContacts.clear();
    for (list<Q3ListViewItem*>::iterator it_sort = sortItems.begin(); it_sort != sortItems.end(); ++it_sort){
        if ((*it_sort)->firstChild() == NULL)
            continue;
        (*it_sort)->sort();
        bChanged = true;
    }
    sortItems.clear();
    center(x, y, 0, 0);
    viewport()->setUpdatesEnabled(true);
    if (bChanged){
        viewport()->repaint();
    }else{
        for (list<Q3ListViewItem*>::iterator it = updatedItems.begin(); it != updatedItems.end(); ++it)
            (*it)->repaint();
    }
    updatedItems.clear();
}

const unsigned UPDATE_TIME = 800;

void UserListBase::addGroupForUpdate(unsigned long id)
{
    for (list<unsigned long>::iterator it = updGroups.begin(); it != updGroups.end(); ++it){
        if (*it == id)
            return;
    }
    updGroups.push_back(id);
    if (!m_bDirty){
        m_bDirty = true;
        updTimer->start(800);
    }
}

void UserListBase::addContactForUpdate(unsigned long id)
{
    for (list<unsigned long>::iterator it = updContacts.begin(); it != updContacts.end(); ++it){
        if (*it == id)
            return;
    }
    updContacts.push_back(id);
    if (!m_bDirty){
        m_bDirty = true;
        updTimer->start(800);
    }
}

void UserListBase::drawItem(UserViewItemBase *base, QPainter *p, const QColorGroup &cg, int width, int margin)
{
    if (base->type() == DIV_ITEM){
        DivItem *divItem = static_cast<DivItem*>(base);
        QString text;
        switch (divItem->m_type){
        case DIV_ONLINE:
            text = i18n("Online");
            break;
        case DIV_OFFLINE:
            text = i18n("Offline");
            break;
        }
        QFont f(font());
        int size = f.pixelSize();
        if (size <= 0){
            size = f.pointSize();
            f.setPointSize(size * 3 / 4);
        }else{
            f.setPixelSize(size * 3 / 4);
        }
        p->setFont(f);
        int x = base->drawText(p, 24 + margin, width, text);
        base->drawSeparator(p, x, width);
    }
}

void UserListBase::addSortItem(Q3ListViewItem *item)
{
    for (list<Q3ListViewItem*>::iterator it = sortItems.begin(); it != sortItems.end(); ++it){
        if ((*it) == item)
            return;
    }
    sortItems.push_back(item);
}

void UserListBase::addUpdatedItem(Q3ListViewItem *item)
{
    for (list<Q3ListViewItem*>::iterator it = updatedItems.begin(); it != updatedItems.end(); ++it){
        if ((*it) == item)
            return;
    }
    updatedItems.push_back(item);
}

unsigned UserListBase::getUnread(unsigned)
{
    return 0;
}

void UserListBase::fill()
{
    m_pressedItem = NULL;
    clear();
    GroupItem *grpItem;
    ContactItem *contactItem;
    UserViewItemBase *divItem;
    UserViewItemBase *divItemOnline = NULL;
    UserViewItemBase *divItemOffline = NULL;
    ContactList *list = getContacts();
    ContactList::GroupIterator grp_it;
    ContactList::ContactIterator contact_it;
    Group *grp;
    Contact *contact;
    switch (m_groupMode){
    case 0:
        divItemOnline  = NULL;
        divItemOffline = NULL;
        while ((contact = ++contact_it) != NULL){
            if (contact->getIgnore() || (contact->getFlags() & CONTACT_TEMPORARY))
                continue;
            unsigned style;
            string icons;
            unsigned status = getUserStatus(contact, style, icons);
            unsigned unread = getUnread(contact->id());
            bool bShow = false;
            ListUserData *data = (ListUserData*)contact->getUserData(CorePlugin::m_plugin->list_data_id);
            if (data && data->ShowAlways.bValue)
                bShow = true;
            if ((unread == 0) && !bShow && (status <= STATUS_OFFLINE) && m_bShowOnline)
                continue;
            divItem = (status <= STATUS_OFFLINE) ? divItemOffline : divItemOnline;
            if (divItem == NULL){
                if (status <= STATUS_OFFLINE){
                    divItemOffline = new DivItem(this, DIV_OFFLINE);
                    setOpen(divItemOffline, true);
                    divItem = divItemOffline;
                }else{
                    divItemOnline = new DivItem(this, DIV_ONLINE);
                    setOpen(divItemOnline, true);
                    divItem = divItemOnline;
                }
            }
            new ContactItem(divItem, contact, status, style, icons.c_str(), unread);
        }
        break;
    case 1:
        if (m_bShowEmpty){
            while ((grp = ++grp_it) != NULL){
                if (grp->id() == 0)
                    continue;
                grpItem = new GroupItem(this, grp, true);
            }
            grpItem = new GroupItem(this, list->group(0), true);
        }
        while ((contact = ++contact_it) != NULL){
            if (contact->getIgnore() || (contact->getFlags() & CONTACT_TEMPORARY))
                continue;
            unsigned style;
            string icons;
            unsigned status = getUserStatus(contact, style, icons);
            unsigned unread = getUnread(contact->id());
            bool bShow = false;
            ListUserData *data = (ListUserData*)contact->getUserData(CorePlugin::m_plugin->list_data_id);
            if (data && data->ShowAlways.bValue)
                bShow = true;
            if ((status <= STATUS_OFFLINE) && !bShow && (unread == 0) && m_bShowOnline)
                continue;
            grpItem = findGroupItem(contact->getGroup());
            if (grpItem == NULL){
                grp = list->group(contact->getGroup());
                if (grp)
                    grpItem = new GroupItem(this, grp, true);
                if (grpItem == NULL)
                    continue;
            }
            contactItem = new ContactItem(grpItem, contact, status, style, icons.c_str(), unread);
            grpItem->m_nContacts++;
            if ((status > STATUS_OFFLINE) && !m_bShowOnline){
                grpItem->m_nContactsOnline++;
                contactItem->m_bOnline = true;
            }
        }
        break;
    case 2:
        divItemOnline = new DivItem(this, DIV_ONLINE);
        setOpen(divItemOnline, true);
        if (m_bShowEmpty){
            while ((grp = ++grp_it) != NULL){
                if (grp->id() == 0)
                    continue;
                grpItem = new GroupItem(divItemOnline, grp, false);
            }
            grpItem = new GroupItem(divItemOnline, list->group(0), false);
        }
        if (!m_bShowOnline){
            divItemOffline = new DivItem(this, DIV_OFFLINE);
            setOpen(divItemOffline, true);
            grp_it.reset();
            if (m_bShowEmpty){
                while ((grp = ++grp_it) != NULL){
                    if (grp->id() == 0)
                        continue;
                    grpItem = new GroupItem(divItemOffline, grp, true);
                }
                grpItem = new GroupItem(divItemOnline, list->group(0), true);
            }
        }
        while ((contact = ++contact_it) != NULL){
            if (contact->getIgnore() || (contact->getFlags() & CONTACT_TEMPORARY))
                continue;
            unsigned style;
            string icons;
            unsigned status = getUserStatus(contact, style, icons);
            unsigned unread = getUnread(contact->id());
            bool bShow = false;
            ListUserData *data = (ListUserData*)contact->getUserData(CorePlugin::m_plugin->list_data_id);
            if (data && data->ShowAlways.bValue)
                bShow = true;
            if ((unread == 0) && !bShow && (status <= STATUS_OFFLINE) && m_bShowOnline)
                continue;
            if (status <= STATUS_OFFLINE){
                if (divItemOffline == NULL){
                    divItemOffline = new DivItem(this, DIV_OFFLINE);
                    setOpen(divItemOffline, true);
                }
                divItem = divItemOffline;
            }else{
                divItem = divItemOnline;
            }
            grpItem = findGroupItem(contact->getGroup(), divItem);
            if (grpItem == NULL){
                Group *grp = getContacts()->group(contact->getGroup());
                if (grp == NULL)
                    continue;
                grpItem = new GroupItem(divItem, grp, true);
            }
            new ContactItem(grpItem, contact, status, style, icons.c_str(), unread);
            grpItem->m_nContacts++;
        }
        break;
    }
    adjustColumn();
}

static void resort(Q3ListViewItem *item)
{
    if (!item->isExpandable())
        return;
    item->sort();
    for (item = item->firstChild(); item; item = item = item->nextSibling())
        resort(item);
}

void *UserListBase::processEvent(Event *e)
{
    if (e->type() == EventRepaintView){
        sort();
        for (Q3ListViewItem *item = firstChild(); item; item = item->nextSibling())
            resort(item);
        viewport()->repaint();
    }
    if (m_bInit){
        switch (e->type()){
        case EventGroupCreated:{
                Group *g = (Group*)(e->param());
                addGroupForUpdate(g->id());
                break;
            }
        case EventGroupChanged:{
                Group *g = (Group*)(e->param());
                addGroupForUpdate(g->id());
                break;
            }
        case EventGroupDeleted:{
                Group *g = (Group*)(e->param());
                for (list<unsigned long>::iterator it = updGroups.begin(); it != updGroups.end(); ++it){
                    if (*it == g->id()){
                        updGroups.erase(it);
                        break;
                    }
                }
                Q3ListViewItem *item = findGroupItem(g->id());
                deleteItem(item);
                break;
            }
        case EventContactCreated:{
                Contact *c = (Contact*)(e->param());
                if (!c->getIgnore() && ((c->getFlags() & CONTACT_TEMPORARY) == 0))
                    addContactForUpdate(c->id());
                break;
            }
        case EventContactStatus:
        case EventContactChanged:{
                Contact *c = (Contact*)(e->param());
                if (!c->getIgnore() && ((c->getFlags() & CONTACT_TEMPORARY) == 0)){
                    addContactForUpdate(c->id());
                }else{
                    Event e(EventContactDeleted, c);
                    processEvent(&e);
                }
                break;
            }
        case EventMessageReceived:{
                Message *msg = (Message*)(e->param());
                if (msg->type() == MessageStatus){
                    Contact *contact = getContacts()->contact(msg->contact());
                    if (contact)
                        addContactForUpdate(contact->id());
                }
                break;
            }
        case EventContactDeleted:{
                Contact *g = (Contact*)(e->param());
                for (list<unsigned long>::iterator it = updContacts.begin(); it != updContacts.end(); ++it){
                    if (*it == g->id()){
                        updContacts.erase(it);
                        break;
                    }
                }
                ContactItem *item = findContactItem(g->id());
                if (item){
                    if (m_groupMode){
                        GroupItem *grpItem = static_cast<GroupItem*>(item->parent());
                        grpItem->m_nContacts--;
                        if (item->m_bOnline)
                            grpItem->m_nContactsOnline--;
                        addGroupForUpdate(grpItem->id());
                        deleteItem(item);
                        if ((m_groupMode == 2) &&
                                (grpItem->firstChild() == NULL) &&
                                m_bShowOnline){
                            DivItem *div = static_cast<DivItem*>(grpItem->parent());
                            if (div->state() == DIV_OFFLINE){
                                deleteItem(grpItem);
                                if (div->firstChild() == NULL)
                                    deleteItem(div);
                            }
                        }
                    }else{
                        Q3ListViewItem *p = item->parent();
                        deleteItem(item);
                        if (p->firstChild() == NULL)
                            deleteItem(p);
                    }
                }
                break;
            }
        }
    }
    return ListView::processEvent(e);
}

GroupItem *UserListBase::findGroupItem(unsigned id, Q3ListViewItem *p)
{
    for (Q3ListViewItem *item = p ? p->firstChild() : firstChild(); item; item = item->nextSibling()){
        UserViewItemBase *i = static_cast<UserViewItemBase*>(item);
        if (i->type() == GRP_ITEM){
            GroupItem *grpItem = static_cast<GroupItem*>(item);
            if (grpItem->id() == id)
                return grpItem;
        }
        if (item->isExpandable()){
            GroupItem *res = findGroupItem(id, item);
            if (res)
                return res;
        }
    }
    return NULL;
}

ContactItem *UserListBase::findContactItem(unsigned id, Q3ListViewItem *p)
{
    for (Q3ListViewItem *item = p ? p->firstChild() : firstChild(); item; item = item->nextSibling()){
        UserViewItemBase *i = static_cast<UserViewItemBase*>(item);
        if (i->type() == USR_ITEM){
            ContactItem *contactItem = static_cast<ContactItem*>(item);
            if (contactItem->id() == id)
                return contactItem;
        }
        if (item->isExpandable()){
            ContactItem *res = findContactItem(id, item);
            if (res)
                return res;
        }
    }
    return NULL;
}

unsigned UserListBase::getUserStatus(Contact *contact, unsigned &style, string &icons)
{
    style = 0;
    string wrkIcons;
    const char *statusIcon;
    unsigned long status = contact->contactInfo(style, statusIcon, &wrkIcons);
    if (statusIcon)
        icons = statusIcon;
    if (wrkIcons.length()){
        if (icons.length())
            icons += ',';
        icons += wrkIcons;
    }
    return status;
}

void UserListBase::deleteItem(Q3ListViewItem *item)
{
    if (item == NULL)
        return;
    if (item == currentItem()) {
        Q3ListViewItem *nextItem = item->nextSibling();
        if (nextItem == NULL){
            if (item->parent()){
                nextItem = item->parent()->firstChild();
            }else{
                nextItem = firstChild();
            }
            for (; nextItem ; nextItem = nextItem->nextSibling())
                if (nextItem->nextSibling() == item)
                    break;
        }
        if ((nextItem == NULL) && item->parent()){
            nextItem = item->parent();
            if (nextItem->firstChild() && (nextItem->firstChild() != item)){
                for (nextItem = nextItem->firstChild(); nextItem; nextItem = nextItem->nextSibling())
                    if (nextItem->nextSibling() == item)
                        break;
            }
        }
        if (nextItem){
            setCurrentItem(nextItem);
            ensureItemVisible(nextItem);
        }
    }
    delete item;
}

UserList::UserList(QWidget *parent)
        : UserListBase(parent)
{
    m_bInit  = true;
    setMenu(0);
    fill();
}

UserList::~UserList()
{
    emit finished();
}

void UserList::drawItem(UserViewItemBase *base, QPainter *p, const QColorGroup &cg, int width, int margin)
{
    if (base->type() == GRP_ITEM){
        GroupItem *item = static_cast<GroupItem*>(base);
        p->setFont(font());
        QString text;
        if (item->id()){
            Group *grp = getContacts()->group(item->id());
            if (grp){
                text = grp->getName();
            }else{
                text = "???";
            }
        }else{
            text = i18n("Not in list");
        }
        int x = drawIndicator(p, 2 + margin, item, isGroupSelected(item->id()), cg);
        if (!CorePlugin::m_plugin->getUseSysColors())
            p->setPen(CorePlugin::m_plugin->getColorGroup());
        x = item->drawText(p, x, width, text);
        item->drawSeparator(p, x, width);
        return;
    }
    if (base->type() == USR_ITEM){
        ContactItem *item = static_cast<ContactItem*>(base);
        int x = drawIndicator(p, 2 + margin, item, isSelected(item->id()), cg);
        if (!item->isSelected() || !hasFocus() || !CorePlugin::m_plugin->getUseDblClick()){
            if (CorePlugin::m_plugin->getUseSysColors()){
                if (item->status() != STATUS_ONLINE)
                    p->setPen(palette().disabled().text());
            }else{
                switch (item->status()){
                case STATUS_ONLINE:
                    break;
                case STATUS_AWAY:
                    p->setPen(CorePlugin::m_plugin->getColorAway());
                    break;
                case STATUS_NA:
                    p->setPen(CorePlugin::m_plugin->getColorNA());
                    break;
                case STATUS_DND:
                    p->setPen(CorePlugin::m_plugin->getColorDND());
                    break;
                default:
                    p->setPen(CorePlugin::m_plugin->getColorOffline());
                    break;
                }
            }
        }
        x = item->drawText(p, x, width, item->text(CONTACT_TEXT));
        return;
    }
    UserListBase::drawItem(base, p, cg, width, margin);
}

bool UserList::isSelected(unsigned id)
{
    for (list<unsigned>::iterator it = selected.begin(); it != selected.end(); ++it){
        if ((*it) == id)
            return true;
    }
    return false;
}

bool UserList::isGroupSelected(unsigned id)
{
    bool bRes = false;
    ContactList::ContactIterator it;
    Contact *contact;
    while ((contact = ++it) != NULL){
        if (contact->getGroup() != id)
            continue;
        if (!isSelected(contact->id()))
            return false;
        bRes = true;
    }
    return bRes;
}

#if COMPAT_QT_VERSION < 0x030000
#define CHECK_OFF	QCheckBox::Off
#define CHECK_ON	QCheckBox::On
#define CHECK_NOCHANGE	QCheckBox::NoChange
#else
#define CHECK_OFF	QStyle::State_Off
#define CHECK_ON	QStyle::State_On
#define CHECK_NOCHANGE	QStyle::State_NoChange
#endif

int UserList::drawIndicator(QPainter *p, int x, Q3ListViewItem *item, bool bState, const QColorGroup &cg)
{
    int state = bState ? CHECK_ON : CHECK_OFF;
#if COMPAT_QT_VERSION < 0x030000
    QSize s = style().indicatorSize();
    QPixmap pixInd(s.width(), s.height());
    QPainter pInd(&pixInd);
    style().drawIndicator(&pInd, 0, 0, s.width(), s.height(), cg, state);
    pInd.end();
    QBitmap mInd(s.width(), s.height());
    pInd.begin(&mInd);
    style().drawIndicatorMask(&pInd, 0, 0, s.width(), s.height(), state);
    pInd.end();
    pixInd.setMask(mInd);
    p->drawPixmap(x, (item->height() - s.height()) / 2, pixInd);
    x += s.width() + 2;
#else
    int w = style()->pixelMetric(QStyle::PM_IndicatorWidth);
    int h = style()->pixelMetric(QStyle::PM_IndicatorHeight);
    const QStyleOption *option;
    style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, option, p, NULL);
    x += w + 2;
#endif
    return x;
}

int UserListBase::heightItem(UserViewItemBase*)
{
    QFontMetrics fm(font());
    int h = fm.height() + 4;
    if (h < 22)
        h = 22;
    return h;
}

void UserList::contentsMouseReleaseEvent(QMouseEvent *e)
{
    Q3ListViewItem *list_item = itemAt(contentsToViewport(e->pos()));
    if (list_item == NULL)
        return;
    switch (static_cast<UserViewItemBase*>(list_item)->type()){
    case USR_ITEM:{
            ContactItem *item = static_cast<ContactItem*>(list_item);
            if (isSelected(item->id())){
                for (list<unsigned>::iterator it = selected.begin(); it != selected.end(); ++it){
                    if ((*it) == item->id()){
                        selected.erase(it);
                        break;
                    }
                }
            }else{
                selected.push_back(item->id());
            }
            item->repaint();
            item->parent()->repaint();
            emit selectChanged();
            break;
        }
    case GRP_ITEM:{
            GroupItem *item = static_cast<GroupItem*>(list_item);
            if (isGroupSelected(item->id())){
                for (Q3ListViewItem *i = item->firstChild(); i; i = i->nextSibling()){
                    ContactItem *ci = static_cast<ContactItem*>(i);
                    list<unsigned>::iterator it;
                    for (it = selected.begin(); it != selected.end(); ++it){
                        if ((*it) == ci->id()){
                            selected.erase(it);
                            break;
                        }
                    }
                    ci->repaint();
                }
            }else{
                for (Q3ListViewItem *i = item->firstChild(); i; i = i->nextSibling()){
                    ContactItem *ci = static_cast<ContactItem*>(i);
                    list<unsigned>::iterator it;
                    for (it = selected.begin(); it != selected.end(); ++it)
                        if ((*it) == ci->id())
                            break;
                    if (it == selected.end()){
                        selected.push_back(ci->id());
                        ci->repaint();
                    }
                }
            }
            item->repaint();
            emit selectChanged();
            break;
        }
    }
    m_pressedItem = NULL;
}

#ifndef WIN32
#include "userlist.moc"
#endif

