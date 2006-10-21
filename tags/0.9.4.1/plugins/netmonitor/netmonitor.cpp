/***************************************************************************
                          netmonitor.cpp  -  description
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

#include "netmonitor.h"
#include "simapi.h"
#include "monitor.h"

#include <qtimer.h>
#include <qwidget.h>

Plugin *createNetmonitorPlugin(unsigned base, bool, Buffer *config)
{
    Plugin *plugin = new NetmonitorPlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Network monitor"),
        I18N_NOOP("Plugin provides monitoring of net and messages\n"
                  "For show monitor on start run sim -m"),
        VERSION,
        createNetmonitorPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

/*
typedef struct NetMonitorData
{
    unsigned long	LogLevel;
	char			*LogPackets;
    long			geometry[5];
    bool			Show;
} NetMonitorData;
*/
static DataDef monitorData[] =
    {
        { "LogLevel", DATA_ULONG, 1, DATA(7) },
        { "LogPackets", DATA_STRING, 1, 0 },
        { "Geometry", DATA_LONG, 5, DATA(-1) },
        { "Show", DATA_BOOL, 1, 0 },
        { NULL, 0, 0, 0 }
    };

NetmonitorPlugin::NetmonitorPlugin(unsigned base, Buffer *config)
        : Plugin(base)
{
    load_data(monitorData, &data, config);

    if (getLogPackets()){
        string packets = getLogPackets();
        while (packets.length()){
            string v = getToken(packets, ',');
            setLogType(atol(v.c_str()), true);
        }
    }

    monitor = NULL;
    CmdNetMonitor = registerType();

    Command cmd;
    cmd->id          = CmdNetMonitor;
    cmd->text        = I18N_NOOP("Network monitor");
    cmd->icon        = "network";
    cmd->bar_id      = ToolBarMain;
    cmd->menu_id     = MenuMain;
    cmd->menu_grp    = 0x8000;
    cmd->flags		= COMMAND_DEFAULT;

    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

    string value;
    CmdParam p = { "-m", I18N_NOOP("Show network monitor"), &value };
    Event e(EventArg, &p);
    if (e.process() || getShow())
        showMonitor();
}

NetmonitorPlugin::~NetmonitorPlugin()
{
    Event eCmd(EventCommandRemove, (void*)CmdNetMonitor);
    eCmd.process();

    if (monitor)
        delete monitor;

    free_data(monitorData, &data);
}

string NetmonitorPlugin::getConfig()
{
    saveState();
    setShow(monitor != NULL);
    string packets;
    for (list<unsigned>::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
        if (packets.length())
            packets += ',';
        packets += number(*it);
    }
    return save_data(monitorData, &data);
}

bool NetmonitorPlugin::isLogType(unsigned id)
{
    for (list<unsigned>::iterator it = m_packets.begin(); it != m_packets.end(); ++it){
        if ((*it) == id)
            return true;
    }
    return false;
}

void NetmonitorPlugin::setLogType(unsigned id, bool bLog)
{
    list<unsigned>::iterator it;
    for (it = m_packets.begin(); it != m_packets.end(); ++it){
        if ((*it) == id)
            break;
    }
    if (bLog){
        if (it == m_packets.end())
            m_packets.push_back(id);
    }else{
        if (it != m_packets.end())
            m_packets.erase(it);
    }
}

const unsigned NO_DATA = (unsigned)(-1);

void NetmonitorPlugin::showMonitor()
{
    if (monitor == NULL)
    {
        monitor = new MonitorWindow(this);
        bool bPos = (data.geometry[LEFT].value != NO_DATA) && (data.geometry[TOP].value != NO_DATA);
        bool bSize = (data.geometry[WIDTH].value != NO_DATA) && (data.geometry[HEIGHT].value != NO_DATA);
        restoreGeometry(monitor, data.geometry, bPos, bSize);
        connect(monitor, SIGNAL(finished()), this, SLOT(finished()));
    }
    raiseWindow(monitor);
}

void *NetmonitorPlugin::processEvent(Event *e)
{
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdNetMonitor){
            showMonitor();
            return monitor;
        }
    }
    return NULL;
}

void NetmonitorPlugin::finished()
{
    saveState();
    QTimer::singleShot(0, this, SLOT(realFinished()));
}

void NetmonitorPlugin::realFinished()
{
    delete monitor;
    monitor = NULL;
}

void NetmonitorPlugin::saveState()
{
    if (monitor == NULL)
        return;
    saveGeometry(monitor, data.geometry);
}

#ifndef _MSC_VER
#include "netmonitor.moc"
#endif
