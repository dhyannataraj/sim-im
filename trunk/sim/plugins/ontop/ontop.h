/***************************************************************************
                          ontop.h  -  description
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

#ifndef _ONTOP_H
#define _ONTOP_H

#include "simapi.h"

#ifdef WIN32
#include <windows.h>
#endif

const unsigned EventInTaskManager = 0x00030000;
const unsigned EventOnTop		  = 0x00030001;

typedef struct OnTopData
{
    SIM::Data	OnTop;
    SIM::Data	InTask;
    SIM::Data	ContainerOnTop;
} OnTopData;

class OnTopPlugin : public QObject, public SIM::Plugin, public SIM::EventReceiver
{
    Q_OBJECT
public:
    OnTopPlugin(unsigned, Buffer*);
    virtual ~OnTopPlugin();
protected:
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void *processEvent(SIM::Event*);
    virtual QWidget *createConfigWindow(QWidget *parent);
    virtual std::string getConfig();
    void getState();
    void setState();
    QWidget *getMainWindow();
    unsigned CmdOnTop;
    PROP_BOOL(OnTop);
    PROP_BOOL(InTask);
    PROP_BOOL(ContainerOnTop);
    OnTopData data;
#ifdef WIN32
    HWND m_state;
#endif
    friend class OnTopCfg;
};

#endif

