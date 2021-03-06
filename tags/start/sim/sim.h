/***************************************************************************
                          sim.h  -  description
                             -------------------
    begin                : Sun Mar 10 2002
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

#ifndef _SIM_H
#define _SIM_H

#include "defs.h"

class MainWindow;

#if USE_KDE
#include <kuniqueapp.h>

class SimApp : public KUniqueApplication
{
    Q_OBJECT
public:
    SimApp();
    ~SimApp();
    virtual int newInstance();
protected:
    void saveState(QSessionManager&);
    bool firstInstance;
};
#endif

#endif


