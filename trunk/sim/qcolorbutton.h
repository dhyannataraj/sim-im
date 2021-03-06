/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301, USA.
*/

#ifndef __QCOLBTN_H__
#define __QCOLBTN_H__

#include "simapi.h"

#ifndef USE_KDE
#include <qpushbutton.h>

class EXPORT QColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY( QColor color READ color WRITE setColor )

public:
    QColorButton( QWidget *parent, const char *name = 0L );
    QColorButton( const QColor &c, QWidget *parent, const char *name = 0L );
    virtual ~QColorButton() {}
    QColor color() const
        {	return col; }
    void setColor( const QColor &c );
    QSize sizeHint() const;

signals:
    void changed( const QColor &newColor );

protected slots:
    void chooseColor();

protected:
    virtual void drawButtonLabel( QPainter *p );

private:
    QColor col;
    QPoint mPos;
};

#else
#include <kcolorbutton.h>

class EXPORT QColorButton : public KColorButton
{
public:
    QColorButton(QWidget *parent, const char *name = NULL);
};

#endif

#endif

