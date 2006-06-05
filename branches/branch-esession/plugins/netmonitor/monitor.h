/***************************************************************************
                          monitor.h  -  description
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

#ifndef _MONITOR_H
#define _MONITOR_H

#include "simapi.h"
#include <qmainwindow.h>

class TextShow;
class QPopupMenu;
class NetmonitorPlugin;

const unsigned short L_PACKETS = 0x08;

class MonitorWindow : public QMainWindow, public SIM::EventReceiver
{
    Q_OBJECT
public:
    MonitorWindow(NetmonitorPlugin*);
signals:
    void finished();
protected slots:
    void save();
    void exit();
    void copy();
    void erase();
    void pause();
    void toggleType(int);
    void toggleAutoscroll();
    void adjustFile();
    void adjustEdit();
    void adjustLog();
protected:
    void *processEvent(SIM::Event*);
    void closeEvent(QCloseEvent*);
    bool bPause;
    bool bAutoscroll;
    TextShow  *edit;
    QPopupMenu *menuFile;
    QPopupMenu *menuEdit;
    QPopupMenu *menuLog;
    NetmonitorPlugin *m_plugin;
};

#endif

