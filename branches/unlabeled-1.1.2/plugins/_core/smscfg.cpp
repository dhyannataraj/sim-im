/***************************************************************************
                          smscfg.cpp  -  description
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

#include "smscfg.h"
#include "core.h"

#include <qmultilineedit.h>
#include <qcheckbox.h>

SMSConfig::SMSConfig(QWidget *parent, void *_data)
        : SMSConfigBase(parent)
{
    SMSUserData *data = (SMSUserData*)(_data);
    if (data->SMSSignatureBefore)
        edtBefore->setText(QString::fromUtf8(data->SMSSignatureBefore));
    if (data->SMSSignatureAfter)
        edtAfter->setText(QString::fromUtf8(data->SMSSignatureAfter));
    chkTranslit->setChecked(data->SMSTranslit);
}

void SMSConfig::apply(void *_data)
{
    SMSUserData *data = (SMSUserData*)(_data);
    set_str(&data->SMSSignatureBefore, edtBefore->text().utf8());
    set_str(&data->SMSSignatureAfter, edtAfter->text().utf8());
    data->SMSTranslit = chkTranslit->isChecked();
}

#ifndef WIN32
#include "smscfg.moc"
#endif

