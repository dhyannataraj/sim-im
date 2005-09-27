/***************************************************************************
                          windock.h  -  description
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

#ifndef _WINDOCK_H
#define _WINDOCK_H

#include "simapi.h"

#include <QEvent>

typedef struct WinDockData
{
    Data	AutoHide;
    Data	State;
    Data	Height;
    Data	Width;
} WinDocData;

class QTimer;

class WinDockPlugin : public QObject, public Plugin, public EventReceiver
{
    Q_OBJECT
public:
    WinDockPlugin(unsigned, Buffer*);
    virtual ~WinDockPlugin();
    PROP_BOOL(AutoHide);
    PROP_USHORT(State);
    PROP_ULONG(Height);
    PROP_ULONG(Width);
    void enableAutoHide(bool);
protected slots:
    void slotSetState();
    void slotAutoHide();
protected:
    virtual void *processEvent(Event*);
    virtual bool eventFilter(QObject*, QEvent*);
    virtual string getConfig();
    QWidget *getMainWindow();
    unsigned CmdAutoHide;
    bool m_bInit;
    void init();
    void uninit();
    QTimer *m_autoHide;
    WinDockData data;
};

#endif

