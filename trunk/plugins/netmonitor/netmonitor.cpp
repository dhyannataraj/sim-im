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

#include <qtimer.h>
#include <qwidget.h>

#include "misc.h"
#include "core_consts.h"

#include "netmonitor.h"
#include "monitor.h"

using namespace std;
using namespace SIM;

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

static DataDef monitorData[] =
    {
        { "LogLevel", DATA_ULONG, 1, DATA(7) },
        { "LogPackets", DATA_STRING, 1, 0 },
        { "Geometry", DATA_LONG, 5, DATA(-1) },
        { "Show", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

NetmonitorPlugin::NetmonitorPlugin(unsigned base, Buffer *config)
        : Plugin(base)
{
    load_data(monitorData, &data, config);

    if (!getLogPackets().isEmpty()){
        QString packets = getLogPackets();
        while (packets.length()){
            QString v = getToken(packets, ',');
            setLogType(v.toULong(), true);
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
    EventCommandCreate(cmd).process();

    EventArg e("-m", I18N_NOOP("Show network monitor"));
    if (e.process() || getShow())
        showMonitor();
}

NetmonitorPlugin::~NetmonitorPlugin()
{
    EventCommandRemove(CmdNetMonitor).process();

    delete monitor;

    free_data(monitorData, &data);
}

QCString NetmonitorPlugin::getConfig()
{
    saveState();
    setShow(monitor != NULL);
    QString packets;
    QValueList<unsigned>::ConstIterator it;
    for (it = m_packets.constBegin(); it != m_packets.constEnd(); ++it){
        if (packets.length())
            packets += ',';
        packets += QString::number(*it);
    }
    setLogPackets(packets);
    return save_data(monitorData, &data);
}

bool NetmonitorPlugin::isLogType(unsigned id)
{
    return ( m_packets.find( id ) != m_packets.end() );
}

void NetmonitorPlugin::setLogType(unsigned id, bool bLog)
{
    QValueList<unsigned>::iterator it = m_packets.find(id);
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
        bool bPos = (data.geometry[LEFT].toLong() != NO_DATA) && (data.geometry[TOP].toLong() != NO_DATA);
        bool bSize = (data.geometry[WIDTH].toLong() != NO_DATA) && (data.geometry[HEIGHT].toLong() != NO_DATA);
        restoreGeometry(monitor, data.geometry, bPos, bSize);
        connect(monitor, SIGNAL(finished()), this, SLOT(finished()));
    }
    raiseWindow(monitor);
}

bool NetmonitorPlugin::processEvent(Event *e)
{
    if (e->type() == eEventCommandExec){
        EventCommandExec *ece = static_cast<EventCommandExec*>(e);
        CommandDef *cmd = ece->cmd();
        if (cmd->id == CmdNetMonitor){
            showMonitor();
            return true;
        }
    }
    return false;
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

#ifndef NO_MOC_INCLUDES
#include "netmonitor.moc"
#endif
