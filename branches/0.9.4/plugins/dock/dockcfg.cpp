/***************************************************************************
                          dockcfg.cpp  -  description
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

#include "dockcfg.h"
#include "dock.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#ifdef USE_KDE
#include <kwin.h>
#endif

DockCfg::DockCfg(QWidget *parent, DockPlugin *plugin)
        : DockCfgBase(parent)
{
    m_plugin = plugin;
    chkAutoHide->setChecked(plugin->getAutoHide());
    unsigned interval = plugin->getAutoHideInterval();
    spnAutoHide->setValue(interval);
    connect(chkAutoHide, SIGNAL(toggled(bool)), this, SLOT(autoHideToggled(bool)));
    connect(btnCustomize, SIGNAL(clicked()), this, SLOT(customize()));
    autoHideToggled(plugin->getAutoHide());
#ifdef USE_KDE
    spn_desk->setMaxValue(KWin::numberOfDesktops());
    spn_desk->setValue(m_plugin->getDesktop());
#else
    spn_desk->hide();
    TextLabel1_2->hide();
#endif
}

void DockCfg::apply()
{
    m_plugin->setAutoHide(chkAutoHide->isChecked());
    m_plugin->setAutoHideInterval(atol(spnAutoHide->text().latin1()));
#ifdef USE_KDE
    m_plugin->setDesktop(atol(spn_desk->text().latin1()));
#endif
}

void DockCfg::autoHideToggled(bool bAutoHide)
{
    spnAutoHide->setEnabled(bAutoHide);
}

void DockCfg::customize()
{
    Event e(EventMenuCustomize, (void*)(m_plugin->DockMenu));
    e.process();
}

#ifndef NO_MOC_INCLUDES
#include "dockcfg.moc"
#endif

