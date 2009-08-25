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

#include <q3multilineedit.h>
#include <QCheckBox>

SMSConfig::SMSConfig(QWidget *parent, void *_data) : QWidget(parent)
        //: SMSConfigBase(parent)
{
	setupUi(this);
    SMSUserData *data = (SMSUserData*)(_data);
    edtBefore->setText(data->SMSSignatureBefore.str());
    edtAfter->setText(data->SMSSignatureAfter.str());
}

void SMSConfig::apply(void *_data)
{
    SMSUserData *data = (SMSUserData*)(_data);
    data->SMSSignatureBefore.str() = edtBefore->text();
    data->SMSSignatureAfter.str() = edtAfter->text();
}

/*
#ifndef NO_MOC_INCLUDES
#include "smscfg.moc"
#endif
*/

