/***************************************************************************
                          mainwin.h  -  description
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

#ifndef _MAINWIN_H
#define _MAINWIN_H

#include "simapi.h"
#include "event.h"

#include "cfg.h"

#include <QMainWindow>
//Added by qt3to4:
#include <QResizeEvent>
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QEvent>
#include <QCloseEvent>
#include <QHBoxLayout>

class QToolBat;
class CorePlugin;
class QSizeGrip;

class MainWindow : public QMainWindow, public SIM::EventReceiver
{
    Q_OBJECT
public:
    MainWindow(SIM::Geometry&);
    ~MainWindow();
    bool m_bNoResize;
	void closeEvent(QCloseEvent *e);
protected slots:
    void setGrip();
protected:
    QWidget *main;
    CToolBar *m_bar;
    QVBoxLayout *lay;
    QHBoxLayout *h_lay;
    QSizeGrip *m_grip;
    void focusInEvent(QFocusEvent*);
    virtual bool processEvent(SIM::Event*);
	void setTitle();
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void quit();
    void addWidget(QWidget*, bool bDown);
    void addStatus(QWidget *w, bool);
    std::list<QWidget*> statusWidgets;
    QString	m_icon;
    friend class CorePlugin;
#ifdef WIN32
    QPoint p;
    QSize s;
#endif
};

#endif

