/***************************************************************************
                          transparent.h  -  description
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

#ifndef _TRANSPARENT_H
#define _TRANSPARENT_H

#include "defs.h"
#include <qobject.h>

class QTextEdit;
class ConfigBool;
class ConfigULong;
class KRootPixmap;
class QPixmap;

class TransparentBg : public QObject
{
    Q_OBJECT
public:
    TransparentBg(QWidget *parent, const char *name = NULL);
    void setTransparent(bool);
    void init(QWidget*);
    const QPixmap *background();
protected slots:
    void backgroundUpdated();
protected:
    bool m_bTransparent;
    bool eventFilter(QObject*, QEvent*);
    int bgX;
    int bgY;
};

class TransparentTop : public QObject
{
    Q_OBJECT
public:
    TransparentTop(QWidget *parent, ConfigBool &useTransparent, ConfigULong &transparent);
    static void setTransparent(QWidget*, bool isTransparent, unsigned long transparency);
    static bool bCanTransparent;
    static TransparentTop *getTransparent(QWidget*);
#if USE_KDE
#if HAVE_KROOTPIXMAP_H
    KRootPixmap *rootpixmap;
    QPixmap bg;
#endif
#endif
signals:
    void backgroundUpdated();
protected slots:
    void updateBackground(const QPixmap &pm);
    void transparentChanged();
protected:
    ConfigBool &useTransparent;
    ConfigULong &transparent;
};

#endif

