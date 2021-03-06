/***************************************************************************
                          forwardcfg.cpp  -  description
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

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#include "misc.h"

#include "forwardcfg.h"
#include "forward.h"

using namespace SIM;

ForwardConfig::ForwardConfig(QWidget *parent, void *_data, ForwardPlugin *plugin)
        : ForwardConfigBase(parent)
{
    m_plugin = plugin;
    ForwardUserData *data = (ForwardUserData*)_data;
    chkFirst->setChecked(data->Send1st.toBool());
    chkTranslit->setChecked(data->Translit.toBool());
    cmbPhone->setEditable(true);
    QString phones = getContacts()->owner()->getPhones();
    while (!phones.isEmpty()){
        QString item = getToken(phones, ';', false);
        QString number = getToken(item, ',');
        getToken(item, ',');
        if (item.toULong() == CELLULAR)
            cmbPhone->insertItem(number);
    }
    cmbPhone->lineEdit()->setText(data->Phone.str());
}

void ForwardConfig::apply(void *_data)
{
    ForwardUserData *data = (ForwardUserData*)_data;
    data->Send1st.asBool() = chkFirst->isChecked();
    data->Translit.asBool() = chkTranslit->isChecked();
    data->Phone.str() = cmbPhone->lineEdit()->text();
}

void ForwardConfig::apply()
{
    apply(getContacts()->getUserData(m_plugin->user_data_id));
}

#ifndef NO_MOC_INCLUDES
#include "forwardcfg.moc"
#endif

