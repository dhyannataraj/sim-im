/***************************************************************************
                          dock.cpp  -  description
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

#include "defs.h"
#include "dock.h"
#include "mainwin.h"
#include "icons.h"
#include "client.h"
#include "history.h"
#include "cuser.h"
#include "log.h"

#include <qpainter.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qvaluelist.h>

#ifdef WIN32
#include <windowsx.h>
#include <shellapi.h>
#endif

#ifdef USE_KDE
#include <kwin.h>
#include <kpopupmenu.h>
#else
#include <qpopupmenu.h>
#include <qbitmap.h>
#endif

#ifndef WIN32
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

unread_msg::unread_msg(ICQMessage *m)
{
    m_id   = m->Id;
    m_type = m->Type();
    m_uin  = m->getUin();
}

#ifdef WIN32

#define WM_DOCK	(WM_USER + 0x100)

static WNDPROC oldDockProc;
static DockWnd *gDock;

LRESULT CALLBACK DockWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DOCK){
        gDock->callProc(lParam);
    }
    if (oldDockProc)
        return oldDockProc(hWnd, msg, wParam, lParam);
    return 0;
}

void DockWnd::callProc(unsigned long param)
{
    switch (param){
    case WM_RBUTTONDOWN:
        POINT pos;
        GetCursorPos(&pos);
        emit showPopup(QPoint(pos.x, pos.y));
        return;
    case WM_LBUTTONDBLCLK:
        bNoToggle = true;
        emit doubleClicked();
        return;
    case WM_LBUTTONDOWN:
        if (!bNoToggle)
            QTimer::singleShot(500, this, SLOT(toggle()));
        return;
    }
}

#else

extern Time qt_x_time;

extern int _argc;
extern char **_argv;

class WharfIcon : public QWidget
{
public:
    WharfIcon(DockWnd *parent);
    ~WharfIcon();
    void set(const char *icon, const char *message);
    bool bActivated;
protected:
    DockWnd *dock;
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent*);
    virtual bool x11Event(XEvent*);
    Window  parentWin;
    QPixmap *vis;
};

WharfIcon::WharfIcon(DockWnd *parent)
        : QWidget(parent, "WharfIcon")
{
    dock = parent;
    setMouseTracking(true);
    QIconSet icon = Icon("offline");
    const QPixmap &pict = icon.pixmap(QIconSet::Large, QIconSet::Normal);
    setIcon(pict);
    resize(pict.width(), pict.height());
    parentWin = 0;
    setBackgroundMode(X11ParentRelative);
    vis = NULL;
    bActivated = false;
}

WharfIcon::~WharfIcon()
{
    if (vis) delete vis;
}

bool WharfIcon::x11Event(XEvent *e)
{
    if ((e->type == ReparentNotify) && !bActivated){
        bActivated = true;
        if (vis) resize(vis->width(), vis->height());
        repaint(false);
    }
    if ((e->type == Expose) && !bActivated)
        return false;
    return QWidget::x11Event(e);
}

void WharfIcon::enterEvent( QEvent* )
{
    if ( !qApp->focusWidget() ) {
        XEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.xfocus.display = qt_xdisplay();
        ev.xfocus.type = FocusIn;
        ev.xfocus.window = winId();
        ev.xfocus.mode = NotifyNormal;
        ev.xfocus.detail = NotifyAncestor;
        Time time = qt_x_time;
        qt_x_time = 1;
        qApp->x11ProcessEvent( &ev );
        qt_x_time = time;
    }
}

#define SMALL_PICT_OFFS	8

void WharfIcon::set(const char *icon, const char *msg)
{
    const QIconSet &icons = Icon(icon);
    QPixmap *nvis = new QPixmap(icons.pixmap(QIconSet::Large, QIconSet::Normal));
    if (bActivated) resize(nvis->width(), nvis->height());
    if (msg){
        QPixmap msgPict = Pict(msg);
        QRegion *rgn = NULL;
        if (nvis->mask() && msgPict.mask()){
            rgn = new QRegion(*msgPict.mask());
            rgn->translate(nvis->width() - msgPict.width() - SMALL_PICT_OFFS,
                           nvis->height() - msgPict.height() - SMALL_PICT_OFFS);
            *rgn += *nvis->mask();
        }
        QPainter p;
        p.begin(nvis);
        p.drawPixmap(nvis->width() - msgPict.width() - SMALL_PICT_OFFS,
                     nvis->height() - msgPict.height() - SMALL_PICT_OFFS, msgPict);
        p.end();
        if (rgn){
            setMask(*rgn);
            delete rgn;
        }
    }else{
        const QBitmap *mask = nvis->mask();
        if (mask) setMask(*mask);
    }
    if (vis) delete vis;
    vis = nvis;
    setIcon(*vis);
    repaint();
}

void WharfIcon::mouseReleaseEvent( QMouseEvent *e)
{
    dock->mouseEvent(e);
}

void WharfIcon::mouseDoubleClickEvent( QMouseEvent *e)
{
    dock->mouseDoubleClickEvent(e);
}

void WharfIcon::paintEvent( QPaintEvent * )
{
    if (!bActivated) return;
    if (vis == NULL) return;
    QPainter painter;
    painter.begin(this);
    painter.drawPixmap(0, 0, *vis);
    painter.end();
}

static XErrorHandler old_handler = 0;
static int dock_xerror = 0;
static int dock_xerrhandler(Display* dpy, XErrorEvent* err)
{
    dock_xerror = err->error_code;
    return old_handler(dpy, err);
}

void trap_errors()
{
    dock_xerror = 0;
    old_handler = XSetErrorHandler(dock_xerrhandler);
}

bool untrap_errors()
{
    XSetErrorHandler(old_handler);
    return (dock_xerror == 0);
}

bool send_message(
    Display* dpy, /* display */
    Window w,     /* sender (tray icon window) */
    long message, /* message opcode */
    long data1,   /* message data 1 */
    long data2,   /* message data 2 */
    long data3    /* message data 3 */
){
    XEvent ev;

    memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.window = w;
    ev.xclient.message_type = XInternAtom (dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = CurrentTime;
    ev.xclient.data.l[1] = message;
    ev.xclient.data.l[2] = data1;
    ev.xclient.data.l[3] = data2;
    ev.xclient.data.l[4] = data3;

    trap_errors();
    XSendEvent(dpy, w, False, NoEventMask, &ev);
    XSync(dpy, False);
    return untrap_errors();
}

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#endif

DockWnd::DockWnd(QWidget *main)
        : QWidget(NULL, "dock",  WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop)
{
    setMouseTracking(true);
    connect(this, SIGNAL(toggleWin()), main, SLOT(toggleShow()));
    connect(this, SIGNAL(showPopup(QPoint)), main, SLOT(showPopup(QPoint)));
    connect(this, SIGNAL(doubleClicked()), main, SLOT(dockDblClicked()));
    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(pMain, SIGNAL(iconChanged()), this, SLOT(reset()));
    m_state = 0;
    showIcon = State;
    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(timer()));
    t->start(800);
    bNoToggle = false;
#ifdef WIN32
    QWidget::hide();
    QWidget::setIcon(Pict(pClient->getStatusIcon()));
    gDock = this;
    oldDockProc = (WNDPROC)SetWindowLongW(winId(), GWL_WNDPROC, (LONG)DockWindowProc);
    if (oldDockProc == 0)
        oldDockProc = (WNDPROC)SetWindowLongA(winId(), GWL_WNDPROC, (LONG)DockWindowProc);
    NOTIFYICONDATAA notifyIconData;
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = topData()->winIcon;
    notifyIconData.hWnd = winId();
    notifyIconData.szTip[0] = 0;
    notifyIconData.uCallbackMessage = WM_DOCK;
    notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    notifyIconData.uID = 0;
    Shell_NotifyIconA(NIM_ADD, &notifyIconData);
#else
    setMinimumSize(22, 22);
    bInit = false;
    inTray = false;
    inNetTray = false;

    wharfIcon = new WharfIcon(this);
    Display *dsp = x11Display();
    WId win = winId();

    setBackgroundMode(X11ParentRelative);
    const QPixmap &pict = Pict(pClient->getStatusIcon());
    setIcon(pict);

    XClassHint classhint;
    classhint.res_name  = (char*)"sim";
    classhint.res_class = (char*)"Wharf";
    XSetClassHint(dsp, win, &classhint);

    Screen *screen = XDefaultScreenOfDisplay(dsp);
    int screen_id = XScreenNumberOfScreen(screen);
    char buf[32];
    snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen_id);
    Atom selection_atom = XInternAtom(dsp, buf, false);
    XGrabServer(dsp);
    Window manager_window = XGetSelectionOwner(dsp, selection_atom);
    if (manager_window != None)
        XSelectInput(dsp, manager_window, StructureNotifyMask);
    XUngrabServer(dsp);
    XFlush(dsp);
    if (manager_window != None){
        inNetTray = true;
        if (!send_message(dsp, manager_window, SYSTEM_TRAY_REQUEST_DOCK, win, 0, 0)){
            inNetTray = false;
        }
    }

    Atom kde_net_system_tray_window_for_atom = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);

    long data[1];
    data[0] = 0;
    XChangeProperty(dsp, win, kde_net_system_tray_window_for_atom, XA_WINDOW,
                    32, PropModeReplace,
                    (unsigned char*)data, 1);

    XWMHints *hints;
    hints = XGetWMHints(dsp, win);
    hints->initial_state = WithdrawnState;
    hints->icon_x = 0;
    hints->icon_y = 0;
    hints->icon_window = wharfIcon->winId();
    hints->window_group = win;
    hints->flags = WindowGroupHint | IconWindowHint | IconPositionHint | StateHint;
    XSetWMHints(dsp, win, hints);
    XFree( hints );
    XSetCommand(dsp, win, _argv, _argc);

    if (!inNetTray){
        move(-21, -21);
        resize(22, 22);
    }
    show();
#endif
    reset();
}

DockWnd::~DockWnd()
{
#ifdef WIN32
    NOTIFYICONDATAA notifyIconData;
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = 0;
    notifyIconData.hWnd = winId();
    memset(notifyIconData.szTip, 0, sizeof(notifyIconData.szTip));
    notifyIconData.uCallbackMessage = 0;
    notifyIconData.uFlags = 0;
    notifyIconData.uID = 0;
    int i = Shell_NotifyIconA(NIM_DELETE, &notifyIconData);
#endif
}

#ifndef WIN32

bool DockWnd::x11Event(XEvent *e)
{
    if (e->type == ClientMessage){
        if (!inTray){
            Atom xembed_atom = XInternAtom( qt_xdisplay(), "_XEMBED", FALSE );
            if (e->xclient.message_type == xembed_atom){
                inTray = true;
                bInit = true;
                if (wharfIcon){
                    delete wharfIcon;
                    wharfIcon = NULL;
                }
                reset();
            }
        }
    }
    if ((e->type == ReparentNotify) && !bInit && inNetTray){
        Display *dsp = qt_xdisplay();
        if (e->xreparent.parent == XRootWindow(dsp,
                                               XScreenNumberOfScreen(XDefaultScreenOfDisplay(dsp)))){
            inNetTray = false;
        }else{
            inTray = true;
            if (wharfIcon){
                delete wharfIcon;
                wharfIcon = NULL;
            }
            bInit = true;
            move(0, 0);
            resize(22, 22);
            XResizeWindow(dsp, winId(), 22, 22);
            reset();
        }
    }
    if (((e->type == FocusIn) || (e->type == Expose)) && !bInit){
        if (wharfIcon){
            delete wharfIcon;
            wharfIcon = NULL;
        }

        if (!inTray){
            bInit = true;
            setFocusPolicy(NoFocus);
            move(pMain->DockX, pMain->DockY);
            reset();
        }
    }
    return QWidget::x11Event(e);
}

#endif

void DockWnd::paintEvent( QPaintEvent* )
{
#ifndef WIN32
    if (!bInit) return;
#endif
    QPainter p(this);
    p.drawPixmap((width() - drawIcon.width())/2, (height() - drawIcon.height())/2, drawIcon);
}

void DockWnd::setIcon(const QPixmap &p)
{
#ifndef WIN32
    if (!inTray) return;
#endif
    drawIcon = p;
#ifdef WIN32
    QWidget::setIcon(p);
    NOTIFYICONDATAA notifyIconData;
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = topData()->winIcon;
    notifyIconData.hWnd = winId();
    notifyIconData.szTip[0] = 0;
    notifyIconData.uCallbackMessage = 0;
    notifyIconData.uFlags = NIF_ICON;
    notifyIconData.uID = 0;
    Shell_NotifyIconA(NIM_MODIFY, &notifyIconData);
#else 
    repaint();
#endif
}

void DockWnd::setTip(const QString &tip)
{
#ifdef WIN32
    NOTIFYICONDATAA notifyIconData;
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = topData()->winIcon;
    notifyIconData.hWnd = winId();
    strncpy(notifyIconData.szTip, tip.local8Bit(), sizeof(notifyIconData.szTip));
    notifyIconData.uCallbackMessage = 0;
    notifyIconData.uFlags = NIF_TIP;
    notifyIconData.uID = 0;
    Shell_NotifyIconA(NIM_MODIFY, &notifyIconData);
#else
    if (wharfIcon == NULL){
        if (isVisible()){
            QToolTip::remove(this);
            QToolTip::add(this, tip);
        }
    }else{
        if (wharfIcon->isVisible()){
            QToolTip::remove(wharfIcon);
            QToolTip::add(wharfIcon, tip);
        }
    }
#endif
}

void DockWnd::timer()
{
    if (++m_state >= 4) m_state = 0;
    ShowIcon needIcon = State;
    bool bBlinked = pClient->isConnecting();
    unsigned short msgType = 0;
    if (pMain->messages.size()) msgType = pMain->messages.back().type();
    switch (m_state){
    case 1:
        if (msgType){
            needIcon = Message;
        }else if (bBlinked){
            needIcon = Blinked;
        }
        break;
    case 2:
        if (msgType && bBlinked)
            needIcon = Blinked;
        break;
    case 3:
        if (msgType){
            needIcon = Message;
        }else if (bBlinked){
            needIcon = Blinked;
        }
        break;
    }
    if (needIcon == showIcon) return;
    showIcon = needIcon;
    switch (showIcon){
    case State:
        setIcon(Pict(pClient->getStatusIcon()));
        break;
    case Message:
        setIcon(Pict(SIMClient::getMessageIcon(msgType)));
        break;
    case Blinked:
        setIcon(Pict(SIMClient::getStatusIcon(ICQ_STATUS_ONLINE)));
        break;
    default:
        break;
    }
#ifndef WIN32
    if (inTray) return;
    const char *icon = pClient->getStatusIcon();
    const char *msg  = NULL;
    const char *bmsg = NULL;
    if (msgType) bmsg = SIMClient::getMessageIcon(msgType);
    if (bBlinked){
        if (m_state & 1){
            icon = SIMClient::getStatusIcon(ICQ_STATUS_ONLINE);
        }else{
            msg = bmsg;
        }
    }else{
        if (!(m_state & 1)){
            msg = bmsg;
        }
    }
    if (wharfIcon){
        wharfIcon->set(icon, msg);
        return;
    }
    const QIconSet &icons = Icon(icon);
    QPixmap nvis(icons.pixmap(QIconSet::Large, QIconSet::Normal));
    resize(nvis.width(), nvis.height());
    if (msg){
        QPixmap msgPict = Pict(msg);
        QRegion *rgn = NULL;
        if (nvis.mask() && msgPict.mask()){
            rgn = new QRegion(*msgPict.mask());
            rgn->translate(nvis.width() - msgPict.width() - SMALL_PICT_OFFS,
                           nvis.height() - msgPict.height() - SMALL_PICT_OFFS);
            *rgn += *nvis.mask();
        }
        QPainter p;
        p.begin(&nvis);
        p.drawPixmap(nvis.width() - msgPict.width() - SMALL_PICT_OFFS,
                     nvis.height() - msgPict.height() - SMALL_PICT_OFFS, msgPict);
        p.end();
        if (rgn){
            setMask(*rgn);
            delete rgn;
        }
    }else{
        const QBitmap *mask = nvis.mask();
        if (mask) setMask(*mask);
    }
    drawIcon = nvis;
    repaint();
#endif
}

void DockWnd::processEvent(ICQEvent *e)
{
    if ((e->type() == EVENT_USER_DELETED) ||
            ((e->type() == EVENT_STATUS_CHANGED) && (e->Uin() == pClient->owner->Uin))){
        showIcon = Unknown;
        reset();
        timer();
    }
}

void DockWnd::reset()
{
    list<msgInfo> msgs;
    pMain->fillUnread(msgs);
    QString s;
    if (msgs.size()){
        QStringList str;
        for (list<msgInfo>::iterator it_msg = msgs.begin(); it_msg != msgs.end(); ++it_msg){
            CUser u((*it_msg).uin);
            str.append(i18n("%1 from %2")
                       .arg(SIMClient::getMessageText((*it_msg).type, (*it_msg).count))
                       .arg(u.name(true)));
        }
#ifdef WIN32
        s = str.join(" ");
#else
        s = str.join("<br>");
#endif
    }else{
        if (!pClient->isConnecting()){
            s = pClient->getStatusText();
        }else{
            s = i18n("Connecting");
        }
    }
    setTip(s);
    showIcon = Unknown;
    timer();
}

void DockWnd::toggle()
{
    if (bNoToggle){
        bNoToggle = false;
        return;
    }
    emit toggleWin();
}

void DockWnd::mouseEvent( QMouseEvent *e)
{
    switch(e->button()){
    case QWidget::LeftButton:
        if (!bNoToggle)
            QTimer::singleShot(700, this, SLOT(toggle()));
        break;
    case QWidget::RightButton:
        emit showPopup(e->globalPos());
        break;
    default:
        break;
    }
}

void DockWnd::mousePressEvent( QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
#ifndef WIN32
    if (inTray || wharfIcon) return;
    grabMouse();
    mousePos = e->pos();
#endif
}

void DockWnd::mouseReleaseEvent( QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
#ifndef WIN32
    if (!inTray && (wharfIcon == NULL)){
        releaseMouse();
        if (!mousePos.isNull()){
            move(e->globalPos().x() - mousePos.x(),  e->globalPos().y() - mousePos.y());
            mousePos = QPoint();
            QPoint p(pMain->DockX - x(), pMain->DockY - y());
            pMain->DockX = x();
            pMain->DockY = y();
            if (p.manhattanLength() > 6)
                return;
        }
    }
#endif
    mouseEvent(e);
}

void DockWnd::mouseMoveEvent( QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
#ifndef WIN32
    if (inTray || wharfIcon) return;
    if (mousePos.isNull()) return;
    move(e->globalPos().x() - mousePos.x(), e->globalPos().y() - mousePos.y());
#endif
}

void DockWnd::mouseDoubleClickEvent( QMouseEvent*)
{
    bNoToggle = true;
    emit doubleClicked();
}

void DockWnd::enterEvent( QEvent* )
{
#ifndef WIN32
    // FIXME(E): Implement for Qt Embedded
    if (wharfIcon != NULL) return;
    if ( !qApp->focusWidget() ) {
        XEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.xfocus.display = qt_xdisplay();
        ev.xfocus.type = FocusIn;
        ev.xfocus.window = winId();
        ev.xfocus.mode = NotifyNormal;
        ev.xfocus.detail = NotifyAncestor;
        Time time = qt_x_time;
        qt_x_time = 1;
        qApp->x11ProcessEvent( &ev );
        qt_x_time = time;
    }
#endif
}

#ifndef _WINDOWS
#include "dock.moc"
#endif

