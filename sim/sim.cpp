/***************************************************************************
                          sim.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; e<ither version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "simapi.h"
#include "log.h"
#include "misc.h"
#include "profile/profilemanager.h"
#include "clients/clientmanager.h"
#include "simfs.h"
#include "paths.h"
#include "contacts/protocolmanager.h"
#include "events/eventhub.h"
#include "events/standardevent.h"
#include "events/logevent.h"
#include "events/menuitemcollectionevent.h"
#include "commands/commandhub.h"
#include "imagestorage/imagestorage.h"
#include "contacts/contactlist.h"
#include "builtinlogger.h"
#include "log.h"
#include "messaging/messageoutpipe.h"
#include "messaging/messagepipe.h"
#include "imagestorage/avatarstorage.h"

#include <QDir>

#include <QLibrary>
#include <QSettings>
#include <QMessageBox>

#ifdef WIN32
//#include <windows.h>
//#include <shlobj.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef USE_KDE
#include <QWidget>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kuniqueapplication.h>
#include "simapp/kdesimapp.h"
#else
#include "aboutdata.h"
#include "simapp/simapp.h"
#endif
#include <QApplication>

#if !defined(WIN32) && !defined(Q_OS_MAC) && !defined(__OS2__)
#include <X11/Xlib.h>
#endif

#include <QSettings>

using namespace SIM;

void simMessageOutput(QtMsgType, const char *msg)
{
    if (logEnabled())
        log(SIM::L_DEBUG, "QT: %s", msg);
}

#ifndef REVISION_NUMBER
	#define REVISION_NUMBER 
#endif

#ifdef CVS_BUILD
#define _VERSION	VERSION " SVN " __DATE__
#else
#define _VERSION	VERSION
#endif

void registerEvents()
{
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init_abort"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("quit"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("load_config"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("save_config"));
    SIM::getEventHub()->registerEvent(SIM::MenuItemCollectionEvent::create());
}

static BuiltinLogger* gs_logger = 0;
void initLogging()
{
    gs_logger = new BuiltinLogger(L_ERROR | L_WARN | L_DEBUG);
    SIM::getEventHub()->registerEvent(SIM::LogEvent::create());
    SIM::getEventHub()->getEvent("log")->connectTo(gs_logger, SLOT(logEvent(QString,int)));
}

void destroyLogging()
{
    delete gs_logger;
    gs_logger = 0;
}

int main(int argc, char *argv[])
{
    SimFileEngineHandler simfs;

    int res = 1;
	QCoreApplication::setOrganizationDomain("sim-im.org");
	QCoreApplication::setApplicationName("Sim-IM");
	SIM::createProfileManager(SIM::PathManager::configRoot());
    qInstallMsgHandler(simMessageOutput);

#ifdef USE_KDE
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options[] =
        {
            { 0, 0, 0 }
        };
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();
    if (!KUniqueApplication::start())
        exit(-1);
#endif

    SimApp app(argc, argv);
#ifdef Q_OS_MAC
    QString sPluginPath = app.applicationDirPath() + "/../";
#else
    QString sPluginPath = app.applicationDirPath() + "/plugins";
	QString sPluginPath2 = app.applicationDirPath();
	QApplication::addLibraryPath(sPluginPath2);
#endif
    QApplication::addLibraryPath(sPluginPath);

    SIM::createEventHub();
    registerEvents();
    initLogging();
    SIM::createMessagePipe();
    SIM::createOutMessagePipe();
    SIM::createImageStorage();
    SIM::createAvatarStorage();
    SIM::createCommandHub();
    SIM::createContactList();
    SIM::createPluginManager(argc, argv);
    SIM::createClientManager(app.services()->protocolManager());

    if(!app.initializePlugins())
        return -1;

    res = app.exec();

    SIM::getClientManager()->sync();
    SIM::destroyClientManager();
    SIM::destroyPluginManager();
    
    SIM::destroyContactList();
    SIM::destroyCommandHub();
    SIM::destroyAvatarStorage();
    SIM::destroyImageStorage();
    SIM::destroyProfileManager();
    SIM::destroyOutMessagePipe();
    SIM::destroyMessagePipe();
    destroyLogging();
    SIM::destroyEventHub();
  
    return res;
}

// vim: set expandtab:

