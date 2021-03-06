/***************************************************************************
                          transparent.cpp  -  description
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

#include <qapplication.h>
#include <qwidgetlist.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qpainter.h>

#include "misc.h"

#include "transparent.h"
#include "transparentcfg.h"
#include "../floaty/floatywnd.h" //Handle Floatings

#ifndef WIN32
#include "transtop.h"
#endif

using namespace SIM;

#ifdef WIN32
#include <qlibrary.h>
#include <windows.h>

#define SHOW_TIMEOUT	300
#define HIDE_TIMEOUT	1000

typedef BOOL(WINAPI *slwa_ptr)(HWND, COLORREF, BYTE, DWORD);
static slwa_ptr slwa = NULL;

#if _WIN32_WINNT < 0x0500
#define WS_EX_LAYERED           0x00080000
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002
#endif

#endif

Plugin *createTransparentPlugin(unsigned base, bool, Buffer *config)
{
#ifdef WIN32
    slwa = (slwa_ptr)QLibrary::resolve("user32.dll","SetLayeredWindowAttributes");
    if (slwa == NULL)
        return NULL;
#endif
    Plugin *plugin = new TransparentPlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Transparent"),
#ifdef WIN32
        I18N_NOOP("Plugin provides windows transparency\n"
                  "This plugin works only on Windows 2000 or Windows XP")
#else
        I18N_NOOP("Plugin provides windows transparency")
#endif
        ,
        VERSION,
        createTransparentPlugin,
#ifdef WIN32
        PLUGIN_DEFAULT
#else
        PLUGIN_NOLOAD_DEFAULT
#endif
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

static DataDef transparentData[] =
    {
        { "Transparency", DATA_ULONG, 1, DATA(60) },
#ifdef WIN32
        { "IfInactive",   DATA_BOOL, 1, DATA(1) },
		{ "IfMainWindow", DATA_BOOL, 1, DATA(1) },
		{ "IfFloatings",  DATA_BOOL, 1, DATA(1) },
#endif
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

TransparentPlugin::TransparentPlugin(unsigned base, Buffer *config)
        : Plugin(base)
#ifndef WIN32
        , EventReceiver(HighPriority)
#endif
{
    load_data(transparentData, &data, config);
    if (getTransparency() >100)
        setTransparency(100);
#ifdef WIN32
    timer = NULL;
    m_bHaveMouse = false;
    m_bActive    = false;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(tickMouse()));
    timer->start(1000);
#else
    top = NULL;
#endif
    setState();
}

void TransparentPlugin::topDestroyed()
{
#ifndef WIN32
    top = NULL;
#endif
}

TransparentPlugin::~TransparentPlugin()
{
#ifdef WIN32
    QWidget *main = getMainWindow();
    if (main)
        SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
    if (timer)
        delete timer;
	
	//Handle Floatings

	QWidgetList *list = QApplication::topLevelWidgets();
	QWidgetListIt it(*list);
	QWidget * w;
	while ((w = it.current()) != NULL) {
		if (w->inherits("FloatyWnd")){
			FloatyWnd *refwnd = static_cast<FloatyWnd*>(w);
			SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(refwnd->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
		}
		++it;
	}

#else
    if (top)
        delete top;
#endif
    free_data(transparentData, &data);
}

QCString TransparentPlugin::getConfig()
{
    return save_data(transparentData, &data);
}

QWidget *TransparentPlugin::getMainWindow()
{
    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    QWidget *w;
    while ( (w=it.current()) != 0 ) {
        ++it;
        if (w->inherits("MainWindow")){
            delete list;
            return w;
        }
    }
    delete list;
    return NULL;
}

QWidget *TransparentPlugin::createConfigWindow(QWidget *parent)
{
    return new TransparentCfg(parent, this);
}

void TransparentPlugin::tickMouse()
{
#ifdef WIN32
    QPoint p = QCursor::pos();
    bool bMouse = false;
    QWidget *main = getMainWindow();
    if (main && main->isVisible()){
        if (main->frameGeometry().contains(p))
            bMouse = true;
    }
	
	
	//Handle Floatings//
	QWidgetList *list = QApplication::topLevelWidgets();
	QWidgetListIt it(*list);
	QWidget * w;
	while ((w = it.current()) != NULL) {
		if (w->inherits("FloatyWnd")){
			if (w->frameGeometry().contains(p))
            bMouse = true;
		}
		++it;
	}
	delete list; //Handle Floatings//




    if (bMouse != m_bHaveMouse){
        m_bHaveMouse = bMouse;
        setState();
    }
#endif
}

bool TransparentPlugin::eventFilter(QObject *o, QEvent *e)
{
#ifdef WIN32
    if (getIfInactive()){
        switch (e->type()){
        case QEvent::WindowActivate:
            m_bActive = true;
            setState();
            break;
        case QEvent::WindowDeactivate:
            m_bActive = false;
            setState();
            break;
        case QEvent::Show:{
                QWidget *main = getMainWindow();
                if (main){
                    setState();
                }
                break;
            }
        default:
            break;
        }
    }
#endif
    return QObject::eventFilter(o, e);
}

void TransparentPlugin::setState()
{
    QWidget *main = getMainWindow();
	QWidgetList *list = QApplication::topLevelWidgets();
        
    if (main == NULL)
        return;
#ifdef WIN32
    if (timer == NULL){
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
        main->installEventFilter(this);
        if (getIfMainWindow()) {
			SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
			slwa(main->winId(), main->colorGroup().background().rgb(), 0, LWA_ALPHA);
			RedrawWindow(main->winId(), NULL, NULL, RDW_UPDATENOW);
		}
		else
			SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));

		main->setMouseTracking(true);
        m_bActive = main->isActiveWindow();
        m_bState  = !m_bActive;
		
		//Handle Floatings
		
			QWidgetListIt it(*list);
			QWidget * w;
			while ((w = it.current()) != NULL) {
				if (w->inherits("FloatyWnd")){
					FloatyWnd *refwnd = static_cast<FloatyWnd*>(w);
					refwnd->installEventFilter(this);
					if (getIfFloatings()) {
						SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
						slwa(refwnd->winId(), refwnd->colorGroup().background().rgb(), 0, LWA_ALPHA);
						RedrawWindow(refwnd->winId(), NULL, NULL, RDW_UPDATENOW);
					}
					else 
						SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(refwnd->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
				}
				++it;
			}
			delete list; //Handle Floatings//
		
    }
    bool bNewState = m_bActive || m_bHaveMouse;
    if (bNewState == m_bState){
		BYTE d = (BYTE)(bNewState ? 255 : QMIN((100 - getTransparency()) * 256 / 100, 255));
		if (getIfMainWindow()) {
			SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
			slwa(main->winId(), main->colorGroup().background().rgb(), d, LWA_ALPHA);
			RedrawWindow(main->winId(), NULL, NULL, RDW_UPDATENOW);
		}
		else
			SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));

		
		//Handle Floatings
		QWidgetListIt it(*list);
		QWidget * w;
		while ((w = it.current()) != NULL) {
			if (w->inherits("FloatyWnd")){
				FloatyWnd *refwnd = static_cast<FloatyWnd*>(w);
				refwnd->installEventFilter(this);
				if (getIfFloatings()) {
					SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(refwnd->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
					slwa(refwnd->winId(), refwnd->colorGroup().background().rgb(), d, LWA_ALPHA);
					RedrawWindow(refwnd->winId(), NULL, NULL, RDW_UPDATENOW);
				}
				else 
					SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(refwnd->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
			}
			++it;
		}
		delete list;//Handle Floatings//
        return;
    }
    m_bState = bNewState;
    startTime = GetTickCount();
    timer->start(10);
#else
    if (!top) {
        top = new TransparentTop(main, getTransparency());
        connect(top,SIGNAL(destroyed()),this,SLOT(topDestroyed()));
    }
    top->setTransparent(getTransparency());
#endif
}

void TransparentPlugin::tick()
{
#ifdef WIN32
    QWidget *main = getMainWindow();
    if (main == NULL){
        timer->stop();
        return;
    }
    unsigned timeout = m_bActive ? SHOW_TIMEOUT : HIDE_TIMEOUT;
    unsigned time = GetTickCount() - startTime;
    if (time >= timeout){
        time = timeout;
        timer->stop();
    }
    if (m_bState)
        time = timeout - time;

    BYTE d = (BYTE)QMIN((100 - getTransparency() * time / timeout) * 256 / 100, 255);
	if (getIfMainWindow()) 
		slwa(main->winId(), main->colorGroup().background().rgb(), d, LWA_ALPHA);
	else
		SetWindowLongW(main->winId(), GWL_EXSTYLE, GetWindowLongW(main->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));

	//Handle Floatings
	
	QWidgetList *list = QApplication::topLevelWidgets();
	QWidgetListIt it(*list);
	QWidget * w;
	while ((w = it.current()) != NULL) {
		if (w->inherits("FloatyWnd")){
			//w->installEventFilter(this);
			//w->setMouseTracking(true);
			FloatyWnd *refwnd = static_cast<FloatyWnd*>(w);
			refwnd->installEventFilter(this);
			if (getIfFloatings()) 
				slwa(refwnd->winId(), refwnd->colorGroup().background().rgb(), d, LWA_ALPHA);
			else
				SetWindowLongW(refwnd->winId(), GWL_EXSTYLE, GetWindowLongW(refwnd->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
		}
		++it;
	}//Handle Floatings//
	
#endif
}

bool TransparentPlugin::processEvent(Event *e)
{
    if (e->type() == eEventInit) {
#ifndef WIN32
        top = NULL;
#endif
        setState();
    }
#ifndef WIN32
    if (e->type() == eEventPaintView){
        if (top == NULL)
            return false;
        EventPaintView *ev = static_cast<EventPaintView*>(e);
        EventPaintView::PaintView *pv = ev->paintView();
        QPixmap pict = top->background(pv->win->colorGroup().background());
        if (!pict.isNull()){
            QPoint p = pv->pos;
            p = pv->win->mapToGlobal(p);
            p = pv->win->topLevelWidget()->mapFromGlobal(p);
            pv->p->drawPixmap(0, 0, pict, p.x(), p.y());
            pv->isStatic = true;
        }
    }
    if (e->type() == eEventRaiseWindow){
        EventRaiseWindow *w = static_cast<EventRaiseWindow*>(e);
        if (w->widget() == getMainWindow())
            setState();
    }
#endif
    return false;
}

#ifndef NO_MOC_INCLUDES
#include "transparent.moc"
#endif
