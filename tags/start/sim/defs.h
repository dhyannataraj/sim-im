/***************************************************************************
                          locale.h  -  description
                             -------------------
    begin                : Mon Mar 18 2002
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

#ifndef _DEFS_H
#define _DEFS_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif
#if HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if HAVE_STDINT_H
#include <stdint.h>
#endif
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef USE_KDE

class QKeySequence;

#include <klocale.h>
#include <klineedit.h>
#include <klistbox.h>
#include <kcombobox.h>
#include <kcolorbutton.h>
#define QLineEdit KLineEdit
#define QListBox  KListBox
#define QComboBox KComboBox
#define QColorButton KColorButton
#else
#include <qobject.h>
QString i18n(const char *text);
QString i18n(const char *singular, const char *plural, unsigned long n);
#define KPopupMenu QPopupMenu
#endif

#if _MSC_VER > 1020
#pragma warning(disable:4530)
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *a, const char *b);
#endif

#if QT_VERSION < 300

#include <qpoint.h>

class QContextMenuEvent
{
public:
    QContextMenuEvent(const QPoint &pos) : p(pos) {}
    const QPoint &globalPos() { return p; }
    void accept() {}
protected:
    QPoint p;
};

#endif
#endif
