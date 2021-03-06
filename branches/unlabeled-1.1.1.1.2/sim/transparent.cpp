/***************************************************************************
                          transparent.cpp  -  description
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

#include "transparent.h"
#include "mainwin.h"
#include "log.h"

#include <qobjectlist.h>
#include <qlistview.h>
#if QT_VERSION < 300
#include "qt3/qtextbrowser.h"
#else
#include <qtextbrowser.h>
#endif
#include <qpainter.h>

#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
#include <krootpixmap.h>
#include "ui/effect.h"
#include <qimage.h>
#endif

#ifdef WIN32
#include <windowsx.h>
#endif

char _TRANSPARENT[] = "transparent";

TransparentBg::TransparentBg(QWidget *p, const char *name)
        : QObject(p, name)
{
    if (p->inherits("QTextEdit")){
        QTextEdit *parent = static_cast<QTextEdit*>(p);
        parent->viewport()->installEventFilter(this);
    }
    init(p);
}

void TransparentBg::init(QWidget *p)
{
    QObject *obj = p->topLevelWidget()->child(_TRANSPARENT);
    if (obj == NULL) return;
    bgX = bgY = 0;
    connect(obj, SIGNAL(backgroundUpdated()), this, SLOT(backgroundUpdated()));
}

const QPixmap *TransparentBg::background(QColor bgColor)
{
    QWidget *p = static_cast<QWidget*>(parent());
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    TransparentTop *top = TransparentTop::getTransparent(p->topLevelWidget());
    if (top)
        return top->background(bgColor);
#endif
    const QPixmap *res = p->topLevelWidget()->backgroundPixmap();
    if (res && !res->isNull()) return res;
    return NULL;
}

bool TransparentBg::eventFilter(QObject*, QEvent *e)
{
    if ((e->type() ==  QEvent::Show) || (e->type() == QEvent::Resize) || (e->type() == QEvent::User)){
        backgroundUpdated();
    }else if (e->type() == QEvent::Paint){
        QTextEdit *text = static_cast<QTextEdit*>(parent());
        QPoint pp = text->viewportToContents(QPoint(0, 0));
        if ((bgX != pp.x()) || (bgY != pp.y()))
            backgroundUpdated();
    }
    return false;
}

void TransparentBg::backgroundUpdated()
{
    if (parent()->inherits("QTextEdit")){
        QTextEdit *text = static_cast<QTextEdit*>(parent());
        const QPixmap *pix = background(text->colorGroup().color(QColorGroup::Base));
        QPoint pp = text->viewportToContents(QPoint(0, 0));
        bgX = pp.x();
        bgY = pp.y();
        if ((pix == NULL) || pix->isNull()){
            if (text->paper().pixmap() && !text->paper().pixmap()->isNull()){
                text->setPaper(QBrush(text->colorGroup().base()));
                text->setStaticBackground(false);
            }
            return;
        }
        QPoint pp1 = text->topLevelWidget()->mapFromGlobal(text->mapToGlobal(QPoint(0, 0)));
        QPixmap bg(bgX + text->width(), bgY + text->height());
        QPainter p;
        p.begin(&bg);
        p.drawTiledPixmap(bgX, bgY, text->width(), text->height(), *pix, pp1.x(), pp1.y());
        p.end();
        text->setPaper(QBrush(text->colorGroup().background(), bg));
        text->setStaticBackground(true);
        text->setBackgroundMode(QWidget::NoBackground);
        text->viewport()->setBackgroundMode(QWidget::NoBackground);
        return;
    }
    if (parent()->inherits("QListView")){
        QListView *p = static_cast<QListView*>(parent());
        p->viewport()->repaint();
    }
}

TransparentTop::TransparentTop(QWidget *parent,
                               ConfigBool &_useTransparent, ConfigULong &_transparent)
        : QObject(parent, _TRANSPARENT),
        useTransparent(_useTransparent),
        transparent(_transparent)
{
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    rootpixmap = new KRootPixmap(parent);
#endif
    QObjectList *l = parent->queryList("TransparentBg");
    QObjectListIt it(*l);
    QObject *obj;
    while ((obj = it.current()) != NULL){
        TransparentBg *transp = static_cast<TransparentBg*>(obj);
        transp->init(parent);
        ++it;
    }
    connect(pMain, SIGNAL(transparentChanged()), this, SLOT(transparentChanged()));
    transparentChanged();
}

void TransparentTop::updateBackground(const QPixmap &pm)
{
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    saveBG = pm;
    genBG = QPixmap();
    genFade = 0;
    emit backgroundUpdated();
#endif
}

void TransparentTop::transparentChanged()
{
#ifdef WIN32
    QWidget *p = static_cast<QWidget*>(parent());
    setTransparent(p,useTransparent, transparent);
#endif
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    if (useTransparent()){
        rootpixmap->start();
    }else{
        rootpixmap->stop();
        saveBG = QPixmap();
        genBG = QPixmap();
        genFade = 0;
        emit backgroundUpdated();
    }
#endif
}

TransparentTop *TransparentTop::getTransparent(QWidget *w)
{
    QObject *transparent = w->topLevelWidget()->child(_TRANSPARENT);
    if (transparent)
        return static_cast<TransparentTop*>(transparent);
    return NULL;
}

bool TransparentTop::bCanTransparent;

class Transparency
{
public:
    Transparency();
};

Transparency transparency;

#ifdef WIN32

typedef BOOL WINAPI FN_SETLAYEREDWINDOWATTRIBUTES (
    HWND hwnd,
    COLORREF crKey,
    BYTE bAlpha,
    DWORD dwFlags);

static FN_SETLAYEREDWINDOWATTRIBUTES *SetLayeredWindowAttributes = NULL;

#define WS_EX_LAYERED           0x00080000
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002

#endif

Transparency::Transparency()
{
    TransparentTop::bCanTransparent = false;
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    TransparentTop::bCanTransparent = true;
#endif
#ifdef WIN32
    HINSTANCE hLib = LoadLibraryA("user32.dll");
    if (hLib != NULL)
        (DWORD&)SetLayeredWindowAttributes = (DWORD)GetProcAddress(hLib,"SetLayeredWindowAttributes");
    if ((DWORD)SetLayeredWindowAttributes != 0)
        TransparentTop::bCanTransparent = true;
#endif
}

void TransparentTop::setTransparent(
#ifdef WIN32
    QWidget *w, bool isTransparent, unsigned long transparency
#else
    QWidget*, bool, unsigned long
#endif
)
{
#ifdef WIN32
    if (!bCanTransparent) return;
    if (isTransparent){
        BYTE d = QMIN(transparency * 256 / 100, 255);
        SetWindowLongW(w->winId(), GWL_EXSTYLE, GetWindowLongW(w->winId(), GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(w->winId(), w->colorGroup().background().rgb(), d, LWA_ALPHA);
    }else{
        SetWindowLongW(w->winId(), GWL_EXSTYLE, GetWindowLongW(w->winId(), GWL_EXSTYLE) & (~WS_EX_LAYERED));
    }
#endif
}

const QPixmap *TransparentTop::background(QColor c)
{
#if defined(USE_KDE) && defined(HAVE_KROOTPIXMAP_H)
    if (useTransparent() && !saveBG.isNull()){
        if ((c.rgb() == genColor.rgb()) && (transparent() == genFade))
            return &genBG;
        QImage img = saveBG.convertToImage();
        fade(img, transparent() / 100., c);
        genBG.convertFromImage(img);
        genFade = transparent();
        genColor = c;
        return &genBG;
    }
#endif
    return NULL;
}

#ifndef _WINDOWS
#include "transparent.moc"
#endif
