/***************************************************************************
                          soundconfig.cpp  -  description
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

#include <qcheckbox.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qsound.h>

#include "editfile.h"
#include "contacts.h"
#include "misc.h"

#include "soundconfig.h"
#include "sounduser.h"
#include "sound.h"

using SIM::getContacts;

SoundConfig::SoundConfig(QWidget *parent, SoundPlugin *plugin) : QWidget(parent)
        //: SoundConfigBase(parent)
{
	setupUi(this);
    m_plugin = plugin;
    user_cfg = NULL;
#ifdef USE_KDE
    bool bSound = false;
    connect(chkArts, SIGNAL(toggled(bool)), this, SLOT(artsToggled(bool)));
    chkArts->setChecked(plugin->getUseArts());
#else
    chkArts->hide();
#ifndef WIN32
    bool bSound = QSound::available();
#endif
#endif

#if defined( WIN32 ) || defined( __OS2__ )
    lblPlayer->hide();
    edtPlayer->hide();
#else
    if (bSound){
        lblPlayer->setText(i18n("Qt provides sound output so you just need to set a player if you don't like Qt's sound."));
    }
    edtPlayer->setText(plugin->getPlayer());
#endif
    edtStartup->setText(plugin->fullName(plugin->getStartUp()));
    edtFileDone->setText(plugin->fullName(plugin->getFileDone()));
    edtSent->setText(plugin->fullName(plugin->getMessageSent()));

    for (QObject *p = parent; p != NULL; p = p->parent()){
        if (!p->inherits("QTabWidget"))
            continue;
        QTabWidget *tab = static_cast<QTabWidget*>(p);
        void *data = getContacts()->getUserData(plugin->user_data_id);
        user_cfg = new SoundUserConfig(tab, data, plugin);
        tab->addTab(user_cfg, i18n("Events"));
        tab->adjustSize();
        break;
    }
}

SoundConfig::~SoundConfig()
{
    if (user_cfg)
        delete user_cfg;
}

void SoundConfig::apply()
{
    if (user_cfg){
        void *data = getContacts()->getUserData(m_plugin->user_data_id);
        user_cfg->apply(data);
    }
#ifdef USE_KDE
    m_plugin->setUseArts(chkArts->isChecked());
    bool bSound = false;
#else
    /* If there is an external player selected, don't use Qt
    Check first for edtPlayer->text().isEmpty() since QSound::available()
    can take 5 seconds to return a value */
    bool bSound = edtPlayer->text().isEmpty() && QSound::available();
#endif
    if (bSound)
        m_plugin->setPlayer("");
    else
        m_plugin->setPlayer(edtPlayer->text());
    m_plugin->setStartUp(sound(edtStartup->text(), "startup.wav"));
    m_plugin->setFileDone(sound(edtFileDone->text(), "startup.wav"));
    m_plugin->setMessageSent(sound(edtSent->text(), "startup.wav"));
}

QString SoundConfig::sound(const QString &text, const QString &def)
{
    QString defFile = m_plugin->fullName(def);
    if (defFile == text)
        return def;
    return text;
}

void SoundConfig::artsToggled(bool)
{
    lblPlayer->setEnabled(!chkArts->isChecked());
    edtPlayer->setEnabled(!chkArts->isChecked());
}

