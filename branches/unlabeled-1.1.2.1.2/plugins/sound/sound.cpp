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
#include "simapi.h"
#include "core.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#ifdef USE_KDE
#include <kaudioplayer.h>
#endif
#endif

#include "xpm/sound.xpm"

Plugin *createSoundPlugin(unsigned base, bool bFirst, const char *config)
{
    Plugin *plugin = new SoundPlugin(base, bFirst, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Sound"),
        I18N_NOOP("Plugin provides sounds on any evenents"),
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
#ifndef WIN32
#ifdef USE_KDE
    bool	UseArts;
#endif
    char	*Player;
#endif
    char	*StartUp;
    char	*FileDone;
} SoundData;
*/
static DataDef soundData[] =
    {
#ifndef WIN32
#ifdef USE_KDE
        { "UseArts", DATA_BOOL, 1, 1 },
#endif
        { "Player", DATA_STRING, 1, (unsigned)"play" },
#endif
        { "StartUp", DATA_STRING, 1, (unsigned)"startup.wav" },
        { "FileDone", DATA_STRING, 1, (unsigned)"filedone.wav" },
        { "MessageSent", DATA_STRING, 1, (unsigned)"msgsent.wav" },
        { "DisableAlert", DATA_BOOL, 1, 1 },
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
        { "Alert", DATA_STRING, 1, (unsigned)"alert.wav" },
        { "Receive", DATA_STRLIST, 1, 0 },
        { NULL, 0, 0, 0 }
    };

static SoundPlugin *soundPlugin = NULL;

static QWidget *getSoundSetup(QWidget *parent, void *data)
{
    return new SoundUserConfig(parent, data, soundPlugin);
}

SoundPlugin::SoundPlugin(unsigned base, bool bFirst, const char *config)
        : Plugin(base)
{
    load_data(soundData, &data, config);
    soundPlugin = this;
    if (bFirst)
        playSound(getStartUp());
    user_data_id = getContacts()->registerUserData(info.title, soundUserData);

    IconDef icon;
    icon.name = "sound";
    icon.xpm = sound;
    icon.isSystem = false;

    Event eIcon(EventAddIcon, &icon);
    eIcon.process();

    Command cmd;
    cmd->id		 = user_data_id + 1;
    cmd->text	 = I18N_NOOP("&Sound");
    cmd->icon	 = "sound";
    cmd->icon_on  = NULL;
    cmd->param	 = (void*)getSoundSetup;
    Event e(EventAddPreferences, cmd);
    e.process();

    Event ePlugin(EventGetPluginInfo, (void*)"_core");
    pluginInfo *info = (pluginInfo*)(ePlugin.process());
    core = static_cast<CorePlugin*>(info->plugin);
}

SoundPlugin::~SoundPlugin()
{
    soundPlugin = NULL;
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
    if (e->type() == EventContactOnline){
        Contact *contact = (Contact*)(e->param());
        SoundUserData *data = (SoundUserData*)(contact->getUserData(user_data_id));
        if (data && data->Alert && *data->Alert &&
                (!getDisableAlert() ||
                 (core &&
                  ((core->getManualStatus() == STATUS_ONLINE) ||
                   (core->getManualStatus() == STATUS_OFFLINE))))){
            Event eSound(EventPlaySound, data->Alert);
            eSound.process();
        }
        return NULL;
    }
    if (e->type() == EventMessageSent){
        Message *msg = (Message*)(e->param());
        if ((msg->getFlags() & MESSAGE_NOHISTORY) == 0){
            if ((msg->getFlags() & MESSAGE_MULTIPLY) && ((msg->getFlags() & MESSAGE_LAST) == 0))
                return NULL;
            const char *sound = getMessageSent();
            if (sound && *sound &&
                    (!getDisableAlert() || (core && (core->getManualStatus() == STATUS_ONLINE)))){
                Event eSound(EventPlaySound, (void*)sound);
                eSound.process();
            }
        }
        return NULL;
    }
    if (e->type() == EventMessageReceived){
        Message *msg = (Message*)(e->param());
		if (msg->type() == MessageStatus)
			return NULL;
        if (getDisableAlert() && core && (core->getManualStatus() != STATUS_ONLINE))
            return NULL;
        Contact *contact = getContacts()->contact(msg->contact());
        if (contact == NULL)
            return NULL;
        SoundUserData *data = (SoundUserData*)(contact->getUserData(user_data_id));
        string sound = messageSound(msg->type(), data);
        if (!sound.empty())
            playSound(sound.c_str());
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
    if (def){
        MessageDef *mdef = (MessageDef*)(def->param);
        if (mdef->base_type)
            type = mdef->base_type;
    }
    string sound;
    if (data)
        sound = get_str(data->Receive, type);
    if (sound == "-")
        return "";
    if (sound.empty()){
        def = core->messageTypes.find(type);
        if (def == NULL)
            return "";
        sound = def->icon;
        sound += ".wav";
        sound = fullName(sound.c_str());
    }
    return sound;
}

string SoundPlugin::fullName(const char *name)
{
    string sound;
    if ((name == NULL) || (*name == 0))
        return sound;
#ifdef WIN32
    char c = name[0];
    if (((((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) && (name[1] == ':')) ||
            ((c == '\\') && (name[1] == '\\'))){
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
    string sound = fullName(s);
#ifdef WIN32
    sndPlaySoundA(sound.c_str(), SND_ASYNC | SND_NODEFAULT);
#else
#ifdef USE_KDE
    if (getUseArts()){
        KAudioPlayer::play(sound.c_str());
        return;
    }
#endif
    ExecParam p;
    p.cmd = getPlayer();
    p.arg = sound.c_str();
    Event e(EventExec, &p);
    e.process();
#endif
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


