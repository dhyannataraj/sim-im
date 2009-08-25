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

#include <QCheckBox>
#include <QRadioButton>
#include <QTabWidget>

MessageConfig::MessageConfig(QWidget *parent, void *_data)
  : QWidget(parent)
{
    setupUi(this);
    m_file = NULL;
    for (QObject *p = parent; p != NULL; p = p->parent()){
        QTabWidget *tab = qobject_cast<QTabWidget*>(p);
        if(!tab)
            continue;
        m_file = new FileConfig(tab, _data);
        tab->addTab(m_file, i18n("File"));
        tab->adjustSize();
        break;
    }

    CoreUserData *data = (CoreUserData*)_data;
    chkOnline->setChecked(data->OpenOnOnline.toBool());
    chkStatus->setChecked(data->LogStatus.toBool());
    switch (data->OpenNewMessage.toULong()){
    case NEW_MSG_NOOPEN:
        btnNoOpen->setChecked(true);
        break;
    case NEW_MSG_MINIMIZE:
        btnMinimize->setChecked(true);
        break;
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
    data->OpenOnOnline.asBool()    = chkOnline->isChecked();
    data->LogStatus.asBool()       = chkStatus->isChecked();
    data->OpenNewMessage.asULong() = NEW_MSG_NOOPEN;
    if (btnMinimize->isChecked())
        data->OpenNewMessage.asULong() = NEW_MSG_MINIMIZE;
    if (btnRaise->isChecked())
        data->OpenNewMessage.asULong() = NEW_MSG_RAISE;
}
