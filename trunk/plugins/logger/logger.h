/***************************************************************************
                          logger.h  -  description
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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <qobject.h>

#include "cfg.h"
#include "event.h"
#include "plugins.h"

class QFile;
const unsigned short L_PACKETS = 0x08;
// const unsigned short L_EVENTS  = 0x10;

struct LoggerData
{
    SIM::Data LogLevel;
    SIM::Data LogPackets;
    SIM::Data File;
};

class QFile;

class LoggerPlugin : public QObject, public SIM::Plugin, public SIM::EventReceiver
{
    Q_OBJECT
public:
    LoggerPlugin(unsigned, Buffer*);
    virtual ~LoggerPlugin();
    PROP_ULONG(LogLevel);
    PROP_STR(LogPackets);
    PROP_STR(File);
    bool isLogType(unsigned id);
    void setLogType(unsigned id, bool bLog);
protected:
//    bool eventFilter(QObject *o, QEvent *e);
    std::list<unsigned> m_packets;
    virtual QWidget *createConfigWindow(QWidget *parent);
    virtual QCString getConfig();
    virtual bool processEvent(SIM::Event *e);
    void openFile();
    QFile *m_file;
    LoggerData data;
    bool m_bFilter;
    friend class LogConfig;
};

#endif

