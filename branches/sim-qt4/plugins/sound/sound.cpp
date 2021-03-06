/***************************************************************************
                          sound.cpp  -  description
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

#include "sound.h"
#include "soundconfig.h"
#include "sounduser.h"
#include "core.h"
#include "exec.h"

#include <QFile>
#include <qsound.h>
#include <QTimer>

#ifdef USE_KDE
#include <kaudioplayer.h>
#endif

const unsigned CHECK_SOUND_TIMEOUT	= 200;
const unsigned WAIT_SOUND_TIMEOUT	= 1000;

Plugin *createSoundPlugin(unsigned base, bool bFirst, Buffer *config)
{
    Plugin *plugin = new SoundPlugin(base, bFirst, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Sound"),
        I18N_NOOP("Plugin provides sounds on any events"),
        VERSION,
        createSoundPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

/*
typedef struct SoundData
{
#ifdef USE_KDE
    bool	UseArts;
#endif
    char	*Player;
    char	*StartUp;
    char	*FileDone;
} SoundData;
*/
static DataDef soundData[] =
    {
#ifdef USE_KDE
        { "UseArts", DATA_BOOL, 1, DATA(1) },
#endif
        { "Player", DATA_STRING, 1, "play" },
        { "StartUp", DATA_STRING, 1, "startup.wav" },
        { "FileDone", DATA_STRING, 1, "filedone.wav" },
        { "MessageSent", DATA_STRING, 1, "msgsent.wav" },
        { NULL, 0, 0, 0 }
    };

/*
typedef struct SoundUserData
{
    char	*Alert;
	void	*Receive;
} SoundUserData;
*/
static DataDef soundUserData[] =
    {
        { "Alert", DATA_STRING, 1, "alert.wav" },
        { "Receive", DATA_STRLIST, 1, 0 },
        { "NoSoundIfActive", DATA_BOOL, 1, 0 },
        { "Disable", DATA_BOOL, 1, 0 },
        { NULL, 0, 0, 0 }
    };

static SoundPlugin *soundPlugin = NULL;

static QWidget *getSoundSetup(QWidget *parent, void *data)
{
    return new SoundUserConfig(parent, data, soundPlugin);
}

SoundPlugin::SoundPlugin(unsigned base, bool bFirst, Buffer *config)
        : Plugin(base)
{
    load_data(soundData, &data, config);
    soundPlugin = this;
    user_data_id = getContacts()->registerUserData(info.title, soundUserData);

    m_bChanged = false;

    CmdSoundDisable   = registerType();
    EventSoundChanged = registerType();

    Command cmd;
    cmd->id		 = user_data_id + 1;
    cmd->text	 = I18N_NOOP("&Sound");
    cmd->icon	 = "sound";
    cmd->icon_on  = NULL;
    cmd->param	 = (void*)getSoundSetup;
    Event e(EventAddPreferences, cmd);
    e.process();

    cmd->id		  = CmdSoundDisable;
    cmd->text	  = I18N_NOOP("&Sound");
    cmd->icon	  = "nosound";
    cmd->icon_on  = "sound";
    cmd->bar_id   = ToolBarMain;
    cmd->bar_grp  = 0;
    cmd->menu_id  = 0;
    cmd->menu_grp = 0;
    cmd->flags	  = COMMAND_CHECK_STATE;
    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

    cmd->icon	  = NULL;
    cmd->icon_on  = NULL;
    cmd->bar_id   = 0;
    cmd->menu_id  = MenuMain;
    cmd->flags	  = COMMAND_CHECK_STATE;
    eCmd.process();

    Event ePlugin(EventGetPluginInfo, (void*)"_core");
    pluginInfo *info = (pluginInfo*)(ePlugin.process());
    core = static_cast<CorePlugin*>(info->plugin);

    m_sound	 = NULL;
#ifndef WIN32
    m_player = 0;
    connect(ExecManager::manager, SIGNAL(childExited(int,int)), this, SLOT(childExited(int,int)));
#endif
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(checkSound()));
    if (bFirst)
        playSound(getStartUp());
}

SoundPlugin::~SoundPlugin()
{
    if (m_sound)
        delete m_sound;
    soundPlugin = NULL;
    Event eCmd(EventCommandRemove, (void*)CmdSoundDisable);
    eCmd.process();
    Event e(EventRemovePreferences, (void*)user_data_id);
    e.process();
    free_data(soundData, &data);
    getContacts()->unregisterUserData(user_data_id);
}

string SoundPlugin::getConfig()
{
    return save_data(soundData, &data);
}

QWidget *SoundPlugin::createConfigWindow(QWidget *parent)
{
    return new SoundConfig(parent, this);
}

void *SoundPlugin::processEvent(Event *e)
{
    if (e->type() == EventSoundChanged){
        Command cmd;
        cmd->id    = CmdSoundDisable;
        SoundUserData *data = (SoundUserData*)(getContacts()->getUserData(user_data_id));
        if (!data->Disable.bValue)
            cmd->flags |= COMMAND_CHECKED;
        m_bChanged = true;
        Event e(EventCommandChecked, cmd);
        e.process();
        m_bChanged = false;
        return NULL;
    }
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdSoundDisable){
            cmd->flags &= ~COMMAND_CHECKED;
            SoundUserData *data = (SoundUserData*)(getContacts()->getUserData(user_data_id));
            if (!data->Disable.bValue)
                cmd->flags |= COMMAND_CHECKED;
            return e->param();
        }
        return NULL;
    }
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (!m_bChanged && (cmd->id == CmdSoundDisable)){
            SoundUserData *data = (SoundUserData*)(getContacts()->getUserData(user_data_id));
            data->Disable.bValue = !data->Disable.bValue;
            Event eChanged(EventSoundChanged);
            eChanged.process();
            return e->param();
        }
        return NULL;
    }
    if (e->type() == EventContactOnline){
        Contact *contact = (Contact*)(e->param());
        SoundUserData *data = (SoundUserData*)(contact->getUserData(user_data_id));
        if (data && data->Alert.ptr && *data->Alert.ptr && !data->Disable.bValue){
            Event eSound(EventPlaySound, data->Alert.ptr);
            eSound.process();
        }
        return NULL;
    }
    if (e->type() == EventMessageSent){
        Message *msg = (Message*)(e->param());
        const char *err = msg->getError();
        if (err && *err)
            return NULL;
        const char *sound = NULL;
        if (msg->type() == MessageFile){
            sound = getFileDone();
        }else if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
            if ((msg->getFlags() & MESSAGE_MULTIPLY) && ((msg->getFlags() & MESSAGE_LAST) == 0))
                return NULL;
            sound = getMessageSent();
        }
        if (sound && *sound){
            Event eSound(EventPlaySound, (void*)sound);
            eSound.process();
        }
        return NULL;
    }
    if (e->type() == EventMessageReceived){
        Message *msg = (Message*)(e->param());
        if (msg->type() == MessageStatus)
            return NULL;
        if (msg->getFlags() & MESSAGE_LIST)
            return NULL;
        Contact *contact = getContacts()->contact(msg->contact());
        SoundUserData *data;
        if (contact){
            data = (SoundUserData*)(contact->getUserData(user_data_id));
        }else{
            data = (SoundUserData*)(getContacts()->getUserData(user_data_id));
        }
        bool bEnable = !data->Disable.bValue;
        if (bEnable && data->NoSoundIfActive.bValue){
            Event e(EventActiveContact);
            if ((unsigned)(e.process()) == contact->id())
                bEnable = false;
        }
        if (bEnable){
            string sound = messageSound(msg->baseType(), data);
            if (!sound.empty())
                playSound(sound.c_str());
        }
        return NULL;
    }
    if (e->type() == EventPlaySound){
        char *name = (char*)(e->param());
        playSound(name);
        return e->param();
    }
    return NULL;
}

string SoundPlugin::messageSound(unsigned type, SoundUserData *data)
{
    CommandDef *def = core->messageTypes.find(type);
    string sound;
    if (data)
        sound = get_str(data->Receive, type);
    if (sound == "(nosound)")
        return "";
    if (sound.empty()){
        def = core->messageTypes.find(type);
        if ((def == NULL) || (def->icon == NULL))
            return "";
        MessageDef *mdef = (MessageDef*)(def->param);
        if (mdef->flags & MESSAGE_SYSTEM){
            sound = "system";
        }else if (mdef->flags & MESSAGE_ERROR){
            sound = "error";
        }else{
            sound = def->icon;
        }
        sound += ".wav";
        sound = fullName(sound.c_str());
    }
    return sound;
}

string SoundPlugin::fullName(const char *name)
{
    string sound="";
    string str_name = name;
    if ((name == NULL) || (*name == 0) || (str_name == "(nosound)"))
        return sound;
#ifdef WIN32
    char c = name[0];
    if (((((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) && (name[1] == ':')) || ((c == '\\') && (name[1] == '\\'))){
#else
    if (name[0] == '/'){
#endif
        sound = name;
    }else{
        sound = "sounds/";
        sound += name;
        sound = app_file(sound.c_str());
    }
    return sound;
}

void SoundPlugin::playSound(const char *s)
{
    if ((s == NULL) || (*s == 0))
        return;
    if (m_current == s)
        return;
    for (list<string>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
        if ((*it) == s)
            return;
    }
    m_queue.push_back(s);
    if (m_sound == NULL)
        processQueue();
}

void SoundPlugin::processQueue()
{
    if (!m_current.empty() || m_queue.empty())
        return;
    m_current = m_queue.front();
    m_queue.erase(m_queue.begin());
    string sound = fullName(m_current.c_str());
    // check whether file is available
    if (!QFile::exists(QString(sound.c_str()))) {
        m_current="";
        return;
    }
#ifdef USE_KDE
    if (getUseArts()){
        KAudioPlayer::play(sound.c_str());
        m_checkTimer->start(WAIT_SOUND_TIMEOUT);
        m_current = "";
        return; // arts
    }
    bool bSound = false;
#elif WIN32
    bool bSound = true;
#else
    /* If there is an external player selected, don't use Qt
    Check first for getPlayer() since QSound::available()
    can take 5 seconds to return a value */
    bool bSound = !getPlayer() && QSound::available();
#endif
    if (bSound){
        if (!QSound::available()){
            m_queue.clear();
            m_current = "";
            return;
        }
        if (m_sound)
            delete m_sound;
        m_sound   = NULL;
        m_sound = new QSound(sound.c_str());
        m_sound->play();
#if COMPAT_QT_VERSION >= 0x030000
        m_checkTimer->start(CHECK_SOUND_TIMEOUT);
#else
        m_checkTimer->start(WAIT_SOUND_TIMEOUT);
#endif
        m_current = "";
        return; // QSound
    }
#ifndef WIN32
    ExecParam p;
    p.cmd = getPlayer();
    if (*p.cmd == 0) {
        m_current="";
        return;
    }
    p.arg = sound.c_str();
    Event e(EventExec, &p);
    m_player = (int)e.process();
    if (m_player == 0){
        log(L_WARN, "Can't execute player");
        m_queue.clear();
    }
    m_current = "";
    return; // external Player
#endif
}

void SoundPlugin::checkSound()
{
    bool bDone = true;
#if COMPAT_QT_VERSION >= 0x030000
    if (m_sound && !m_sound->isFinished())
        bDone = false;
#endif
    if (bDone){
        m_checkTimer->stop();
        if (m_sound)
            delete m_sound;
        m_sound   = NULL;
        m_current = "";
        processQueue();
    }
}

#ifdef WIN32

void SoundPlugin::childExited(int, int)
{
}

#else

void SoundPlugin::childExited(int pid, int)
{
    if (pid == m_player){
        m_player = 0;
        m_current = "";
        processQueue();
    }
}

#endif

#ifdef WIN32
#include <windows.h>

/**
 * DLL's entry point
 **/
int WINAPI DllMain(HINSTANCE, DWORD, LPVOID)
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
#include "sound.moc"
#endif



