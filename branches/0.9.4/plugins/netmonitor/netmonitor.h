/***************************************************************************
                          netmonitor.h  -  description
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

#ifndef _NETMONITOR_H
#define _NETMONITOR_H

#include "simapi.h"
#include "stl.h"

typedef struct NetMonitorData
{
    Data	LogLevel;
    Data	LogPackets;
    Data	geometry[5];
    Data	Show;
} NetMonitorData;

class MonitorWindow;

class NetmonitorPlugin : public QObject, public Plugin, public EventReceiver
{
    Q_OBJECT
public:
    NetmonitorPlugin(unsigned, Buffer *name);
    virtual ~NetmonitorPlugin();
    PROP_ULONG(LogLevel);
    PROP_STR(LogPackets);
    PROP_BOOL(Show);
    bool isLogType(unsigned id);
    void setLogType(unsigned id, bool bLog);
protected slots:
    void finished();
    void realFinished();
protected:
    virtual void *processEvent(Event*);
    virtual string getConfig();
    void showMonitor();
    void saveState();
    unsigned long CmdNetMonitor;
    list<unsigned> m_packets;
    NetMonitorData data;
    MonitorWindow *monitor;
};

#endif

