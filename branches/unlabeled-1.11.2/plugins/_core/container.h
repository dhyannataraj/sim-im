/***************************************************************************
                          container.h  -  description
                             -------------------
    begin                : Sun Mar 10 2002
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

#ifndef _CONTAINER_H
#define _CONTAINER_H

#include "simapi.h"
#include "stl.h"

#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qtabbar.h>

const unsigned NEW_CONTAINER	= (unsigned)(-1);
const unsigned GRP_CONTAINER	= 0x80000000;

class UserWnd;
class UserTabBar;
class QSplitter;
class CToolBar;
class QWidgetStack;
class CorePlugin;
class Container;
class QAccel;

typedef struct ContainerData
{
    Data	Id;
    Data	Windows;
    Data	ActiveWindow;
    Data	geometry[5];
    Data	barState[7];
    Data	StatusSize;
    Data	WndConfig;
} ContainerData;

class ContainerStatus : public QStatusBar
{
    Q_OBJECT
public:
    ContainerStatus(QWidget *parent);
signals:
    void sizeChanged(int);
protected:
    void resizeEvent(QResizeEvent*);
};

class UserTabBar : public QTabBar
{
    Q_OBJECT
public:
    UserTabBar(QWidget *parent);
    void raiseTab(unsigned id);
    UserWnd *wnd(unsigned id);
    UserWnd *currentWnd();
    list<UserWnd*> windows();
    void removeTab(unsigned id);
    void changeTab(unsigned id);
    void setBold(unsigned id, bool bState);
    void setCurrent(unsigned i);
    unsigned current();
    bool isBold(UserWnd *wnd);
public slots:
    void slotRepaint();
protected:
    virtual void layoutTabs();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void paintLabel(QPainter *p, const QRect &rc, QTab *t, bool bFocus) const;
};

class Container : public QMainWindow, public EventReceiver
{
    Q_OBJECT
public:
    Container(unsigned id, const char *cfg = NULL);
    ~Container();
    QString name();
    UserWnd *wnd(unsigned id);
    UserWnd *wnd();
    list<UserWnd*> windows();
    string getState();
    bool isReceived() { return m_bReceived; }
    void setReceived(bool bReceived) { m_bReceived = bReceived; }
    void setNoSwitch(bool bState);
    void setMessageType(unsigned id);
    void contactChanged(Contact *contact);
    PROP_ULONG(Id);
    PROP_STR(Windows);
    PROP_ULONG(ActiveWindow);
    PROP_ULONG(StatusSize);
    PROP_STRLIST(WndConfig);
    bool m_bNoRead;
public slots:
    void addUserWnd(UserWnd*, bool bRaise);
    void removeUserWnd(UserWnd*);
    void raiseUserWnd(UserWnd*);
    void contactSelected(int);
    void toolbarChanged(QToolBar*);
    void statusChanged(int);
    void accelActivated(int);
    void statusChanged(UserWnd*);
    void modeChanged();
    void wndClosed();
    void flash();
protected:
    virtual void resizeEvent(QResizeEvent*);
    virtual void moveEvent(QMoveEvent*);
    virtual bool event(QEvent*);
    void *processEvent(Event*);
    void showBar();
    void init();
    void setupAccel();
    ContainerData	data;
    bool			m_bInit;
    bool			m_bInSize;
    bool			m_bStatusSize;
    bool			m_bBarChanged;
    bool			m_bReceived;
    bool			m_bNoSwitch;
#ifdef WIN32
	bool			m_bInitWnd;
#endif
    CToolBar		*m_bar;
    QSplitter		*m_tabSplitter;
    UserTabBar		*m_tabBar;
    ContainerStatus	*m_status;
    QWidgetStack	*m_wnds;
    QAccel			*m_accel;
};

#endif

