/***************************************************************************
                          replace.h  -  description
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

#ifndef _REPLACE_H
#define _REPLACE_H

#include "simapi.h"
//Added by qt3to4:
#include <QEvent>

typedef struct ReplaceData
{
    Data	Keys;
    Data	Qt::Key;
    Data	Value;
} ReplaceData;

class ReplacePlugin : public QObject, public Plugin
{
    Q_OBJECT
public:
    ReplacePlugin(unsigned, Buffer *cfg);
    virtual ~ReplacePlugin();
    PROP_ULONG(Keys)
    PROP_UTFLIST(Qt::Key)
    PROP_UTFLIST(Value)
protected:
    virtual string getConfig();
    virtual QWidget *createConfigWindow(QWidget *parent);
    bool eventFilter(QObject *o, QEvent *e);
    ReplaceData data;
};

#endif

