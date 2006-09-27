/***************************************************************************
                          dockwnd.cpp  -  description
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

#ifdef WIN32
#define _WIN32_IE 0x0500
#include <windows.h>
#include <shellapi.h>
#endif

#include "simapi.h"
#include "dockwnd.h"
#include "dock.h"
#include "core.h"

#include <stdio.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qvaluelist.h>
#include <qregexp.h>

#ifdef USE_KDE
#include <kwin.h>
#include <kpopupmenu.h>
#else
#include <qpopupmenu.h>
#include <qbitmap.h>
#endif

#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif
#endif

using namespace std;
using namespace SIM;

#ifdef WIN32

const unsigned TIP_TIMEOUT = 24 * 60 * 60 * 1000;

static WNDPROC oldDockProc;
static DockWnd *gDock;
static UINT	   WM_DOCK = 0;

#ifndef NIN_BALLOONSHOW
#define NIN_BALLOONSHOW		(WM_USER + 2)
#endif

#ifndef NIN_BALLOONHIDE
#define NIN_BALLOONHIDE		(WM_USER + 3)
#endif

#ifndef NIN_BALLOONTIMEOUT
#define NIN_BALLOONTIMEOUT		(WM_USER + 4)
#endif

#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK	(WM_USER + 5)
#endif

LRESULT CALLBACK DockWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if ((msg == WM_DOCK) || (msg == 0)){
        gDock->callProc(lParam);
    }
    if (oldDockProc)
        return oldDockProc(hWnd, msg, wParam, lParam);
    return 0;
}

void DockWnd::callProc(unsigned long param)
{
    unsigned id;
    Client *client;
    switch (param){
    case WM_RBUTTONUP:
        QTimer::singleShot(0, this, SLOT(showPopup()));
        return;
    case WM_LBUTTONDBLCLK:
        bNoToggle = true;
        QTimer::singleShot(0, this, SLOT(dbl_click()));
        return;
    case WM_LBUTTONDOWN:
        if (!bNoToggle)
            QTimer::singleShot(500, this, SLOT(toggle()));
        return;
    case NIN_BALLOONHIDE:
    case NIN_BALLOONTIMEOUT:
    case NIN_BALLOONUSERCLICK:
        if (m_queue.empty())
            return;
        id = m_queue.front().id;
        client = m_queue.front().client;
        m_queue.erase(m_queue.begin());
        if (!m_queue.empty())
            showBalloon();
        if (param == NIN_BALLOONUSERCLICK){
            Command cmd;
            cmd->id    = id;
            cmd->param = client;
            Event e(EventCommandExec, cmd);
            e.process();
        }
        return;
    }
}

void DockWnd::showPopup()
{
    POINT pos;
    GetCursorPos(&pos);
    if (qApp->activeWindow() == NULL)
        setFocus();
    emit showPopup(QPoint(pos.x, pos.y));
}

#else

void DockWnd::showPopup()
{
}

#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)

extern Time qt_x_time;

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
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    virtual bool x11Event(XEvent*);
#endif
    unsigned p_width;
    unsigned p_height;
    Window  parentWin;
    QPixmap *vis;
};

WharfIcon::WharfIcon(DockWnd *parent)
        : QWidget(parent, "WharfIcon")
{
    setCaption("SIM Wharf");
    dock = parent;
    p_width  = 64;
    p_height = 64;
    setMouseTracking(true);
    QIconSet icon = Icon("inactive");
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
        XWindowAttributes a;
        XGetWindowAttributes(qt_xdisplay(), e->xreparent.parent, &a);
        p_width  = a.width;
        p_height = a.height;
        bActivated = true;
        dock->bInit = true;
        if (vis){
            resize(vis->width(), vis->height());
            move((p_width - vis->width()) / 2, (p_height - vis->height()) / 2);
        }
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
    QIconSet icons = Icon(icon);
    QPixmap *nvis = new QPixmap(icons.pixmap(QIconSet::Large, QIconSet::Normal));
    if (bActivated){
        resize(nvis->width(), nvis->height());
        move((p_width - nvis->width()) / 2, (p_height - nvis->height()) / 2);
    }
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

#define MWM_HINTS_DECORATIONS         (1L << 1)

typedef struct _mwmhints
{
    unsigned long       flags;
    unsigned long       functions;
    unsigned long       decorations;
    long                inputMode;
    unsigned long       status;
}
MWMHints;

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

// Code for enlightment support (from epplet.c)

static const char        *win_name = NULL;
static const char        *win_version = NULL;
static const char        *win_info = NULL;

static Display     *dd = NULL;
static Window      comms_win = 0;
static Window      my_win = 0;
static Window      root = 0;

static void
CommsFindCommsWindow(void)
{
    unsigned char      *s;
    Atom                a, ar;
    unsigned long       num, after;
    int                 format;
    Window              rt;
    int                 dint;
    unsigned int        duint;

    a = XInternAtom(dd, "ENLIGHTENMENT_COMMS", True);
    if (a != None)
    {
        s = NULL;
        XGetWindowProperty(dd, root, a, 0, 14, False, AnyPropertyType, &ar,
                           &format, &num, &after, &s);
        if (s)
        {
            sscanf((char *)s, "%*s %x", (unsigned int *)&comms_win);
            XFree(s);
        }
        else
            (comms_win = 0);
        if (comms_win)
        {
            if (!XGetGeometry(dd, comms_win, &rt, &dint, &dint,
                              &duint, &duint, &duint, &duint))
                comms_win = 0;
            s = NULL;
            if (comms_win)
            {
                XGetWindowProperty(dd, comms_win, a, 0, 14, False,
                                   AnyPropertyType, &ar, &format, &num,
                                   &after, &s);
                if (s)
                    XFree(s);
                else
                    comms_win = 0;
            }
        }
    }
    if (comms_win)
        XSelectInput(dd, comms_win, StructureNotifyMask | SubstructureNotifyMask);
}

static void
ECommsSetup(Display * d)
{
    dd = d;
    root = DefaultRootWindow(dd);
    if (!my_win)
    {
        my_win = XCreateSimpleWindow(dd, root, -100, -100, 5, 5, 0, 0, 0);
        XSelectInput(dd, my_win, StructureNotifyMask | SubstructureNotifyMask);
    }
    CommsFindCommsWindow();
}

#define ESYNC ECommsSend((char*)"nop");free(ECommsWaitForMessage());

static void
ECommsSend(char *s)
{
    char                ss[21];
    int                 i, j, k, len;
    XEvent              ev;
    Atom                a = 0;

    if (!s)
        return;
    len = strlen(s);
    if (!a)
        a = XInternAtom(dd, "ENL_MSG", False);
    ev.xclient.type = ClientMessage;
    ev.xclient.serial = 0;
    ev.xclient.send_event = True;
    ev.xclient.window = comms_win;
    ev.xclient.message_type = a;
    ev.xclient.format = 8;

    for (i = 0; i < len + 1; i += 12)
    {
        snprintf(ss, sizeof(ss), "%8x", (int)my_win);
        for (j = 0; j < 12; j++)
        {
            ss[8 + j] = s[i + j];
            if (!s[i + j])
                j = 12;
        }
        ss[20] = 0;
        for (k = 0; k < 20; k++)
            ev.xclient.data.b[k] = ss[k];
        XSendEvent(dd, comms_win, False, 0, (XEvent *) & ev);
    }
}

static char        *
ECommsGet(XEvent * ev)
{
    char                s[13], s2[9], *msg = NULL;
    int                 i;
    Window              win = 0;
    static char        *c_msg = NULL;

    if (!ev)
        return NULL;
    if (ev->type != ClientMessage)
        return NULL;
    s[12] = 0;
    s2[8] = 0;
    msg = NULL;
    for (i = 0; i < 8; i++)
        s2[i] = ev->xclient.data.b[i];
    for (i = 0; i < 12; i++)
        s[i] = ev->xclient.data.b[i + 8];
    sscanf(s2, "%x", (unsigned*)&win);
    if (win == comms_win)
    {
        if (c_msg)
        {
            c_msg = (char*)realloc(c_msg, strlen(c_msg) + strlen(s) + 1);
            if (!c_msg)
                return NULL;
            strcat(c_msg, s);
        }
        else
        {
            c_msg = (char*)malloc(strlen(s) + 1);
            if (!c_msg)
                return NULL;
            strcpy(c_msg, s);
        }
        if (strlen(s) < 12)
        {
            msg = c_msg;
            c_msg = NULL;
        }
    }
    return msg;
}

static              Bool
ev_check(Display * d, XEvent * ev, XPointer p)
{
    if (((ev->type == ClientMessage) && (ev->xclient.window == my_win)) ||
            ((ev->type == DestroyNotify) &&
             (ev->xdestroywindow.window == comms_win)))
        return True;
    return False;
    d = NULL;
    p = NULL;
}


static char        *
ECommsWaitForMessage(void)
{
    XEvent              ev;
    char               *msg = NULL;

    while ((!msg) && (comms_win))
    {
        XIfEvent(dd, &ev, ev_check, NULL);
        if (ev.type == DestroyNotify)
            comms_win = 0;
        else
            msg = ECommsGet(&ev);
    }
    return msg;
}

class MyPixmap : public QPixmap
{
public:
    MyPixmap(Pixmap pp, int w, int h);
};

MyPixmap::MyPixmap(Pixmap pp, int w, int h)
        : QPixmap(w, h)
{
    data->uninit = false;
    Screen *screen =  XDefaultScreenOfDisplay(dd);
    int scr = XScreenNumberOfScreen(screen);
    x11SetScreen(scr);
    GC gc = qt_xget_temp_gc( scr, FALSE );
    XSetSubwindowMode( dd, gc, IncludeInferiors );
    XCopyArea( dd, pp, handle(), gc, 0, 0, w, h, 0, 0 );
    XSetSubwindowMode( dd, gc, ClipByChildren );
}

QPixmap
getClassPixmap(char *iclass, char *state, QWidget *w, int width = 0, int height = 0)
{
    Pixmap              pp = 0, mm = 0;
    char                s[1024], *msg;

    if (width == 0) width = w->width();
    if (height == 0) height = w->height();

    QPixmap res = QPixmap();

    snprintf(s, sizeof(s), "imageclass %s apply_copy 0x%x %s %i %i", iclass,
             (unsigned)w->winId(), state, width, height);
    ECommsSend(s);
    msg = ECommsWaitForMessage();
    if (msg)
    {
        sscanf(msg, "%x %x", (unsigned int *)&pp, (unsigned int *)&mm);
        free(msg);
        if (pp){
            res = MyPixmap(pp, width, height);
        }
        snprintf(s, sizeof(s), "imageclass %s free_pixmap 0x%x", iclass,
                 (unsigned int)pp);
        ECommsSend(s);
    }
    return res;
}

void
set_background_properties(QWidget *w)
{
    QPixmap bg = getClassPixmap((char*)"EPPLET_BACKGROUND_VERTICAL", (char*)"normal", w);
    if (!bg.isNull()){
        int border = 2;
        QPixmap img = getClassPixmap((char*)"EPPLET_DRAWINGAREA", (char*)"normal", w,
                                     w->width() - border * 2, w->height() - border * 2);
        if (!img.isNull()){
            QPainter p(&bg);
            p.drawPixmap(border, border, img);
        }
        w->setBackgroundPixmap(bg);
        if (bg.mask()){
            w->setMask(*bg.mask());
        }else{
            w->clearMask();
        }
    }
}

#endif
#endif

DockWnd::DockWnd(DockPlugin *plugin, const char *icon, const char *text)
        : QWidget(NULL, "dock",  WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop),
        EventReceiver(LowPriority)
{
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    wharfIcon = NULL;
#endif
#endif
    m_plugin = plugin;
    setMouseTracking(true);
    bNoToggle = false;
    bBlink = false;
    m_state = icon;
    blinkTimer = new QTimer(this);
    connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blink()));
#ifdef WIN32
    m_bBalloon = false;
    hShell = NULL;
    setIcon(icon);
    QWidget::hide();
    gDock = this;
    WM_DOCK = RegisterWindowMessageA("SIM dock");
    if (IsWindowUnicode(winId())){
        oldDockProc = (WNDPROC)SetWindowLongW(winId(), GWL_WNDPROC, (LONG)DockWindowProc);
        OSVERSIONINFOA osvi;
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        GetVersionExA(&osvi);
        if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osvi.dwMajorVersion > 4))
            m_bBalloon = true;
        NOTIFYICONDATAW notifyIconData;
        if (m_bBalloon){
            memset(&notifyIconData, 0, sizeof(notifyIconData));
            notifyIconData.cbSize = sizeof(notifyIconData);
            notifyIconData.uVersion = NOTIFYICON_VERSION;
            Shell_NotifyIconW(NIM_SETVERSION, (NOTIFYICONDATAW*)&notifyIconData);
        }
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        notifyIconData.uCallbackMessage = WM_DOCK;
        notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        Shell_NotifyIconW(NIM_ADD, &notifyIconData);
    }else{
        oldDockProc = (WNDPROC)SetWindowLongA(winId(), GWL_WNDPROC, (LONG)DockWindowProc);
        NOTIFYICONDATAA notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        notifyIconData.uCallbackMessage = WM_DOCK;
        notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        Shell_NotifyIconA(NIM_ADD, &notifyIconData);
    }
#else
    setMinimumSize(22, 22);
    resize(22, 22);
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    bInit = false;
    inTray = false;
    inNetTray = false;

    Display *dsp = x11Display();
    WId win = winId();

    bool bEnlightenment = false;
    QWidget tmp;
    Atom enlightenment_desktop = XInternAtom(dsp, "ENLIGHTENMENT_DESKTOP", false);
    WId w = tmp.winId();
    Window p, r;
    Window *c;
    unsigned int nc;
    while (XQueryTree(dsp, w, &r, &p, &c, &nc)){
        if (c && nc > 0)
            XFree(c);
        if (! p) {
            log(L_WARN, "No parent");
            break;
        }
        unsigned char *data_ret = NULL;
        Atom type_ret;
        int i_unused;
        unsigned long l_unused;
        if ((XGetWindowProperty(dsp, p, enlightenment_desktop, 0, 1, False, XA_CARDINAL,
                                &type_ret, &i_unused, &l_unused, &l_unused,
                                &data_ret) == Success) && (type_ret == XA_CARDINAL)) {
            if (data_ret)
                XFree(data_ret);
            bEnlightenment = true;
            log(L_DEBUG, "Detect Enlightenment");
            break;
        }
        if (p == r) break;
        w = p;
    }

    if (bEnlightenment){
        bInit = true;
        resize(64, 64);
        setFocusPolicy(NoFocus);
        move(m_plugin->getDockX(), m_plugin->getDockY());
        MWMHints mwm;
        mwm.flags = MWM_HINTS_DECORATIONS;
        mwm.functions = 0;
        mwm.decorations = 0;
        mwm.inputMode = 0;
        mwm.status = 0;
        Atom a = XInternAtom(dsp, "_MOTIF_WM_HINTS", False);
        XChangeProperty(dsp, win, a, a, 32, PropModeReplace,
                        (unsigned char *)&mwm, sizeof(MWMHints) / 4);
        XStoreName(dsp, win, "SIM");
        XClassHint *xch = XAllocClassHint();
        xch->res_name  = (char*)"SIM";
        xch->res_class = (char*)"Epplet";
        XSetClassHint(dsp, win, xch);
        XFree(xch);
        XSetIconName(dsp, win, "SIM");
        unsigned long val = (1 << 0) /* | (1 << 9) */ ;
        a = XInternAtom(dsp, "_WIN_STATE", False);
        XChangeProperty(dsp, win, a, XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)&val, 1);
        val = 2;
        a = XInternAtom(dsp, "_WIN_LAYER", False);
        XChangeProperty(dsp, win, a, XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)&val, 1);
        val = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 5);
        a = XInternAtom(dsp, "_WIN_HINTS", False);
        XChangeProperty(dsp, win, a, XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char *)&val, 1);
        win_name = "SIM";
        win_version = VERSION;
        win_info = "";
        while (!comms_win)
        {
            ECommsSetup(dsp);
            sleep(1);
        }
        char s[256];
        snprintf(s, sizeof(s), "set clientname %s", win_name);
        ECommsSend(s);
        snprintf(s, sizeof(s), "set version %s", win_version);
        ECommsSend(s);
        snprintf(s, sizeof(s), "set info %s", win_info);
        ECommsSend(s);
        ESYNC;

        set_background_properties(this);

        setIcon(icon);
        show();
        return;
    }

    wharfIcon = new WharfIcon(this);
#endif
    setBackgroundMode(X11ParentRelative);
    setIcon(icon);

#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
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

    Event eArgc(EventArgc);
    long argc = (long)eArgc.process();
    Event eArgv(EventArgv);
    char **argv = (char**)eArgv.process();
    XSetCommand(dsp, win, argv, argc);

    if (!inNetTray){
        move(-21, -21);
        resize(22, 22);
    }
    /*
    * show dockWnd only if there is nowhere to dock(e.g. WindowMaker)
    * */
    if (manager_window == None){
        resize(64, 64);
        QApplication::syncX();
        show();
    }
#endif
#endif
    setTip(text);
    reset();
}

DockWnd::~DockWnd()
{
    quit();
#ifdef WIN32
    if (hShell)
        FreeLibrary((HMODULE)hShell);
#endif
}

void DockWnd::quit()
{
#ifdef WIN32
    if (IsWindowUnicode(winId())){
        NOTIFYICONDATAW notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hWnd = winId();
        Shell_NotifyIconW(NIM_DELETE, &notifyIconData);
    }else{
        NOTIFYICONDATAA notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hWnd = winId();
        Shell_NotifyIconA(NIM_DELETE, &notifyIconData);
    }
#endif
}

void DockWnd::dbl_click()
{
    emit doubleClicked();
}

void *DockWnd::processEvent(Event *e)
{
    switch (e->type()){
    case EventMessageReceived:
    case EventMessageRead:
    case EventMessageDeleted:
        reset();
        break;
    case EventSetMainIcon:
        m_state = (const char*)(e->param());
        if (!bBlink)
            setIcon(m_state);
        break;
    case EventSetMainText:
        m_tip = (const char*)(e->param());
        setTip(m_tip);
        break;
    case EventIconChanged:
        setIcon((bBlink && m_unread) ? m_unread : m_state);
        break;
    case EventLanguageChanged:
        setTip(m_tip);
        break;
    case EventQuit:
        quit();
        break;
#ifdef WIN32
    case EventShowError:{
            if (!m_bBalloon)
                return NULL;
            clientErrorData *data = (clientErrorData*)(e->param());
            if (data->id == 0)
                return NULL;
            for (list<BalloonItem>::iterator it = m_queue.begin(); it != m_queue.end(); ++it){
                if ((*it).id == data->id)
                    return e->param();
            }
            QString arg;
            if (data->args){
                arg = QString::fromUtf8(data->args);
                free(data->args);
            }
            BalloonItem item;
            item.id   = data->id;
            item.client = data->client;
            item.flags  = (data->flags & ERR_INFO) ? NIIF_INFO : NIIF_ERROR;
            item.text = i18n(data->err_str);
            if (item.text.find("%1") >= 0)
                item.text = item.text.arg(arg);
            if (!m_queue.empty()){
                m_queue.push_back(item);
                return e->param();
            }
            item.title = "SIM";
            if (getContacts()->nClients() > 1){
                for (unsigned i = 0; i < getContacts()->nClients(); i++){
                    if (getContacts()->getClient(i) == data->client){
                        item.title = getContacts()->getClient(i)->name().c_str();
                        int n = item.title.find(".");
                        if (n > 0)
                            item.title = item.title.left(n) + " " + item.title.mid(n + 1);
                    }
                }
            }
            m_queue.push_back(item);
            if (showBalloon())
                return e->param();
            return NULL;
        }
#endif
    }
    return NULL;
}

#ifdef WIN32

bool DockWnd::showBalloon()
{
    if (m_queue.empty())
        return false;
    BalloonItem &item = m_queue.front();

    NOTIFYICONDATAW notifyIconData;
    memset(&notifyIconData, 0, sizeof(notifyIconData));
    notifyIconData.cbSize   = sizeof(notifyIconData);
    notifyIconData.hWnd     = winId();
    notifyIconData.uFlags   = NIF_INFO;
    notifyIconData.uTimeout = TIP_TIMEOUT;
    unsigned i;
    unsigned size = item.text.length() + 1;
    if (size >= sizeof(notifyIconData.szInfo) / sizeof(wchar_t))
        size = sizeof(notifyIconData.szInfo) / sizeof(wchar_t) - 1;
    for (i = 0; i < size; i++)
        notifyIconData.szInfo[i] = item.text[(int)i].unicode();
    notifyIconData.szInfo[size] = 0;
    size = item.title.length() + 1;
    if (size >= sizeof(notifyIconData.szInfoTitle) / sizeof(wchar_t))
        size = sizeof(notifyIconData.szInfoTitle) / sizeof(wchar_t) - 1;
    for (i = 0; i < size; i++)
        notifyIconData.szInfoTitle[i] = item.title[(int)i].unicode();
    notifyIconData.szInfoTitle[size] = 0;
    notifyIconData.dwInfoFlags = item.flags;
    if (!Shell_NotifyIconW(NIM_MODIFY, (NOTIFYICONDATAW*)&notifyIconData)){
        m_queue.erase(m_queue.begin());
        return false;
    }
    return true;
}

#endif

#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)

bool DockWnd::x11Event(XEvent *e)
{
    if (e->type == ClientMessage){
        if (!inTray){
            Atom xembed_atom = XInternAtom( qt_xdisplay(), "_XEMBED", FALSE );
            if (e->xclient.message_type == xembed_atom){
                inTray = true;
                bInit = true;
                resize(22, 22);
                if (wharfIcon){
                    delete wharfIcon;
                    wharfIcon = NULL;
                }
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
            move(m_plugin->getDockX(), m_plugin->getDockY());
        }
    }
    return QWidget::x11Event(e);
}

#endif
#endif

void DockWnd::paintEvent( QPaintEvent* )
{
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (!bInit) return;
#endif
#endif
    QPainter p(this);
    p.drawPixmap((width() - drawIcon.width())/2, (height() - drawIcon.height())/2, drawIcon);
}

void DockWnd::setIcon(const char *icon)
{
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (wharfIcon){
        wharfIcon->set(m_state, bBlink ?  m_unread : NULL);
	repaint();
	return;
    }
#endif
#endif
    drawIcon = Pict(icon);
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (!inTray){
        repaint();
        return;
    }
#endif
#endif
#ifdef WIN32
    QWidget::setIcon(drawIcon);
    if (IsWindowUnicode(winId())){
        NOTIFYICONDATAW notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        notifyIconData.uFlags = NIF_ICON;
        Shell_NotifyIconW(NIM_MODIFY, &notifyIconData);
    }else{
        NOTIFYICONDATAA notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        notifyIconData.uFlags = NIF_ICON;
        Shell_NotifyIconA(NIM_MODIFY, &notifyIconData);
    }
#else
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (wharfIcon)
        return;
#endif
    // from PSI:
    // thanks to Robert Spier for this:
    // for some reason the repaint() isn't being honored, or isn't for
    // the icon.  So force one on the widget behind the icon
    erase();
    QPaintEvent pe( rect() );
    paintEvent(&pe);
#endif
}

void DockWnd::setTip(const char *text)
{
    m_tip = text;
    QString tip = m_unreadText;
    if (tip.isEmpty()){
        tip = i18n(text);
        tip = tip.replace(QRegExp("\\&"), "");
    }
#ifdef WIN32
    if (IsWindowUnicode(winId())){
        NOTIFYICONDATAW notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        unsigned size = tip.length();
        if (size >= sizeof(notifyIconData.szTip) / sizeof(wchar_t))
            size = sizeof(notifyIconData.szTip) / sizeof(wchar_t) - 1;
        memcpy(notifyIconData.szTip, tip.unicode(), size * sizeof(wchar_t));
        notifyIconData.uFlags = NIF_TIP;
        Shell_NotifyIconW(NIM_MODIFY, &notifyIconData);
    }else{
        NOTIFYICONDATAA notifyIconData;
        memset(&notifyIconData, 0, sizeof(notifyIconData));
        notifyIconData.cbSize = sizeof(notifyIconData);
        notifyIconData.hIcon = topData()->winIcon;
        notifyIconData.hWnd = winId();
        strncpy(notifyIconData.szTip, tip.local8Bit(), sizeof(notifyIconData.szTip));
        notifyIconData.uFlags = NIF_TIP;
        Shell_NotifyIconA(NIM_MODIFY, &notifyIconData);
    }
#else
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (wharfIcon == NULL){
#endif
            QToolTip::remove(this);
            QToolTip::add(this, tip);
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    }else{
        if (wharfIcon->isVisible()){
            QToolTip::remove(wharfIcon);
            QToolTip::add(wharfIcon, tip);
        }
    }
#endif
#endif
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
    case QWidget::MidButton:
        emit doubleClicked();
        break;
    default:
        break;
    }
}

void DockWnd::mousePressEvent( QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (inTray || wharfIcon) return;
#endif
    grabMouse();
    mousePos = e->pos();
#endif
}

void DockWnd::mouseReleaseEvent( QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (!inTray && (wharfIcon == NULL)){
#endif
        releaseMouse();
        if (!mousePos.isNull()){
            move(e->globalPos().x() - mousePos.x(),  e->globalPos().y() - mousePos.y());
            mousePos = QPoint();
            QPoint p(m_plugin->getDockX() - x(), m_plugin->getDockY() - y());
            m_plugin->setDockX(x());
            m_plugin->setDockY(y());
            if (p.manhattanLength() > 6)
                return;
        }
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    }
#endif
#endif
    mouseEvent(e);
}

void DockWnd::mouseMoveEvent( QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
#ifndef WIN32
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
    if (inTray || wharfIcon) return;
#endif
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
#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC)
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
#endif
}

void DockWnd::blink()
{
    if (m_unread == NULL){
        bBlink = false;
        blinkTimer->stop();
        setIcon(m_state);
        return;
    }
    bBlink = !bBlink;
    setIcon(bBlink ? m_unread : m_state);
}

typedef struct msgIndex
{
    unsigned	contact;
    unsigned	type;
} msgIndex;

bool operator < (const struct msgIndex &a, const struct msgIndex &b)
{
    if (a.contact < b.contact)
        return true;
    if (a.contact > b.contact)
        return false;
    return a.type < b.type;
}

typedef map<msgIndex, unsigned> MAP_COUNT;

void DockWnd::reset()
{
    m_unread = NULL;
    QString oldUnreadText = m_unreadText;
    m_unreadText = "";
    MAP_COUNT count;
    MAP_COUNT::iterator itc;
    for (list<msg_id>::iterator it = m_plugin->m_core->unread.begin(); it != m_plugin->m_core->unread.end(); ++it){
        if (m_unread == NULL){
            CommandDef *def = m_plugin->m_core->messageTypes.find((*it).type);
            if (def)
                m_unread = def->icon;
        }
        msgIndex m;
        m.contact = (*it).contact;
        m.type    = (*it).type;
        itc = count.find(m);
        if (itc == count.end()){
            count.insert(MAP_COUNT::value_type(m, 1));
        }else{
            (*itc).second++;
        }
    }
    if (!count.empty()){
        for (itc = count.begin(); itc != count.end(); ++itc){
            CommandDef *def = m_plugin->m_core->messageTypes.find((*itc).first.type);
            if (def == NULL)
                continue;
            MessageDef *mdef = (MessageDef*)(def->param);
            QString msg = i18n(mdef->singular, mdef->plural, (*itc).second);

            Contact *contact = getContacts()->contact((*itc).first.contact);
            if (contact == NULL)
                continue;
            msg = i18n("%1 from %2")
                  .arg(msg)
                  .arg(contact->getName());
#ifdef WIN32
            if (m_unreadText.length() + 2 + msg.length() >= 64){
                m_unreadText += "...";
                break;
            }
#endif

            if (!m_unreadText.isEmpty())
#ifdef WIN32
                m_unreadText += ", ";
#else
                m_unreadText += "\n";
#endif
            m_unreadText += msg;
        }
    }
    if (m_unread && !blinkTimer->isActive())
        blinkTimer->start(1500);
    if (m_unreadText != oldUnreadText)
        setTip(m_tip);
}

#ifndef NO_MOC_INCLUDES
#include "dockwnd.moc"
#endif
