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

#include "forwardcfg.h"
#include "forward.h"

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

ForwardConfig::ForwardConfig(QWidget *parent, void *_data, ForwardPlugin *plugin)
        : ForwardConfigBase(parent)
{
    m_plugin = plugin;
    ForwardUserData *data = (ForwardUserData*)_data;
    chkFirst->setChecked(data->Send1st != 0);
    chkTranslit->setChecked(data->Translit != 0);
    cmbPhone->setEditable(true);
    QString phones = getContacts()->owner()->getPhones();
    while (!phones.isEmpty()){
        QString item = getToken(phones, ';', false);
        QString number = getToken(item, ',');
        getToken(item, ',');
        if ((unsigned)atol(item.latin1()) == CELLULAR)
            cmbPhone->insertItem(number);
    }
    if (data->Phone)
        cmbPhone->lineEdit()->setText(QString::fromUtf8(data->Phone));
}

void ForwardConfig::apply(void *_data)
{
    ForwardUserData *data = (ForwardUserData*)_data;
    data->Send1st = chkFirst->isChecked();
    data->Translit = chkTranslit->isChecked();
    set_str(&data->Phone, cmbPhone->lineEdit()->text().utf8());
}

void ForwardConfig::apply()
{
    apply(getContacts()->getUserData(m_plugin->user_data_id));
}

#ifndef WIN32
#include "forwardcfg.moc"
#endif

