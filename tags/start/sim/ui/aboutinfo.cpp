/***************************************************************************
                          aboutinfo.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : shutoff@mail.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "aboutinfo.h"
#include "icqclient.h"
#include "client.h"
#include "icons.h"

#include <qmultilineedit.h>
#include <qlabel.h>
#include <qpixmap.h>

AboutInfo::AboutInfo(QWidget *p, bool readOnly)
        : AboutInfoBase(p)
{
    lblPict->setPixmap(Pict("info"));
    if (readOnly){
        edtAbout->setReadOnly(true);
    }else{
        load(pClient);
    }
}

void AboutInfo::load(ICQUser *u)
{
    edtAbout->setText(QString::fromLocal8Bit(u->About.c_str()));
}

void AboutInfo::save(ICQUser*)
{
}

void AboutInfo::apply(ICQUser *u)
{
    u->About = edtAbout->text().local8Bit();
}

#ifndef _WINDOWS
#include "aboutinfo.moc"
#endif

