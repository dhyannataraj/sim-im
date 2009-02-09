/***************************************************************************
                          sound.h  -  description
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

#ifndef _SOUND_H
#define _SOUND_H

#include "simapi.h"

#include <qobject.h>
#include <qthread.h>
#include <q3process.h>
//Added by qt3to4:
#include <Q3CString>

#include "cfg.h"
#include "event.h"
#include "plugins.h"

//#define USE_AUDIERE

#ifdef WIN32
#include <windows.h>

#else  // assume POSIX

#include <unistd.h>

#endif

#ifdef USE_AUDIERE
	#include <audiere.h>
	#include <iostream>
#endif

inline void sleepSecond() {
#ifdef WIN32
  Sleep(1000);
#else
  sleep(1000);
#endif
}

inline void sleepTime(int i) {
#ifdef WIN32
  Sleep(i);
#else
  sleep(i);
#endif
}
struct SoundData
{
#ifdef USE_KDE
    SIM::Data	UseArts;
#endif
    SIM::Data	Player;
    SIM::Data	StartUp;
    SIM::Data	FileDone;
    SIM::Data	MessageSent;
};

struct SoundUserData
{
    SIM::Data	Alert;
    SIM::Data	Receive;
    SIM::Data	NoSoundIfActive;
    SIM::Data	Disable;
};

class CorePlugin;
class QTimer;
class QSound;

class SoundPlugin
#ifdef WIN32
    : public QThread,
#else
    : public QObject,
#endif
      public SIM::Plugin, public SIM::EventReceiver

{
    Q_OBJECT
public:
    SoundPlugin(unsigned, bool, Buffer*);
    virtual ~SoundPlugin();

#ifdef USE_KDE
    PROP_BOOL(UseArts);
#endif
    PROP_STR(Player);
    PROP_STR(StartUp);
    PROP_STR(FileDone);
    PROP_STR(MessageSent);
    unsigned long CmdSoundDisable;
    SIM::SIMEvent EventSoundChanged;
protected slots:
    void checkSound();
    void childExited(int, int);
	void processExited();
	
protected:
    unsigned long user_data_id;
    virtual bool processEvent(SIM::Event *e);
    virtual Q3CString getConfig();
    virtual QWidget *createConfigWindow(QWidget *parent);
	virtual void run();
    QString fullName(const QString &name);
    QString messageSound(unsigned type, SoundUserData *data);
    void playSound(const QString &sound);
    void processQueue();
    QString         m_current;
    QStringList     m_queue;
    QSound         *m_sound;
    QTimer         *m_checkTimer;
	QString		    m_snd;
	Q3Process* m_process;

#if !defined( WIN32 ) && !defined( __OS2__ )
    long             m_player;
#endif
    SoundData	data;
    CorePlugin	*core;
    bool	    m_bChanged;
    bool bDone;
    bool destruct;
    bool isPlaying;
    friend class SoundConfig;
    friend class SoundUserConfig;
};

#endif

