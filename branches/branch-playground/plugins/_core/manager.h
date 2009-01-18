/***************************************************************************
                          manager.h  -  description
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

#ifndef _MANAGER_H
#define _MANAGER_H

#include "ui_managerbase.h"
#include "event.h"
//Added by qt3to4:
#include <QCloseEvent>
#include <QResizeEvent>

class CorePlugin;

class ConnectionManager : public QDialog, public Ui::ConnectionManagerBase
{
    Q_OBJECT
public:
    ConnectionManager(bool bModal);
signals:
    void finished();
protected slots:
    void addClient();
    void removeClient();
    void upClient();
    void downClient();
    void updateClient();
    void selectionChanged();
protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    SIM::Client *currentClient();
    void fill(SIM::Client *current = NULL);
    bool m_bModal;
};

#endif

