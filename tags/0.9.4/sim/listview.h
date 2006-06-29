/***************************************************************************
                          listview.h  -  description
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

#ifndef _LISTVIEW_H
#define _LISTVIEW_H

#include "simapi.h"

#include <qlistview.h>
#include <qdragobject.h>

const unsigned long MenuListView		= 0x100;
const unsigned long CmdListDelete	= 0x100;

class QTimer;

class EXPORT ListView : public QListView, public EventReceiver
{
    Q_OBJECT
    Q_PROPERTY( int expandingColumn READ expandingColumn WRITE setExpandingColumn )
public:
    ListView(QWidget *parent, const char *name=NULL);
    ~ListView();
    int   expandingColumn() const;
    void  setExpandingColumn(int);
    QListViewItem *m_pressedItem;
    void  startDrag(QDragObject*);
    void acceptDrop(bool bAccept);
    void setMenu(unsigned long menuId);
signals:
    void clickItem(QListViewItem*);
    void deleteItem(QListViewItem*);
    void dragStart();
    void dragEnter(QMimeSource*);
    void drop(QMimeSource*);
public slots:
    void adjustColumn();
    virtual void startDrag();
    void sizeChange(int,int,int);
protected:
    virtual ProcessMenuParam *getMenu(QListViewItem *item);
    virtual void *processEvent(Event*);
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual QDragObject *dragObject();
    void viewportContextMenuEvent( QContextMenuEvent *e);
    void viewportMousePressEvent(QMouseEvent *e);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDropEvent(QDropEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void showPopup(QListViewItem *item, QPoint p);
    int m_expandingColumn;
    unsigned long m_menuId;
    ProcessMenuParam m_mp;
    QTimer	 *m_resizeTimer;
#if COMPAT_QT_VERSION < 0x030000
    QPoint m_mousePressPos;
#endif
    bool m_bAcceptDrop;
    static bool s_bInit;
};

class EXPORT ContactDragObject : public QStoredDrag
{
    Q_OBJECT
public:
    ContactDragObject(ListView *dragSource, Contact *contact);
    ~ContactDragObject();
    static bool canDecode(QMimeSource*);
    static Contact *decode(QMimeSource*);
protected:
    unsigned long m_id;
};

#endif

