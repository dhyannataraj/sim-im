/***************************************************************************
                          xpstyle.h  -  description
                             -------------------
    begin                : Sun Mar 17 2002
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

#ifndef _XPSTYLE_H
#define _XPSTYLE_H

#include <qwindowsstyle.h>

class QXpStyle: public QWindowsStyle
{
    Q_OBJECT
public:
    QXpStyle();
    virtual ~QXpStyle();
};

#endif

