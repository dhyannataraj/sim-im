/***************************************************************************
                          msgcfg.cpp  -  description
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

#include "msgcfg.h"
#include "filecfg.h"
#include "smscfg.h"
#include "core.h"

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qmultilineedit.h>
#include <qradiobutton.h>
#include <qtabwidget.h>

MessageConfig::MessageConfig(QWidget *parent, void *_data)
        : MessageConfigBase(parent)
{
    m_file = NULL;
    for (QObject *p = parent; p != NULL; p = p->parent()){
        if (!p->inherits("QTabWidget"))
            continue;
        QTabWidget *tab = static_cast<QTabWidget*>(p);
        m_file = new FileConfig(tab, _data);
        tab->addTab(m_file, i18n("File"));
        tab->adjustSize();
        break;
    }

    CoreUserData *data = (CoreUserData*)_data;
    chkOnline->setChecked(data->OpenOnOnline.bValue);
    chkStatus->setChecked(data->LogStatus.bValue);
#ifndef WIN32
	btnMinimize->hide();
#endif
    switch (data->OpenNew.value){
    case NEW_MSG_NOOPEN:
        btnNoOpen->setChecked(true);
        break;
#ifdef WIN32
    case NEW_MSG_MINIMIZE:
        btnMinimize->setChecked(true);
        break;
#endif
    case NEW_MSG_RAISE:
        btnRaise->setChecked(true);
        break;
    }
}

void MessageConfig::apply(void *_data)
{
    if (m_file)
        m_file->apply(_data);

    CoreUserData *data = (CoreUserData*)_data;
    data->OpenOnOnline.bValue  = chkOnline->isChecked();
    data->LogStatus.bValue     = chkStatus->isChecked();
    data->OpenNew.value = NEW_MSG_NOOPEN;
#ifdef WIN32
    if (btnMinimize->isOn())
        data->OpenNew.value = NEW_MSG_MINIMIZE;
#endif
    if (btnRaise->isOn())
        data->OpenNew.value = NEW_MSG_RAISE;
}

void MessageConfig::setEnabled(bool state)
{
    if (m_file)
        m_file->setEnabled(state);
    MessageConfigBase::setEnabled(state);
}

#ifndef WIN32
#include "msgcfg.moc"
#endif

