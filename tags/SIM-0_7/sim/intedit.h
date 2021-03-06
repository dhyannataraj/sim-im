/***************************************************************************
                          intedit.h  -  description
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

#ifndef _INTEDIT_H
#define _INTEDIT_H

#include "defs.h"

#include <qlineedit.h>

class IntLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    IntLineEdit(QWidget *parent);
signals:
    void escape();
    void focusOut();
protected:
    void focusOutEvent(QFocusEvent*);
    void keyPressEvent(QKeyEvent*);
};

#endif

