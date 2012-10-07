/***************************************************************************
                          userview.h  -  description
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

#ifndef _USERVIEW_H
#define _USERVIEW_H

#include "userlist.h"

class CorePlugin;
class IntLineEdit;
class QTimer;
class TipLabel;

struct BlinkCount
{
    unsigned long	id;
    unsigned		count;
};

class UserView : public UserListBase
{
    Q_OBJECT
public:
    UserView();
    ~UserView();
    CorePlugin *m_plugin;
    IntLineEdit *edtGroup;
    IntLineEdit *edtContact;
    QListViewItem *mTipItem;
    bool	 m_bUnreadBlink;
public slots:
    void editGroupEnter();
    void editContactEnter();
    void editEscape();
    void renameGroup();
    void renameContact();
    void deleteGroup(void*);
    void deleteContact(void*);
    void joinContacts(void*);
    void cancelJoinContacts(void*);
    void showTip();
    void hideTip();
    void tipDestroyed();
    void searchTipDestroyed();
    void blink();
    void unreadBlink();
    void doDrop();
    void doClick();
    void dragScroll();
protected:
    virtual void fill();
    virtual void drawItem(UserViewItemBase *base, QPainter *p, const QColorGroup &cg, int width, int margin);
    virtual int heightItem(UserViewItemBase *base);
    virtual unsigned getUnread(unsigned contact_id);
    virtual bool getMenu(QListViewItem *item, unsigned long &id, void* &param);
    virtual bool processEvent(SIM::Event*);
    void paintEmptyArea(QPainter *p, const QRect &r);
    bool eventFilter(QObject *obj, QEvent *e);
    void setGroupMode(unsigned mode, bool bFirst=false);
    void keyPressEvent(QKeyEvent *e);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    void contentsMouseDoubleClickEvent(QMouseEvent *e);
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDropEvent(QDropEvent *e);
    void focusOutEvent(QFocusEvent*);
    void sortAll();
    void sortAll(QListViewItem*);
    void dragEvent(QDropEvent *e, bool isDrop);
    void search(std::list<QListViewItem*> &items);
    void search(QListViewItem*, std::list<QListViewItem*> &items);
    void stopSearch();
    void showTip(QListViewItem*);
    QDragObject *dragObject();
    virtual void deleteItem(QListViewItem *item);
    std::list<BlinkCount> blinks;
    std::list<QListViewItem*> closed_items;
    unsigned long m_dropContactId;
    QListViewItem *m_dropItem;
    QListViewItem *m_current;
    TipLabel *m_tip;
    TipLabel *m_searchTip;
    bool	 m_bBlink;
    QTimer   *tipTimer;
    QTimer	 *blinkTimer;
    QTimer	 *unreadTimer;
    QString	 m_search;
    bool	 m_bRemoveHistory;
    QListViewItem *m_searchItem;
};

#endif

