/***************************************************************************
                          ontop.cpp  -  description
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

#include "ontop.h"
#include "ontopcfg.h"
#include "simapi.h"

#include <qapplication.h>
#include <qwidgetlist.h>

#ifdef WIN32
#include <windows.h>
#else
#include <kwin.h>
#endif

Plugin *createOnTopPlugin(unsigned base, bool, const char *config)
{
    Plugin *plugin = new OnTopPlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("On Top"),
        I18N_NOOP("Plugin provides main window allways on top"),
        VERSION,
        createOnTopPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

/*
typedef struct OnTopData
{
    bool OnTop;
    bool InTask;
} OnTopData;
*/
static DataDef onTopData[] =
    {
        { "OnTop", DATA_BOOL, 1, 1 },
        { "InTask", DATA_BOOL, 1, 0 },
        { NULL, 0, 0, 0 }
    };

OnTopPlugin::OnTopPlugin(unsigned base, const char *config)
        : Plugin(base)
{
    load_data(onTopData, &data, config);

    CmdOnTop = registerType();

    Command cmd;
    cmd->id          = CmdOnTop;
    cmd->text        = I18N_NOOP("Always on top");
    cmd->menu_id     = MenuMain;
    cmd->menu_grp    = 0x7000;
    cmd->flags		= COMMAND_CHECK_STATE;

    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

#if defined(WIN32) || defined (USE_KDE)
    qApp->installEventFilter(this);
#endif

    setState();
}

OnTopPlugin::~OnTopPlugin()
{
    Event eCmd(EventCommandRemove, (void*)CmdOnTop);
    eCmd.process();

    setOnTop(false);
    setState();
    free_data(onTopData, &data);
}

void *OnTopPlugin::processEvent(Event *e)
{
    if (e->type() == EventInit)
        setState();
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdOnTop){
            setOnTop(!getOnTop());
            setState();
            return cmd;
        }
    }
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdOnTop){
            getState();
            cmd->flags &= ~COMMAND_CHECKED;
            if (getOnTop())
                cmd->flags |= COMMAND_CHECKED;
            return cmd;
        }
    }
    return NULL;
}

string OnTopPlugin::getConfig()
{
    getState();
    return save_data(onTopData, &data);
}

QWidget *OnTopPlugin::getMainWindow()
{
    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    QWidget *w;
    while ( (w=it.current()) != 0 ) {
        ++it;
        if (w->inherits("MainWindow")){
            delete list;
            return w;
        }
    }
    delete list;
    return NULL;
}

void OnTopPlugin::getState()
{
#ifdef USE_KDE
    QWidget *main = getMainWindow();
    if (main == NULL) return;
    setOnTop(KWin::info(main->winId()).state & NET::StaysOnTop);
#endif
}

void OnTopPlugin::setState()
{
    QWidget *main = getMainWindow();
    if (main == NULL) return;
#ifdef WIN32
    HWND hState = HWND_NOTOPMOST;
    if (getOnTop()) hState = HWND_TOPMOST;
    SetWindowPos(main->winId(), hState, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    if (IsWindowUnicode(main->winId())){
        if (getInTask()){
            SetWindowLongW(main->winId(), GWL_EXSTYLE,
                           (GetWindowLongW(main->winId(), GWL_EXSTYLE) | WS_EX_APPWINDOW) & (~WS_EX_TOOLWINDOW));
        }else{
            SetWindowLongW(main->winId(), GWL_EXSTYLE,
                           (GetWindowLongW(main->winId(), GWL_EXSTYLE) & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);
        }
    }else{
        if (getInTask()){
            SetWindowLongA(main->winId(), GWL_EXSTYLE,
                           (GetWindowLongA(main->winId(), GWL_EXSTYLE) | WS_EX_APPWINDOW) & (~WS_EX_TOOLWINDOW));
        }else{
            SetWindowLongA(main->winId(), GWL_EXSTYLE,
                           (GetWindowLongA(main->winId(), GWL_EXSTYLE) & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW);
        }
    }
#else
#ifdef USE_KDE
    if (getOnTop()){
        KWin::setState(main->winId(), NET::StaysOnTop);
    }else{
        KWin::clearState(main->winId(), NET::StaysOnTop);
    }
    if (getInTask()){
        KWin::clearState(main->winId(), NET::SkipTaskbar);
    }else{
        KWin::setState(main->winId(), NET::SkipTaskbar);
    }
#endif
#endif
}

QWidget *OnTopPlugin::createConfigWindow(QWidget *parent)
{
#if defined(USE_KDE) || defined(WIN32)
    return new OnTopCfg(parent, this);
#endif
}

bool OnTopPlugin::eventFilter(QObject *o, QEvent *e)
{
#ifdef WIN32
    if ((e->type() == QEvent::WindowActivate) && getOnTop() &&
            !o->inherits("MainWindow") && o->inherits("QDialog")){
        QWidget *w = static_cast<QWidget*>(o);
        SetWindowPos(w->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    if ((e->type() == QEvent::WindowDeactivate) && getOnTop() &&
            !o->inherits("MainWindow") && o->inherits("QDialog")){
        QWidget *w = static_cast<QWidget*>(o);
        SetWindowPos(w->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
#endif
#ifdef USE_KDE
    if ((e->type() == QEvent::WindowActivate) && getOnTop() &&
            !o->inherits("MainWindow") && o->inherits("QDialog")){
        QWidget *w = static_cast<QWidget*>(o);
        KWin::setState(w->winId(), NET::StaysOnTop);
    }
    if ((e->type() == QEvent::WindowDeactivate) && getOnTop() &&
            !o->inherits("MainWindow") && o->inherits("QDialog")){
        QWidget *w = static_cast<QWidget*>(o);
        KWin::clearState(w->winId(), NET::StaysOnTop);
    }
#endif
    return QObject::eventFilter(o, e);
}

#ifdef WIN32

/**
 * DLL's entry point
 **/
int WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    return TRUE;
}

/**
 * This is to prevent the CRT from loading, thus making this a smaller
 * and faster dll.
 **/
extern "C" BOOL __stdcall _DllMainCRTStartup( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain( hinstDLL, fdwReason, lpvReserved );
}

#endif

#ifndef WIN32
#include "ontop.moc"
#endif


