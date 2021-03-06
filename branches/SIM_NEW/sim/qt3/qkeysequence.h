/****************************************************************************
** $Id: qkeysequence.h,v 1.4 2002-07-11 01:26:48 shutoff Exp $
**
** Definition of QKeySequence class
**
** Created : 0108007
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QKEYSEQUENCE_H
#define QKEYSEQUENCE_H

#ifndef QT_H
#ifndef QT_H
#include "qnamespace.h"
#include "qstring.h"
#include "qt3stuff.h"
#endif // QT_H
#endif

#if QT_VERSION < 300

#ifndef QT_NO_ACCEL

namespace Qt3
{

class QKeySequencePrivate;

class QKeySequence : public Qt
{
public:
    QKeySequence();
    QKeySequence( const QString& key );
    QKeySequence( int key );

    operator QString() const;
    operator int () const;

    QKeySequence( const QKeySequence& );
    QKeySequence &operator=( const QKeySequence & );
    ~QKeySequence();
    bool operator==( const QKeySequence& ) const;
    bool operator!= ( const QKeySequence& ) const;

private:
    QKeySequencePrivate* d;

};

/*****************************************************************************
  QKeySequence stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
QDataStream &operator<<( QDataStream &, const QKeySequence & );
QDataStream &operator>>( QDataStream &, QKeySequence & );
#endif

#else
class QKeySequence : public Qt
{
public:
    QKeySequence() {}
    QKeySequence( int ) {}
};

#endif //QT_NO_ACCEL

}

#endif

#endif
