/***************************************************************************
                          ignorelist.h  -  description
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

#ifndef _IGNORELIST_H
#define _IGNORELIST_H

#include "ignorelistbase.h"

#include "event.h"

class Q3ListViewItem;

class IgnoreList : public QWidget, public Ui::IgnoreListBase, public SIM::EventReceiver
{
    Q_OBJECT
public:
    IgnoreList(QWidget *parent);
protected slots:
    void deleteItem(Q3ListViewItem*);
    void dragStart();
    void dragEnter(QMimeSource*);
    void drop(QMimeSource*);
protected:
    virtual bool processEvent(SIM::Event *e);
    void removeItem(Q3ListViewItem*);
    void updateItem(Q3ListViewItem*, SIM::Contact*);
    void unignoreItem(Q3ListViewItem*);
    Q3ListViewItem *findItem(SIM::Contact*);
};

#endif

