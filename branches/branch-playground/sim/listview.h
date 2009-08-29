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
#include "event.h"

#include <q3dragobject.h>
#include <q3listview.h>

class Q3DragObject;

const unsigned long MenuListView		= 0x100;
const unsigned long CmdListDelete	= 0x100;

class QTimer;

class EXPORT ListView : public Q3ListView, public SIM::EventReceiver
{
    Q_OBJECT
    Q_PROPERTY( int expandingColumn READ expandingColumn WRITE setExpandingColumn )
public:
    ListView(QWidget *parent, const char *name=NULL);
    ~ListView();
    int   expandingColumn() const;
    void  setExpandingColumn(int);
    Q3ListViewItem *m_pressedItem;
    void  startDrag(Q3DragObject*);
    void acceptDrop(bool bAccept);
    void setMenu(unsigned long menuId);
signals:
    void clickItem(Q3ListViewItem*);
    void deleteItem(Q3ListViewItem*);
    void dragStart();
    void dragEnter(QMimeSource*);
    void drop(QMimeSource*);
public slots:
    void adjustColumn();
    virtual void startDrag();
    void sizeChange(int,int,int);
protected:
    virtual bool getMenu(Q3ListViewItem *item, unsigned long &id, void *&param);
    virtual bool processEvent(SIM::Event *e);
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual Q3DragObject *dragObject();
    void viewportContextMenuEvent( QContextMenuEvent *e);
    void viewportMousePressEvent(QMouseEvent *e);
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsDragEnterEvent(QDragEnterEvent *e);
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDropEvent(QDropEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void showPopup(Q3ListViewItem *item, QPoint p);
    int m_expandingColumn;
    unsigned long m_menuId;
    QTimer	 *m_resizeTimer;
    bool m_bAcceptDrop;
    static bool s_bInit;
};

class EXPORT ContactDragObject : public Q3StoredDrag
{
    Q_OBJECT
public:
    ContactDragObject(ListView *dragSource, SIM::Contact *contact);
    ~ContactDragObject();
    static bool canDecode(QMimeSource*);
    static SIM::Contact *decode(QMimeSource*);
protected:
    unsigned long m_id;
};

#endif

