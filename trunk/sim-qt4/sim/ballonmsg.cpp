/***************************************************************************
                          ballonmsg.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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

#include "ballonmsg.h"

#include <QTimer>
#include <QPainter>
#include <QToolTip>
#include <QBitmap>
#include <q3pointarray.h>
#include <QApplication>
#include <QPushButton>
#include <QLayout>
#include <QImage>
#include <QFrame>
#include <QCheckBox>
#include <QColorGroup>
#include <QPalette>
#include <q3simplerichtext.h>
#include <QPixmap>
#include <QPaintEvent>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <Q3StyleSheet>



#ifdef WIN32
#include <windows.h>
#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW   0x00020000
#endif
#endif

#define BALLOON_R			10
#define BALLOON_TAIL		20
#define BALLOON_TAIL_WIDTH	12
#define BALLOON_MARGIN		8
#define BALLOON_SHADOW_DEF	2

EXPORT QPixmap& intensity(QPixmap &pict, float percent)
{
    QImage image = pict.toImage();
    int i, tmp, r, g, b;
    int segColors = image.depth() > 8 ? 256 : image.numColors();
    unsigned char *segTbl = new unsigned char[segColors];
    int pixels = image.depth() > 8 ? image.width()*image.height() :
                 image.numColors();
   unsigned int *data = image.depth() > 8 ? (unsigned int *)image.bits() :
                 (unsigned int*)image.colorTable().size();

    bool brighten = (percent >= 0);
    if(percent < 0)
        percent = -percent;

    if(brighten){ // keep overflow check out of loops
        for(i=0; i < segColors; ++i){
            tmp = (int)(i*percent);
            if(tmp > 255)
                tmp = 255;
            segTbl[i] = (unsigned char)tmp;
        }
    }
    else{
        for(i=0; i < segColors; ++i){
            tmp = (int)(i*percent);
            if(tmp < 0)
                tmp = 0;
            segTbl[i] = (unsigned char)tmp;
        }
    }

    if(brighten){ // same here
        for(i=0; i < pixels; ++i){
            r = qRed(data[i]);
            g = qGreen(data[i]);
            b = qBlue(data[i]);
            r = r + segTbl[r] > 255 ? 255 : r + segTbl[r];
            g = g + segTbl[g] > 255 ? 255 : g + segTbl[g];
            b = b + segTbl[b] > 255 ? 255 : b + segTbl[b];
            data[i] = qRgb(r, g, b);
        }
    }
    else{
        for(i=0; i < pixels; ++i){
            r = qRed(data[i]);
            g = qGreen(data[i]);
            b = qBlue(data[i]);
            r = r - segTbl[r] < 0 ? 0 : r - segTbl[r];
            g = g - segTbl[g] < 0 ? 0 : g - segTbl[g];
            b = b - segTbl[b] < 0 ? 0 : b - segTbl[b];
            data[i] = qRgb(r, g, b);
        }
    }
    delete [] segTbl;

    pict.fromImage(image);
    return pict;
}

BalloonMsg::BalloonMsg(void *param, const QString &_text, QStringList &btn, QWidget *parent, const QRect *rcParent,
                       bool bModal, bool bAutoHide, unsigned bwidth, const QString &box_msg, bool *bChecked)
        : QDialog(parent,
		(bAutoHide ? Qt::Popup : Qt::Window | Qt::WindowStaysOnTopHint)
                  | Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint)
 {
	setAttribute( Qt::WA_DeleteOnClose );
    this->setModal(bModal);
	m_param = param;
    m_parent = parent;
    m_width = bwidth;
    m_bAutoHide = bAutoHide;
    m_bYes = false;
    m_bChecked = bChecked;
    bool bTailDown = true;
    setPalette(QToolTip::palette());
    text = _text;
    Q3Frame *frm = new Q3Frame(this);
    frm->setPalette(palette());
    QVBoxLayout *vlay = new QVBoxLayout(frm);
    vlay->setMargin(0);
    m_check = NULL;
    if (!box_msg.isEmpty()){
        m_check = new QCheckBox(box_msg, frm);
        vlay->addWidget(m_check);
        if (m_bChecked)
            m_check->setChecked(*m_bChecked);
    }
    QHBoxLayout *lay = new QHBoxLayout(vlay);
    lay->setSpacing(5);
    lay->addStretch();
    unsigned id = 0;
    bool bFirst = true;
    for (QStringList::Iterator it = btn.begin(); it != btn.end(); ++it, id++){
        BalloonButton *b = new BalloonButton(*it, frm, id);
        connect(b, SIGNAL(action(int)), this, SLOT(action(int)));
        lay->addWidget(b);
        if (bFirst){
            b->setDefault(true);
            bFirst = false;
        }
    }
    setButtonsPict(this);
    lay->addStretch();
    int wndWidth = frm->minimumSizeHint().width();
    int hButton  = frm->minimumSizeHint().height();

    int txtWidth = bwidth;
    QRect rc;
    if (rcParent){
        rc = *rcParent;
    }else{
        QPoint p = parent->mapToGlobal(parent->rect().topLeft());
        rc = QRect(p.x(), p.y(), parent->width(), parent->height());
    }
    if (rc.width() > txtWidth) txtWidth = rc.width();

    Q3SimpleRichText richText(_text, font(), "", Q3StyleSheet::defaultSheet(), Q3MimeSourceFactory::defaultFactory(), -1, Qt::blue, false);
    richText.setWidth(wndWidth);
    richText.adjustSize();
    QSize s(richText.widthUsed(), richText.height());
    QSize sMin = frm->minimumSizeHint();
    if (s.width() < sMin.width())
        s.setWidth(sMin.width());
    int BALLOON_SHADOW = BALLOON_SHADOW_DEF;
#ifdef WIN32
    /* FIXME */
    /*    if ((GetClassLong(winId(), GCL_STYLE) & CS_DROPSHADOW) && style().inherits("QWindowsXPStyle"))
            BALLOON_SHADOW = 0; */
#endif
    resize(s.width() + BALLOON_R * 2 + BALLOON_SHADOW,
           s.height() + BALLOON_R * 2 + BALLOON_TAIL + BALLOON_SHADOW + hButton + BALLOON_MARGIN);
    mask = QBitmap(width(), height());
    int w = width() - BALLOON_SHADOW;
    int tailX = w / 2;
    int posX = rc.left() + rc.width() / 2 + BALLOON_TAIL_WIDTH - tailX;
    if (posX <= 0) posX = 1;
    QRect rcScreen = screenGeometry();
    if (posX + width() >= rcScreen.width())
        posX = rcScreen.width() - 1 - width();
    int tx = posX + tailX - BALLOON_TAIL_WIDTH;
    if (tx < rc.left()) tx = rc.left();
    if (tx > rc.left() + rc.width()) tx = rc.left() + rc.width();
    tailX = tx + BALLOON_TAIL_WIDTH - posX;
    if (tailX < BALLOON_R) tailX = BALLOON_R;
    if (tailX > width() - BALLOON_R - BALLOON_TAIL_WIDTH) tailX = width() - BALLOON_R - BALLOON_TAIL_WIDTH;
    if (rc.top() <= height() + 2){
        bTailDown = false;
        move(posX, rc.top() + rc.height() + 1);
    }else{
        move(posX, rc.top() - height() - 1);
    }
    int pos = 0;
    int h = height() - BALLOON_SHADOW - BALLOON_TAIL;
    if (!bTailDown) pos += BALLOON_TAIL;
    textRect.setRect(BALLOON_R, pos + BALLOON_R, w - BALLOON_R * 2, h);
    frm->resize(s.width(), hButton);
    frm->move(BALLOON_R, pos + h - BALLOON_R - hButton);
    QPainter p;
    p.begin(&mask);
#ifdef WIN32
    QColor bg(255, 255, 255);
    QColor fg(0, 0, 0);
#else
    QColor bg(0, 0, 0);
    QColor fg(255, 255, 255);
#endif
    p.fillRect(0, 0, width(), height(), bg);
    p.fillRect(0, pos + BALLOON_R, w, h - BALLOON_R * 2, fg);
    p.fillRect(BALLOON_R, pos, w - BALLOON_R * 2, h, fg);
    p.fillRect(BALLOON_SHADOW, pos + BALLOON_R + BALLOON_SHADOW, w, h - BALLOON_R * 2, fg);
    p.fillRect(BALLOON_R + BALLOON_SHADOW, pos + BALLOON_SHADOW, w - BALLOON_R * 2, h, fg);
    p.setBrush(fg);
    p.drawEllipse(0, pos, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2, pos, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2, pos + h - BALLOON_R * 2, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(0, pos + h - BALLOON_R * 2, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(BALLOON_SHADOW, pos + BALLOON_SHADOW, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2 + BALLOON_SHADOW, pos + BALLOON_SHADOW, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2 + BALLOON_SHADOW, pos + h - BALLOON_R * 2 + BALLOON_SHADOW, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(BALLOON_SHADOW, pos + h - BALLOON_R * 2 + BALLOON_SHADOW, BALLOON_R * 2, BALLOON_R * 2);
    Q3PointArray arr(3);
    arr.setPoint(0, tailX, bTailDown ? h : pos);
    arr.setPoint(1, tailX + BALLOON_TAIL_WIDTH, bTailDown ? h : pos);
    arr.setPoint(2, tailX - BALLOON_TAIL_WIDTH, bTailDown ? height() - BALLOON_SHADOW : 0);
    p.drawPolygon(arr);
    arr.setPoint(0, tailX + BALLOON_SHADOW, (bTailDown ? h : pos) + BALLOON_SHADOW);
    arr.setPoint(1, tailX + BALLOON_TAIL_WIDTH + BALLOON_SHADOW, (bTailDown ? h : pos) + BALLOON_SHADOW);
    arr.setPoint(2, tailX - BALLOON_TAIL_WIDTH + BALLOON_SHADOW, bTailDown ? height() : BALLOON_SHADOW);
    p.drawPolygon(arr);
    p.end();
    setMask(mask);
    qApp->syncX();
    QPixmap pict = QPixmap::grabWindow(QApplication::desktop()->winId(), x(), y(), width(), height());
    intensity(pict, -0.50f);
    p.begin(&pict);
    p.setBrush(this->palette().background());
    p.drawEllipse(0, pos, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2, pos, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(w - BALLOON_R * 2, pos + h - BALLOON_R * 2, BALLOON_R * 2, BALLOON_R * 2);
    p.drawEllipse(0, pos + h - BALLOON_R * 2, BALLOON_R * 2, BALLOON_R * 2);
    arr.setPoint(0, tailX, bTailDown ? h - 1 : pos + 1);
    arr.setPoint(1, tailX + BALLOON_TAIL_WIDTH, bTailDown ? h - 1 : pos + 1);
    arr.setPoint(2, tailX - BALLOON_TAIL_WIDTH, bTailDown ? height() - BALLOON_SHADOW : 0);
    p.drawPolygon(arr);
    p.fillRect(0, pos + BALLOON_R, w, h - BALLOON_R * 2, this->palette().background());
    p.fillRect(BALLOON_R, pos, w - BALLOON_R * 2, h, this->palette().background());
    p.drawLine(0, pos + BALLOON_R, 0, pos + h - BALLOON_R);
    p.drawLine(w - 1, pos + BALLOON_R, w - 1, pos + h - BALLOON_R);
    if (bTailDown){
        p.drawLine(BALLOON_R, 0, w - BALLOON_R, 0);
        p.drawLine(BALLOON_R, h - 1, tailX, h - 1);
        p.drawLine(tailX + BALLOON_TAIL_WIDTH, h - 1, w - BALLOON_R, h - 1);
    }else{
        p.drawLine(BALLOON_R, pos + h - 1, w - BALLOON_R, pos + h - 1);
        p.drawLine(BALLOON_R, pos, tailX, pos);
        p.drawLine(tailX + BALLOON_TAIL_WIDTH, pos, w - BALLOON_R, pos);
    }
    p.end();
    this->setBackgroundPixmap(pict);
    if (!bAutoHide)
        setFocusPolicy(Qt::NoFocus);

    QWidget *top = NULL;
    if (parent)
        top = parent->topLevelWidget();
    if (top){
        raiseWindow(top);
        top->installEventFilter(this);
    }
}

BalloonMsg::~BalloonMsg()
{
    if (!m_bYes)
        emit no_action(m_param);
    emit finished();
}

bool BalloonMsg::isChecked()
{
    if (m_check)
        return m_check->isChecked();
    return false;
}

bool BalloonMsg::eventFilter(QObject *o, QEvent *e)
{
    if ((e->type() == QEvent::Hide) && (o == m_parent->topLevelWidget()))
        return true;
    return QDialog::eventFilter(o, e);
}

void BalloonMsg::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    Q3SimpleRichText richText(text, font(), "", Q3StyleSheet::defaultSheet(), Q3MimeSourceFactory::defaultFactory(), -1, Qt::blue, false);
    richText.setWidth(m_width);
    richText.adjustSize();
    richText.draw(&p,
		          (width() - textRect.width()) / 2,
	              textRect.y(),
	              QRect(0, 0, width(), height()),
                  QToolTip::palette().active() );
    p.end();
}

void BalloonMsg::mousePressEvent(QMouseEvent *e)
{
    if (m_bAutoHide && rect().contains(e->pos())){
        QImage img = mask.toImage();
        QRgb rgb = img.pixel(e->pos().x(), e->pos().y());
        if (rgb & 0xFFFFFF)
            QTimer::singleShot(10, this, SLOT(close()));
    }
    QWidget::mousePressEvent(e);
}

void BalloonMsg::action(int id)
{
    if (m_bChecked && m_check)
        *m_bChecked = m_check->isChecked();
    emit action(id, m_param);
    if (id == 0){
        emit yes_action(m_param);
        m_bYes = true;
    }
}

void BalloonMsg::message(const QString &text, QWidget *parent, bool bModal, unsigned width, const QRect *rc)
{
    QStringList btns;
    btns.append(i18n("&Ok"));
    BalloonMsg *msg = new BalloonMsg(NULL, QString("<center>") + quoteString(text) + "</center>", btns, parent, rc, bModal, true, width);
    if (bModal){
        msg->exec();
    }else{
        msg->show();
    }
}

void BalloonMsg::ask(void *param, const QString &text, QWidget *parent,
                     const char *slotYes, const char *slotNo,
                     const QRect *rc, QObject *receiver,
                     const QString &checkText, bool *bCheck)
{
    QStringList btns;
    btns.append(i18n("&Yes"));
    btns.append(i18n("&No"));
    BalloonMsg *msg = new BalloonMsg(param, QString("<center>") + quoteString(text) + "</center>", btns, parent, rc, false, true, 300, checkText, bCheck);
    if (receiver == NULL)
        receiver = parent;
    if (slotYes)
        connect(msg, SIGNAL(yes_action(void*)), receiver, slotYes);
    if (slotNo)
        connect(msg, SIGNAL(no_action(void*)), receiver, slotNo);
    msg->show();
}

BalloonButton::BalloonButton(QString string, QWidget *parent, int _id)
        : QPushButton(string, parent)
{
    id = _id;
    setPalette(parent->palette());
    resize(sizeHint());
    connect(this, SIGNAL(clicked()), this, SLOT(click()));
}

void BalloonButton::click()
{
    topLevelWidget()->hide();
    emit action(id);
    topLevelWidget()->close();
}

