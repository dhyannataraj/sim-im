/***************************************************************************
                          homedir.cpp  -  description
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

#include "simapi.h"

#include "homedir.h"

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>

#include <qlibrary.h>
#include <qsettings.h>

#include "homedircfg.h"

static BOOL (WINAPI *_SHGetSpecialFolderPathA)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate) = NULL;
static BOOL (WINAPI *_SHGetSpecialFolderPathW)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate) = NULL;

#else
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#endif

#include <qdir.h>
#include <qfile.h>

#include "log.h"
#include "misc.h"

using namespace std;
using namespace SIM;

Plugin *createHomeDirPlugin(unsigned base, bool, Buffer*)
{
    Plugin *plugin = new HomeDirPlugin(base);
    return plugin;
}

static PluginInfo info =
    {
#ifdef WIN32
        I18N_NOOP("Home directory"),
        I18N_NOOP("Plugin provides select directory for store config files"),
#else
        NULL,
        NULL,
#endif
        VERSION,
        createHomeDirPlugin,
        PLUGIN_NO_CONFIG_PATH | PLUGIN_NODISABLE
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

#ifdef WIN32

static char key_name[] = "/SIM";
static char path_value[] = "Path";

#endif

HomeDirPlugin::HomeDirPlugin(unsigned base)
        : Plugin(base)
{
#ifdef WIN32
    m_bSave    = true;
    QSettings setting( QSettings::Native );
    setting.setPath( key_name, "", QSettings::User );
    m_homeDir = setting.readEntry( path_value );
    m_bDefault = m_homeDir.isNull();
#endif
    QString d;
    EventArg e("-b:", I18N_NOOP("Set home directory"));
    if (e.process() && !e.value().isEmpty()){
        d = e.value();
#ifdef WIN32
        m_bSave   = false;
#endif
    } else {
        d = m_homeDir;
    }
    QDir dir( d );
    if ( d.isEmpty() || !dir.exists() ) {
        m_homeDir = defaultPath();
#ifdef WIN32
        m_bDefault = true;
        m_bSave   = false;
#endif
    }
}

QString HomeDirPlugin::defaultPath()
{
    QString s;
#ifndef WIN32
    struct passwd *pwd = getpwuid(getuid());
    if (pwd){
        s = QFile::decodeName(pwd->pw_dir);
    }else{
        log(L_ERROR, "Can't get pwd");
    }
    if (!s.endsWith("/"))
        s += '/';
#ifdef USE_KDE
    char *kdehome = getenv("KDEHOME");
    if (kdehome){
        s = kdehome;
    }else{
        s += ".kde/";
    }
    if (!s.endsWith("/"))
        s += '/';
    s += "share/apps/sim";
#else
    
#ifdef __OS2__
    char *os2home = getenv("HOME");
    if (os2home) {
        s = os2home;
        s += "\\";
    }
    s += ".sim";
    if ( access( s, F_OK ) != 0 ) {
    	mkdir( s, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    }
#else
    s += ".sim";
#endif

#endif
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
	//(DWORD&)_SHGetKnownFolderPath	   = (DWORD)QLibrary::resolve("Shell32.dll","SHGetKnownFolderPath"); //for Vista :-/

    if (_SHGetSpecialFolderPathW && _SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, true)){
        defPath = QString::fromUcs2((unsigned short*)szPath);
    }else if (_SHGetSpecialFolderPathA && _SHGetSpecialFolderPathA(NULL, szPath, CSIDL_APPDATA, true)){
        defPath = QFile::decodeName(szPath);
	}
    //}else if (_SHGetKnownFolderPath && _SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0x00008000, NULL, szPath)){
	//	defPath = QFile::decodeName(szPath);

	/*HRESULT SHGetKnownFolderPath(          REFKNOWNFOLDERID rfid,
    DWORD dwFlags,
    HANDLE hToken,
    PWSTR *ppszPath );*/

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
#ifdef HAVE_CHMOD
    chmod(QFile::encodeName(s), 0700);
#endif
    return QDir::convertSeparators(s);
}

#ifdef WIN32

QWidget *HomeDirPlugin::createConfigWindow(QWidget *parent)
{
    return new HomeDirConfig(parent, this);
}

QCString HomeDirPlugin::getConfig()
{
    if (!m_bSave)
        return "";
    QSettings setting( QSettings::Native );
    setting.setPath( key_name, "", QSettings::User );

    if (!m_bDefault){
        setting.writeEntry( path_value, m_homeDir );
    }else{
        setting.removeEntry( path_value );
    }
    return "";
}

#endif

QString HomeDirPlugin::buildFileName(const QString &name)
{
    QString s;
    QString fname = name;
    if(QDir(fname).isRelative()) {
        s += m_homeDir;
        s += '/';
    }
    s += fname;
    return QDir::convertSeparators(s);
}

bool HomeDirPlugin::processEvent(Event *e)
{
    if (e->type() == eEventHomeDir){
        EventHomeDir *homedir = static_cast<EventHomeDir*>(e);
        homedir->setHomeDir(buildFileName(homedir->homeDir()));
        return true;
    }
    return false;
}
