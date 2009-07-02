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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "simapi.h"
#include "log.h"
#include "misc.h"
#include "profilemanager.h"
#include "simfs.h"

#include <QDir>

#include <QLibrary>
#include <QSettings>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef USE_KDE
#include <qwidget.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kuniqueapplication.h>
#else
#include "aboutdata.h"
#endif
#include <QApplication>

#if !defined(WIN32) && !defined(QT_MACOSX_VERSION) && !defined(QT_MAC) && !defined(__OS2__)
//#include <X11/X.h>
#include <X11/Xlib.h>
#endif

#include <QSettings>

using namespace SIM;

#ifdef USE_KDE
class SimApp : public KUniqueApplication
{
public:
    SimApp();
    ~SimApp();
    int newInstance();
    void commitData(QSessionManager&);
    void saveState(QSessionManager&);
protected:
    bool firstInstance;
};

SimApp::SimApp() : KUniqueApplication()
{
    firstInstance = true;
}

SimApp::~SimApp()
{
}

int SimApp::newInstance()
{
    if (firstInstance)
	{
        firstInstance = false;
    }
	else
	{
        QWidgetList  *list = QApplication::topLevelWidgets();
        QWidgetListIt it( *list );
        QWidget *w;
        while((w = it.current()) != 0 )
		{
            ++it;
            if (w->inherits("MainWindow")){
                raiseWindow(w);
            }
        }
        delete list;
    }
    return 0;
}

#else

class SimApp : public QApplication
{
public:
    SimApp(int &argc, char **argv)
      : QApplication(argc, argv)
    {}
    ~SimApp()
    {}
protected:
    void commitData(QSessionManager&);
    void saveState(QSessionManager&);
};

#endif

void SimApp::commitData(QSessionManager&)
{
    save_state();
}

void SimApp::saveState(QSessionManager &sm)
{
    QApplication::saveState(sm);
}

void simMessageOutput( QtMsgType, const char *msg )
{
    if (logEnabled())
        log(L_DEBUG, "QT: %s", msg);
}

#ifndef WIN32

static const char *qt_args[] =
    {
#ifdef USE_KDE
        "caption:",
        "icon:",
        "miniicon:",
        "config:",
        "dcopserver:",
        "nocrashhandler",
        "waitforwm",
        "style:",
        "geometry:",
        "smkey:",
        "nofork",
        "help",
        "help-kde",
        "help-qt",
        "help-all",
        "author",
        "version",
        "license",
#endif
        "display:",
        "session:",
        "cmap"
        "ncols:",
        "nograb",
        "dograb",
        "sync",
        "fn",
        "font:",
        "bg",
        "background:",
        "fg",
        "foreground:",
        "btn",
        "button:",
        "name:",
        "title:",
        "reverse",
        "screen:",
        NULL
    };

#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC) && !defined(__OS2__)
extern "C" {
    static int (*old_errhandler)(Display*, XErrorEvent*) = NULL;
    static int x_errhandler( Display *dpy, XErrorEvent *err )
    {
        if (err->error_code == BadMatch)
            return 0;
        if (old_errhandler)
            return old_errhandler(dpy, err);
        return 0;
    }
}
#endif
#endif

#ifndef REVISION_NUMBER
	#define REVISION_NUMBER 
#endif

#ifdef CVS_BUILD
#define _VERSION	VERSION " SVN " __DATE__
#else
#define _VERSION	VERSION
#endif

#ifdef WIN32
#ifdef _DEBUG

class Debug
{
public:
    Debug()		{}
    ~Debug()	{} //causes crash on close in win32 by noragen
};

Debug d;

#endif

static BOOL (WINAPI *_SHGetSpecialFolderPathA)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate) = NULL;
static BOOL (WINAPI *_SHGetSpecialFolderPathW)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate) = NULL;
#endif

QString getConfigRootPath()
{
    QString s;
#ifndef WIN32
# ifdef USE_KDE4
    char *kdehome = getenv("KDEHOME");
    if (kdehome){
        s = kdehome;
    }else{
        s += ".kde/";
    }
    if (!s.endsWith("/"))
        s += '/';
    s += "share/apps/sim";
# elif defined(__OS2__)
    char *os2home = getenv("HOME");
    if (os2home) {
        s = os2home;
        s += "\\";
    }
    s += ".sim-qt4";
    if ( access( s, F_OK ) != 0 ) {
        mkdir( s, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    }
# else
    s = QDir::homePath() + "/.sim-qt4";
# endif
#else
    char szPath[1024];
    szPath[0] = 0;
    QString defPath;
	
	//Fixme:
	//FOLDERID_RoamingAppData <<== this is used in Vista.. should be fixed
	//otherwise the config is stored in "Downloads" per default :-/
	//Windows 2008 Server tested, simply works...
    
	(DWORD&)_SHGetSpecialFolderPathW   = (DWORD)QLibrary::resolve("Shell32.dll","SHGetSpecialFolderPathW");
    (DWORD&)_SHGetSpecialFolderPathA   = (DWORD)QLibrary::resolve("Shell32.dll","SHGetSpecialFolderPathA");

    if (_SHGetSpecialFolderPathW && _SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, true)){
        defPath = QString::fromUtf16((unsigned short*)szPath);
    }else if (_SHGetSpecialFolderPathA && _SHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, true)){
        defPath = QFile::decodeName(szPath);
	}

	if (!defPath.isEmpty()){
        if (!defPath.endsWith("\\"))
            defPath += '\\';
        defPath += "sim";
        makedir(defPath + '\\');
        QString lockTest = defPath + "\\.lock";
        QFile f(lockTest);
        if (!f.open(IO_ReadWrite | IO_Truncate))
            defPath = "";
        f.close();
        QFile::remove(lockTest);
    }
    if (!defPath.isEmpty()){
        s = defPath;
    }else{
        s = app_file("");
    }
#endif
    QDir().mkpath(s);
#ifdef HAVE_CHMOD
    chmod(QFile::encodeName(s), 0700);
#endif
    return QDir::convertSeparators(s);
}

int main(int argc, char *argv[])
{
    SimFileEngineHandler simfs;

    int res = 1;
	QCoreApplication::setOrganizationDomain("sim-im.org");
	QCoreApplication::setApplicationName("Sim-IM");
	new SIM::ProfileManager(getConfigRootPath());
#ifdef WIN32
    Qt::HANDLE hMutex = CreateMutexA(NULL, FALSE, "SIM_Mutex");
#elif defined(__OS2__)    
    HMTX hMutex = NULLHANDLE;
    if ( DosCreateMutexSem("\\SEM32\\SIM_Mutex", &hMutex, 0, FALSE) != 0 ) {
        // prevent running another instance
        return 1;
    }
#endif
    qInstallMsgHandler(simMessageOutput);

    KAboutData aboutData(PACKAGE,
                         I18N_NOOP("Sim-IM"),
                         _VERSION,
                         I18N_NOOP("Multiprotocol Instant Messenger"),
                         KAboutData::License_GPL,
                         "Copyright (C) 2002-2004, Vladimir Shutoff\n"
                         "2005-2009, Sim-IM Development Team",
                         0,
                         "http://sim-im.org/",
                         "https://mailman.dg.net.ua/listinfo/sim-im-main");
    aboutData.addAuthor("Sim-IM Development Team",I18N_NOOP("Current development"),	"sim-im-main@lists.sim-im.org",	"http://sim-im.org/");
    aboutData.addAuthor("Vladimir Shutoff"		 ,I18N_NOOP("Original Author"),		"vovan@shutoff.ru");
    aboutData.addAuthor("Christian Ehrlicher"	 ,I18N_NOOP("Developer"),			"Ch.Ehrlicher@gmx.de");
    setAboutData(&aboutData);

#ifndef WIN32
    int _argc = 0;
    char **_argv = new char*[argc + 1];
    _argv[_argc++] = argv[0];
    char **to = argv + 1;
    // check all parameters and sort them
    // _argc/v: parameter for KUnqiueApplication
    //  argc/v: plugin parameter
    for (char **p = argv + 1; *p; ++p){
        char *arg = *p;
        // check if "-" or "--"
        if (arg[0] != '-') {
            *(to++) = *p;
            continue;
        }
        arg++;
        if (arg[0] == '-')
            arg++;
        // if they are parameters with variable params we need
        // to skip the next param
        bool bSkip = false;
        const char **q;
        // check for qt or kde - parameters
        for (q = qt_args; *q; ++q){
            unsigned len = strlen(*q);
            bSkip = false;
            // variable parameter?
            if ((*q)[len-1] == ':'){
                len--;
                bSkip = true;
            }
            // copy them for KUnqiueApplication-args
            if ((strlen(arg) == len) && !memcmp(arg, *q, len))
                break;
        }
        // dunno know what to do here
        if (*q){
            _argv[_argc++] = *p;
            argc--;
            if (bSkip){
                ++p;
                if (*p == NULL) break;
                _argv[_argc++] = *p;
                argc--;
            }
        }else{
            *(to++) = *p;
        }
    }
    *to = NULL;
    _argv[_argc] = NULL;
#ifdef USE_KDE
    KCmdLineArgs::init( _argc, _argv, &aboutData );
    KCmdLineOptions options[] =
        {
            { 0, 0, 0 }
        };
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();
    if (!KUniqueApplication::start())
        exit(-1);
    SimApp app;
#else
    SimApp app(_argc, _argv);
#endif
#if !defined(QT_MACOSX_VERSION) && !defined(Q_OS_MAC) && !defined(__OS2__)
    old_errhandler = XSetErrorHandler(x_errhandler);
#endif
#else
    for (int i = 0; i < argc; i++){
        QByteArray arg = argv[i];
        if ((arg[0] == '/') || (arg[0] == '-'))
            arg = arg.mid(1);
        if ((arg == "reinstall") || (arg == "showicons") || (arg == "hideicons"))
            return 0;
    }
    SimApp app(argc, argv);
#endif
    QApplication::addLibraryPath( app.applicationDirPath() + "/plugins" );
    PluginManager p(argc, argv);
    app.setQuitOnLastWindowClosed( false );
    if (p.isLoaded())
        res = app.exec();
#ifdef WIN32
    CloseHandle(hMutex);
#elif defined(__OS2__)    
    DosCloseMutexSem(hMutex);
#endif
	return res;
}
