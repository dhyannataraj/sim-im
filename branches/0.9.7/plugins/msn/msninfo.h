/***************************************************************************
                          msninfo.h  -  description
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

#ifndef _MSNINFO_H
#define _MSNINFO_H

#include "ui_msninfobase.h"
#include "event.h"

struct MSNUserData;
class MSNClient;

class MSNInfo : public QWidget, public Ui::MSNInfoBase, public SIM::EventReceiver
{
    Q_OBJECT
public:
    MSNInfo(QWidget *parent, MSNUserData *data, MSNClient *client);
public slots:
    void apply();
    void apply(SIM::Client*, void*);
protected:
    virtual bool processEvent(SIM::Event *e);
    void fill();
    MSNUserData *m_data;
    MSNClient *m_client;
};


#endif

