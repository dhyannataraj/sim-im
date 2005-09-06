/***************************************************************************
                          dockwnd.h  -  description
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

#ifndef _DOCKWND_H
#define _DOCKWND_H

#include "simapi.h"
#include "stl.h"

#include <qwidget.h>
#include <qtooltip.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>

class WharfIcon;
class DockPlugin;

#ifdef WIN32

typedef struct BalloonItem
{
    QString		text;
    QString		title;
    unsigned	id;
    unsigned	flags;
    Client		*client;
} BallonItem;

#endif

class DockWnd : public QWidget, public EventReceiver
{
    Q_OBJECT
public:
    DockWnd(DockPlugin *plugin, const char *icon, const char *text);
    ~DockWnd();
    void setIcon(const char *icon);
    void setTip(const char *text);
#ifdef WIN32
    void callProc(unsigned long);
#endif
    void mouseEvent( QMouseEvent *e);
    virtual void mouseDoubleClickEvent( QMouseEvent *e);
signals:
    void showPopup(QPoint);
    void toggleWin();
    void doubleClicked();
protected slots:
    void toggle();
    void blink();
    void dbl_click();
    void showPopup();
protected:
    virtual void *processEvent(Event *e);
    void  reset();
    bool  bNoToggle;
    const char *m_tip;
    const char *m_state;
    const char *m_unread;
    QString m_unreadText;
    QPixmap drawIcon;
    virtual void enterEvent( QEvent *e);
    virtual void paintEvent( QPaintEvent *e);
    virtual void mousePressEvent( QMouseEvent *e);
    virtual void mouseReleaseEvent( QMouseEvent *e);
    virtual void mouseMoveEvent( QMouseEvent *e);
    void quit();
    bool bBlink;
    QTimer *blinkTimer;
#ifndef WIN32
    QPoint mousePos;
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    WharfIcon *wharfIcon;
    bool x11Event(XEvent*);
    bool inTray;
    bool inNetTray;
    bool bInit;
    friend class WharfIcon;
#endif
#else
    void				*hShell;
    bool				m_bBalloon;
    list<BalloonItem>	m_queue;
    bool				showBalloon();
#endif
    DockPlugin *m_plugin;
};

#endif

