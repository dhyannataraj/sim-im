/***************************************************************************
                          userview.cpp  -  description
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

#include "userview.h"
#include "cuser.h"
#include "mainwin.h"
#include "client.h"
#include "icons.h"
#include "intedit.h"
#include "history.h"
#include "transparent.h"
#include "log.h"
#include "ui/ballonmsg.h"
#include "ui/toolsetup.h"

#include <qheader.h>
#include <qpopupmenu.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qbutton.h>
#include <qobjectlist.h>
#include <qstringlist.h>
#include <qaccel.h>

#include <stdio.h>

#ifdef USE_KDE
#include <kwin.h>
#include <kpopupmenu.h>
#else
#include "ui/kpopup.h"
#endif

#ifndef WIN32
#include <X11/Xlib.h>
#endif

#if QT_VERSION < 300
#define CHECK_OFF	QButton::Off
#define CHECK_ON	QButton::On
#define CHECK_NOCHANGE	QButton::NoChange
#else
#define CHECK_OFF	QStyle::Style_Off
#define CHECK_ON	QStyle::Style_On
#define CHECK_NOCHANGE	QStyle::Style_NoChange
#endif

const unsigned STATE_ONLINE = 0;
const unsigned STATE_OFFLINE = 1;
const unsigned STATE_NOTINLIST = 2;

UserViewItemBase::UserViewItemBase(UserView *parent)
        : QListViewItem(parent)
{
}

UserViewItemBase::UserViewItemBase(UserViewItemBase *parent)
        : QListViewItem(parent)
{
}

QString UserViewItemBase::key(int, bool) const
{
    return text(1);
}

void UserViewItemBase::paintFocus(QPainter*, const QColorGroup&, const QRect & )
{
}

void UserViewItemBase::paint(QPainter *p, const QString s, const QColorGroup &c, bool bSeparator, bool bEnabled, int *pW, int *pWidth)
{
    QColorGroup cg(c);
    int h = height();
    int width = listView()->width();
    const QScrollBar *vBar = listView()->verticalScrollBar();
    if (vBar && vBar->isVisible()) width -= vBar->width();
    UserView *userView = static_cast<UserView*>(listView());
    const QPixmap *pix = userView->transparent->background(cg.base());
    bool bSelected = isSelected() && !userView->bFloaty;
    if (bEnabled){
        p->setPen(pMain->UseSystemColors ?
                  cg.color(QColorGroup::Text) :
                  QColor(pMain->OnlineColor));
    }else{
        p->setPen(pMain->UseSystemColors ?
                  cg.color(QColorGroup::Dark) :
                  QColor(pMain->OfflineColor));
    }

    const QImage &bgPict = userView->bgPict;
    int w = 0;
    int ch = height();
    bool bTile = false;
    int cx = 0;
    int cy = 0;
    bool bStaticBg = false;
    QPoint pos = listView()->itemRect(this).topLeft();
    pos = listView()->viewport()->mapToParent(pos);
    if (!bgPict.isNull()){
        switch (pMain->BackgroundMode){
        case bgModeContactLeft:
            w = bgPict.width();
            break;
        case bgModeContactScale:
            w = width;
            bStaticBg = true;
            break;
        case bgModeWndTop:
            w = bgPict.width();
            ch = bgPict.height();
            cy = pos.y();
            bTile = true;
            break;
        case bgModeWndBottom:
            w = bgPict.width();
            ch = bgPict.height();
            cy = pos.y() + (bgPict.height() - userView->height());
            bTile = true;
            bStaticBg = true;
            break;
        case bgModeWndCenter:
            w = bgPict.width();
            ch = bgPict.height();
            cy = pos.y() + (bgPict.height() - userView->height()) / 2;
            bTile = true;
            bStaticBg = true;
            break;
        case bgModeWndScale:
            w = width;
            ch = userView->height();
            cy = pos.y();
            bStaticBg = true;
            break;
        }
    }
    int xp = 0;
    int yp = 0;
    if (bSelected){
        xp = 2;
        yp = 2;
    }
    if (w){
        int wImg = w;
        if (bTile) w = width;
        if (bgPict.hasAlphaBuffer()){
            QPixmap bPict(w, height());
            QPainter pp(&bPict);
            if (pix != NULL){
                QPoint gPos = listView()->mapToGlobal(pos);
                gPos = listView()->topLevelWidget()->mapFromGlobal(gPos);
                pp.drawTiledPixmap(0, 0, w, h, *pix, gPos.x(), gPos.y());
            }else{
                pp.fillRect(0, 0, w, h, cg.base());
            }
            userView->drawImage(&pp, 0, 0, w, height(), cx, cy, wImg, ch);
            pp.end();
            p->drawPixmap(xp, yp, bPict);
        }else{
            userView->drawImage(p, xp, yp, w, height(), cx, cy, wImg, ch);
        }
        if (w < width){
            if (pix != NULL){
                QPoint gPos = listView()->mapToGlobal(QPoint(w, pos.y()));
                gPos = listView()->topLevelWidget()->mapFromGlobal(gPos);
                p->drawTiledPixmap(w + xp, yp, width - w, h, *pix, gPos.x(), gPos.y());
            }else{
                p->fillRect(w + xp, yp, width - w, h, cg.base());
            }
        }
    }else{
        if (pix != NULL){
            QPoint pos = listView()->itemRect(this).topLeft();
            pos = listView()->viewport()->mapToParent(pos);
            pos = listView()->mapToGlobal(pos);
            pos = listView()->topLevelWidget()->mapFromGlobal(pos);
            p->drawTiledPixmap(xp, yp, width, h, *pix, pos.x(), pos.y());
        }else{
            p->fillRect( 0, 0, width, height(), cg.base());
        }
    }
    listView()->setStaticBackground(userView->bStaticBg || bStaticBg ||
                                    (pix && (listView()->contentsHeight() >= listView()->viewport()->height())));
    int x = pMain->IconMargin;
    if (userView->bList){
#if QT_VERSION < 300
        QSize s = listView()->style().indicatorSize();
        if (!bSeparator) x += s.width() / 2;
        QPixmap pixInd(s.width(), s.height());
        QPainter pInd(&pixInd);
        listView()->style().drawIndicator(&pInd, 0, 0,
                                          s.width(), s.height(),
                                          cg, text(3).toInt());
        pInd.end();
        QBitmap mInd(s.width(), s.height());
        pInd.begin(&mInd);
        listView()->style().drawIndicatorMask(&pInd, 0, 0,
                                              s.width(), s.height(),
                                              text(3).toInt());
        pInd.end();
        pixInd.setMask(mInd);
        p->drawPixmap(x, (height() - s.height()) / 2, pixInd);
        x += s.width() + 5;
#else
        int w = listView()->style().pixelMetric(QStyle::PM_IndicatorWidth);
        int h = listView()->style().pixelMetric(QStyle::PM_IndicatorHeight);
        QRect rc(x, (height() - h) / 2, w, h);
        listView()->style().drawPrimitive(QStyle::PE_Indicator, p, rc, cg, text(3).toInt());
        x += w + 5;
#endif
        if (bEnabled){
            p->setPen(pMain->UseSystemColors ?
                      cg.color(QColorGroup::Text) :
                      QColor(pMain->OnlineColor));
        }else{
            p->setPen(pMain->UseSystemColors ?
                      cg.color(QColorGroup::Dark) :
                      QColor(pMain->OfflineColor));
        }
    }else{
        QString pict = text(2);
        if (pict.length()){
            const QPixmap &icon = Pict(pict);
            p->drawPixmap(pMain->IconMargin + xp, (height() - icon.height()) + yp / 2, icon);
            x += icon.width() + 5;
        }
    }
    QRect br;
    p->drawText(x + xp, yp, width - x, height(), AlignLeft | AlignVCenter, s, -1, &br);
    x = br.right() + 5;

    if (bSeparator && (x < width - 6)){
#if QT_VERSION > 300
        QRect rcSep(x + xp, height()/2 + yp, width-6-x, 1);
        listView()->style().drawPrimitive(QStyle::PE_Separator, p, rcSep, cg);
#else
        listView()->style().drawSeparator(p, x + xp, height() / 2 + yp, width - 6, h / 2, cg);
#endif
    }
    if (pW) *pW = x;
    if (pWidth) *pWidth = width - 6;
    if (bSelected){
        p->setPen(cg.color(QColorGroup::Text));
        p->moveTo(0, h - 1);
        p->lineTo(0, 0);
        p->lineTo(width - 4, 0);
        p->setPen(cg.color(QColorGroup::Dark));
        p->moveTo(width - 3, h - 1);
        p->lineTo(1, h - 1);
        p->lineTo(1, 1);
        p->lineTo(width - 3, 1);
    }
}

UserViewItem::UserViewItem(ICQUser *u, UserView *parent)
        : UserViewItemBase(parent)
{
    m_itemState = 0;
    update(u, true);
    UserView *users = static_cast<UserView*>(listView());
    if (users->bList) setText(3, QString::number(CHECK_OFF));
#if QT_VERSION >= 300    
    setDragEnabled(!users->bList);
#endif
}

UserViewItem::UserViewItem(ICQUser *u, UserViewItemBase *parent)
        : UserViewItemBase(parent)
{
    m_itemState = 0;
    update(u, true);
    UserView *users = static_cast<UserView*>(listView());
    if (users->bList) setText(3, QString::number(CHECK_OFF));
#if QT_VERSION >= 300
    setDragEnabled(!users->bList);
#endif
}

int UserViewItem::width(const QFontMetrics&, const QListView *lv, int) const
{
    QFont f(listView()->font());
    f.setStrikeOut(m_bStrikeOut);
    f.setItalic(m_bItalic);
    f.setUnderline(m_bUnderline);
    QPainter p(lv);
    p.setFont(f);
    int w = p.boundingRect(20, 0, 10000, height(), AlignLeft | AlignVCenter, text(0)).right() + 8;
    if (m_bInvisible) w += Pict("invisible").width() + 2;
    if (m_bBirthday) w += Pict("birthday").width() + 2;
    if (m_bMobile) w += Pict("cell").width() + 2;
    if (m_bPhone) w += Pict("phone").width() + 2;
    if (m_bPhoneBusy) w += Pict("nophone").width() + 2;
    if (m_bPager) w += Pict("wpager").width() + 2;
    return w;
}

void UserViewItem::paintCell(QPainter *p, const QColorGroup &cg, int, int, int)
{
    QFont f(listView()->font());
    f.setStrikeOut(m_bStrikeOut);
    f.setItalic(m_bItalic);
    f.setUnderline(m_bUnderline);
    f.setBold(nBlink);
    p->setFont(f);
    int w, width;
    int status = m_status & 0xFF;
    int bNormal = (status == ICQ_STATUS_ONLINE) || (status == ICQ_STATUS_FREEFORCHAT);
    if (nBlink) bNormal = ((nBlink & 1) != 0);
    paint(p, text(0), cg, false, bNormal, &w, &width);
    UserView *userView = static_cast<UserView*>(listView());
    if (userView->bList) return;
    bool bSelected = isSelected() && !userView->bFloaty;
    int dx = 0;
    int dy = 0;
    if (bSelected){
        dx = 2;
        dy = 2;
    }
    if (m_bSecure){
        const QPixmap &pSecure = Pict("encrypted");
        if (width - pSecure.width() >= w){
            width -= pSecure.width();
            p->drawPixmap(width + dx, (height() - pSecure.height()) / 2 + dy, pSecure);
            width -= 2;
        }
    }
    if (m_bMobile){
        const QPixmap &pCell = Pict("cell");
        if (width - pCell.width() >= w){
            width -= pCell.width();
            p->drawPixmap(width + dx, (height() - pCell.height()) / 2 + dy, pCell);
            width -= 2;
        }
    }
    if (m_bPager){
        const QPixmap &pPager = Pict("wpager");
        if (width - pPager.width() >= w){
            width -= pPager.width();
            p->drawPixmap(width + dx, (height() - pPager.height()) / 2 + dy, pPager);
            width -= 2;
        }
    }
    if (m_bPhone){
        const QPixmap &pPhone = Pict("phone");
        if (width - pPhone.width() >= w){
            width -= pPhone.width();
            p->drawPixmap(width + dx, (height() - pPhone.height()) / 2 + dy, pPhone);
            width -= 2;
        }
    }
    if (m_bPhoneBusy){
        const QPixmap &pPhoneBusy = Pict("nophone");
        if (width - pPhoneBusy.width() >= w){
            width -= pPhoneBusy.width();
            p->drawPixmap(width + dx, (height() - pPhoneBusy.height()) / 2 + dy, pPhoneBusy);
            width -= 2;
        }
    }
    if (m_bBirthday){
        const QPixmap &pBirthday = Pict("birthday");
        if (width - pBirthday.width() >= w){
            width -= pBirthday.width();
            p->drawPixmap(width + dx, (height() - pBirthday.height()) / 2 + dy, pBirthday);
            width -= 2;
        }
    }
    if (m_bInvisible){
        const QPixmap &pInvisible = Pict("invisible");
        if (width - pInvisible.width() >= w){
            width -= pInvisible.width();
            p->drawPixmap(width + dx, (height() - pInvisible.height()) / 2 + dy, pInvisible);
            width -= 2;
        }
    }
}

void UserViewItem::update(ICQUser *u, bool bFirst)
{
    UserView *users = static_cast<UserView*>(listView());
    CUser user(u);
    if (!bFirst && ((u->uStatus == ICQ_STATUS_OFFLINE) != (m_status == ICQ_STATUS_OFFLINE))){
        if (parent() && (m_status != ICQ_STATUS_OFFLINE))
            static_cast<GroupViewItem*>(parent())->changeCounter(false);
        users->decStateCount(m_itemState);
        bFirst = true;
    }
    if (u->uStatus != ICQ_STATUS_OFFLINE){
        m_bInvisible = ((u->uStatus & ICQ_STATUS_FxPRIVATE) != 0);
        m_bBirthday = ((u->uStatus & ICQ_STATUS_FxBIRTHDAY) != 0);
        m_itemState = STATE_ONLINE;
    }else{
        m_bInvisible = m_bBirthday = false;
        m_itemState = STATE_OFFLINE;
    }
    if (u->GrpId == 0) m_itemState = STATE_NOTINLIST;
    m_bMobile = false;
    m_bPhone = false;
    m_bPhoneBusy = false;
    m_bPager = false;
    m_bSecure = u->isSecure();
    for (PhoneBook::iterator it = u->Phones.begin(); it != u->Phones.end(); it++){
        PhoneInfo *phone = static_cast<PhoneInfo*>(*it);
        if (phone->Type == SMS) m_bMobile = true;
        if (phone->Type == PAGER) m_bPager = true;
        if (phone->Active){
            if (u->PhoneState == 2) m_bPhoneBusy = true;
            if (u->PhoneState == 1) m_bPhone = true;
        }
    }
    m_bItalic = (u->VisibleId != 0);
    m_bUnderline = !pClient->BypassAuth && u->WaitAuth;
    m_bStrikeOut = (u->InvisibleId != 0);
    if (bFirst){
        if (parent() && (u->uStatus != ICQ_STATUS_OFFLINE))
            static_cast<GroupViewItem*>(parent())->changeCounter(true);
        users->incStateCount(m_itemState);
    }
    unsigned short msgType = 0;
    if (u->unreadMsgs.size()){
        unsigned long msgId = u->unreadMsgs.back();
        History h(u->Uin);
        ICQMessage *msg = h.getMessage(msgId);
        if (msg){
            msgType = msg->Type();
            if (msg->Id < MSG_PROCESS_ID) delete msg;
        }
    }
    QString name = user.name();
    setText(0, name);
    const char *userIcon = SIMClient::getUserIcon(u);
    if (m_bInvisible && ((u->uStatus & 0xFF) == ICQ_STATUS_ONLINE)){
        m_bInvisible = false;
        userIcon = "invisible";
    }
    setText(2, msgType ? SIMClient::getMessageIcon(msgType) : userIcon);
    m_status = u->uStatus;
    m_uin = u->Uin;
    nBlink = 0;
    unsigned st = 9;
    if (u->uStatus != ICQ_STATUS_OFFLINE){
        SIMUser *_u = static_cast<SIMUser*>(u);
        SIMUser *_o = static_cast<SIMUser*>(pClient->owner);
        switch (u->uStatus & 0xFF){
        case ICQ_STATUS_ONLINE:
        case ICQ_STATUS_FREEFORCHAT:
            if (((u->prevStatus & 0xFF) != ICQ_STATUS_ONLINE) &&
                    ((u->prevStatus & 0xFF) != ICQ_STATUS_FREEFORCHAT) &&
                    (((pClient->owner->uStatus & 0xFF) == ICQ_STATUS_ONLINE) ||
                     ((pClient->owner->uStatus & 0xFF) == ICQ_STATUS_FREEFORCHAT)) &&
                    ((u->prevStatus == ICQ_STATUS_OFFLINE) || _o->AlertAway) &&
                    ((u->OnlineTime > _o->OnlineTime) || ((u->prevStatus & 0xFFFF) != ICQ_STATUS_OFFLINE))){
                if (!_u->AlertOverride) _u = _o;
                if (_u->AlertBlink)
                    nBlink = 18;
            }
            st = 1;
            break;
        case ICQ_STATUS_DND:
            st = 2;
            break;
        case ICQ_STATUS_OCCUPIED:
            st = 3;
            break;
        case ICQ_STATUS_AWAY:
            st = 4;
            break;
        default:
            st = 5;
        }
    }
    if (pMain->AlphabetSort){
        QString s;
        if (!users->m_bGroupMode) s = QString::number(m_itemState) + "1";
        s += name;
        setText(1, s);
    }else{
        char b[32];
        snprintf(b, sizeof(b), "%u%u1%08lX", m_itemState, st, -(u->LastActive+1));
        QString t = b;
        t += name;
        setText(1, t);
    }
}

UserViewItem::~UserViewItem()
{
    if (parent() && (m_status != ICQ_STATUS_OFFLINE))
        static_cast<GroupViewItem*>(parent())->changeCounter(false);
    UserView *users = static_cast<UserView*>(listView());
    if (users)
        users->decStateCount(m_itemState);
}

int UserViewItem::type()
{
    return 1;
}

GroupViewItem::GroupViewItem(ICQGroup *g, unsigned n, UserView *parent)
        : UserViewItemBase(parent)
{
    m_nOnline = 0;
    char b[32];
    snprintf(b, sizeof(b), "%08u", n);
    setText(1, b);
    setExpandable(true);
    if (g == NULL){
        m_id = 0;
        setText(0, i18n("Not in list"));
        setOpen(pClient->contacts.Expand);
        return;
    }
    m_id = g->Id;
    CGroup grp(g);
    setText(0, grp.name());
    setOpen(static_cast<SIMGroup*>(g)->Expand);
    UserView *users = static_cast<UserView*>(listView());
    if (users->bList) setText(3, QString::number(CHECK_OFF));
}

int GroupViewItem::type()
{
    return 2;
}

void GroupViewItem::setOpen(bool bState)
{
    UserView *users = static_cast<UserView*>(listView());
    if (users){
        if (users->bList) bState = true;
        setText(2, bState ? "expanded" : "collapsed");
        users->setGroupExpand(m_id, bState);
    }
    UserViewItemBase::setOpen(bState);
    if (m_id){
        ICQGroup *g = pClient->getGroup(m_id);
        static_cast<SIMGroup*>(g)->Expand = bState;
    }else{
        pClient->contacts.Expand = bState;
    }
}

void GroupViewItem::paintCell(QPainter *p, const QColorGroup &cg, int, int, int)
{
    QFont f(listView()->font());
    int size = f.pixelSize();
    if (size <= 0){
        size = f.pointSize();
        f.setPointSize(size * 3 / 4);
    }else{
        f.setPixelSize(size * 3 / 4);
    }
    f.setBold(true);
    p->setFont(f);
    QString s;
    if (m_nOnline) s.sprintf(" (%u)", m_nOnline);
    paint(p, text(0) + s, cg, true);
}

void GroupViewItem::changeCounter(bool bInc)
{
    bInc ? m_nOnline++ : m_nOnline--;
    repaint();
}

DivItem::DivItem(const QString &name, const QString &key, UserView *parent)
        : UserViewItemBase(parent)
{
    setText(0, name);
    setText(1, key);
    setSelectable(false);
}

int DivItem::type()
{
    return 2;
}

void DivItem::paintCell(QPainter *p, const QColorGroup &cg, int, int, int)
{
    QFont f(listView()->font());
    int size = f.pixelSize();
    if (size <= 0){
        size = f.pointSize();
        f.setPointSize(size * 3 / 4);
    }else{
        f.setPixelSize(size * 3 / 4);
    }
    p->setFont(f);
    paint(p, text(0), cg, true);
}

#define ACCEL(a)	QAccel::stringToKey(SIMClient::getMessageAccel(a))

UserView::UserView (QWidget *parent, bool _bList, bool bFill, WFlags f)
        : QListView(parent, NULL, f), QToolTip(viewport())
{
    bList = _bList;
    bStaticBg = false;
    setBackgroundMode(QListView::NoBackground);
    viewport()->setBackgroundMode(QListView::NoBackground);
    viewport()->setAcceptDrops(true);
    bFloaty = false;
    mousePressPos = QPoint(0, 0);
    m_bShowOffline = false;
    m_bGroupMode = false;
    if (bList){
        m_bShowOffline = true;
        m_bGroupMode = true;
    }
    header()->hide();
    addColumn("", -1);
    setSorting(0);
    memset(m_counts, 0, sizeof(m_counts));
    grp_id = 0;
    menuGroup = new KPopupMenu(this);
    connect(menuGroup, SIGNAL(activated(int)), this, SLOT(grpFunction(int)));
    setHScrollBarMode(AlwaysOff);
    setTreeStepSize(0);
    if (bFill) fill();
    connect(this, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(doubleClick(QListViewItem*)));
    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(pClient, SIGNAL(messageRead(ICQMessage*)), this, SLOT(messageRead(ICQMessage*)));
    connect(pClient, SIGNAL(messageReceived(ICQMessage*)), this, SLOT(messageReceived(ICQMessage*)));
    connect(pMain, SIGNAL(bgChanged()), this, SLOT(bgChanged()));
    edtGroup = new IntLineEdit(viewport());
    edtGroup->hide();
    QFont font(QListView::font());
    int size = font.pixelSize();
    if (size <= 0){
        size = font.pointSize();
        font.setPointSize(size * 3 / 4);
    }else{
        font.setPixelSize(size * 3 / 4);
    }
    font.setBold(true);
    edtGroup->setFont(font);
    connect(edtGroup, SIGNAL(escape()), this, SLOT(editEscape()));
    connect(edtGroup, SIGNAL(returnPressed()), this, SLOT(editEnter()));
    connect(edtGroup, SIGNAL(focusOut()), this, SLOT(editEnter()));
    transparent = new TransparentBg(this);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(blink()));
    timer->start(800);
    connect(pMain, SIGNAL(iconChanged()), this, SLOT(iconChanged()));
    QObjectList * l = queryList("QScrollBar");
    QObjectListIt it(*l);
    QObject * obj;
    while ( (obj=it.current()) != 0 ){
        ++it;
        obj->installEventFilter(this);
    }
    delete l;
    QAccel *accel = new QAccel(this);
    connect(accel, SIGNAL(activated(int)), this, SLOT(accelActivated(int)));
    accel->insertItem(ACCEL(ICQ_MSGxMSG), mnuMessage);
    accel->insertItem(ACCEL(ICQ_MSGxURL), mnuURL);
    accel->insertItem(ACCEL(ICQ_MSGxFILE), mnuFile);
    accel->insertItem(ACCEL(ICQ_MSGxCHAT), mnuChat);
    accel->insertItem(ACCEL(ICQ_MSGxCONTACTxLIST), mnuContacts);
    accel->insertItem(ACCEL(ICQ_MSGxMAIL), mnuMail);
    accel->insertItem(QAccel::stringToKey(i18n("Del", "Delete")), mnuDelete);
    accel->insertItem(QListView::Key_F2, mnuGrpRename);
    accel->insertItem(QAccel::stringToKey(i18n("Ctrl+N", "Create group")), mnuGrpCreate);
    accel->insertItem(QListView::CTRL + QListView::Key_Up, mnuGrpUp);
    accel->insertItem(QListView::CTRL + QListView::Key_Down, mnuGrpDown);
    accel->insertItem(QListView::CTRL + QListView::Key_Plus, mnuGrpExpandAll);
    accel->insertItem(QListView::CTRL + QListView::Key_Minus, mnuGrpCollapseAll);
    bgChanged();
    pressedItem = NULL;
    viewport()->setMouseTracking(true);
}

void UserView::accelActivated(int id)
{
    switch (id){
    case mnuGrpExpandAll:
    case mnuGrpCollapseAll:
        grpFunction(id);
        return;
    default:
        break;
    }
    QRect rc;
    QPoint pRect;
    QListViewItem *p = selectedItem();
    if (p == NULL) return;
    switch (static_cast<UserViewItemBase*>(p)->type()){
    case 1:{
            UserViewItem *item = static_cast<UserViewItem*>(p);
            switch (id){
            case mnuMessage:
            case mnuURL:
            case mnuFile:
            case mnuChat:
            case mnuContacts:
            case mnuMail:
            case mnuDelete:
                pMain->m_uin = item->m_uin;
                rc = itemRect(item);
                pRect = mapToGlobal(rc.topLeft());
                rc.setRect(pRect.x(), pRect.y(), rc.width(), rc.height());
                pMain->m_rc = rc;
                pMain->userFunction(id);
                break;
            }
            break;
        }
    case 2:{
            GroupViewItem *item = static_cast<GroupViewItem*>(p);
            grp_id = item->Id();
            switch (id){
            case mnuDelete:
                grpFunction(mnuGrpDelete);
                break;
            case mnuGrpRename:
            case mnuGrpCreate:
            case mnuGrpUp:
            case mnuGrpDown:
                grpFunction(id);
                break;
            }
            break;
        }
    }
}

#ifndef WIN32
extern Time qt_x_time;
#endif

bool UserView::eventFilter(QObject *obj, QEvent *e)
{
    if ((e->type() == QEvent::Enter) && !qApp->focusWidget()){
#ifndef WIN32
        XEvent ev;
        memset(&ev, 0, sizeof(ev));
        ev.xfocus.display = qt_xdisplay();
        ev.xfocus.type = FocusIn;
        ev.xfocus.window = static_cast<QWidget*>(obj)->topLevelWidget()->winId();
        ev.xfocus.mode = NotifyNormal;
        ev.xfocus.detail = NotifyAncestor;
        Time time = qt_x_time;
        qt_x_time = 1;
        qApp->x11ProcessEvent( &ev );
        qt_x_time = time;
#endif
    }
    if (obj == menuGroup){
        if (e->type() == QEvent::Hide)
            QTimer::singleShot(0, this, SLOT(clearGroupMenu()));
        return QListView::eventFilter(obj, e);
    }
    bool res = QListView::eventFilter(obj, e);
    if ((e->type() == QEvent::Show) || (e->type() == QEvent::Hide))
        viewport()->repaint();
    return res;
}

void UserView::clearGroupMenu()
{
    menuGroup->clear();
}

void UserView::iconChanged()
{
    viewport()->repaint();
}

void UserView::drawImage(QPainter *p, int x, int y, int w, int h, int imgX, int imgY, int imgW, int imgH)
{
    if (bgPict.isNull()) return;
    QImage &img = bgPict;
    if ((bgPict.width() != imgW) || (bgPict.height() != imgH)){
        if (bgPictScale.isNull() || (bgPictScale.width() != imgW) || (bgPictScale.height() != imgH)){
            bgPictScale = bgPict;
            bgPictScale = bgPictScale.smoothScale(imgW, imgH);
        }
        img = bgPictScale;
    }
    while (imgX >= img.width())
        imgX -= img.width();
    while (imgX < 0)
        imgX += img.width();
    while (imgY >= img.height())
        imgY -= img.height();
    while (imgY < 0)
        imgY += img.height();

    int right  = x + w;
    int bottom = y + h;
    while (y < bottom){
        int wx = x;
        int wImgX = imgX;
        while (wx < right){
            p->drawImage(wx, y, img, wImgX, imgY, right - wx, bottom - y);
            if (wImgX){
                wx += img.width() - wImgX;
                wImgX = 0;
            }else{
                wx += img.width();
            }
        }
        if (imgY){
            y += img.height() - imgY;
            imgY = 0;
        }else{
            y += img.height();
        }
    }
}

void UserView::bgChanged()
{
    bgPictScale = QImage();
    if (*pMain->BackgroundFile.c_str()){
        bgPict = QImage(QString::fromLocal8Bit(pMain->BackgroundFile.c_str()));
    }else{
        bgPict = QImage();
    }
    viewport()->repaint();
}

void UserView::blink()
{
    for (QListViewItem *item = firstChild(); item != NULL; item = item->nextSibling())
        blink(item);
}

void UserView::blink(QListViewItem *p)
{
    for (QListViewItem *item = p->firstChild(); item != NULL; item = item->nextSibling())
        blink(item);
    if (static_cast<UserViewItemBase*>(p)->type() != 1) return;
    UserViewItem *userItem = static_cast<UserViewItem*>(p);
    if (userItem->nBlink == 0) return;
    userItem->nBlink--;
    userItem->repaint();
}

void UserView::editEscape()
{
    edtGroup->hide();
}

void UserView::editEnter()
{
    edtGroup->hide();
    GroupViewItem *item = findGroupItem(grp_id);
    if (item == NULL) return;
    ICQGroup *grp = pClient->getGroup(grp_id);
    if (grp == NULL) return;
    pClient->renameGroup(grp, edtGroup->text().local8Bit());
}

extern const ToolBarDef *pToolBarMain;

void UserView::grpFunction(int id)
{
    switch (id){
    case mnuGrpExpandAll:
        setOpen(true);
        return;
    case mnuGrpCollapseAll:
        setOpen(false);
        return;
    case mnuGrpCreate:
        pMain->setShow(true);
        pClient->createGroup(i18n("New group"));
        return;
    case mnuToolBar:
        ToolBarSetup::show(pToolBarMain, &pMain->ToolBarMain);
        return;
    case mnuGrpRename:{
            if (grp_id == 0) return;
            GroupViewItem *item = findGroupItem(grp_id);
            if (item == NULL) return;
            ICQGroup *grp = pClient->getGroup(grp_id);
            if (grp == NULL) return;
            CGroup g(grp);
            ensureItemVisible(item);
            QRect rc = itemRect(item);
            rc.setLeft(rc.left() + 18);
            edtGroup->setGeometry(rc);
            edtGroup->setText(*grp->Name.c_str() ? g.name() : i18n("New group"));
            edtGroup->setSelection(0, edtGroup->text().length());
            edtGroup->show();
            edtGroup->setFocus();
            return;
        }
    case mnuGrpDelete:{
            if (grp_id == 0) return;
            GroupViewItem *item = findGroupItem(grp_id);
            if (item == NULL) return;
            ICQGroup *grp = pClient->getGroup(grp_id);
            if (grp == NULL) return;
            pClient->deleteGroup(grp);
            return;
        }
    case mnuGrpUp:{
            if (grp_id == 0) return;
            ICQGroup *grp = pClient->getGroup(grp_id);
            if (grp == NULL) return;
            unsigned i;
            for (i = 0; i < pClient->contacts.groups.size(); i++){
                if (pClient->contacts.groups[i] == grp) break;
            }
            if (i == 0) break;
            ICQGroup *s_grp = pClient->contacts.groups[i-1];
            pClient->contacts.groups[i-1] = grp;
            pClient->contacts.groups[i] = s_grp;
            refresh();
            QListViewItem *item = findGroupItem(grp_id);
            if (item) setCurrentItem(item);
            return;
        }
    case mnuGrpDown:{
            if (grp_id == 0) return;
            ICQGroup *grp = pClient->getGroup(grp_id);
            if (grp == NULL) return;
            unsigned i;
            for (i = 0; i < pClient->contacts.groups.size(); i++){
                if (pClient->contacts.groups[i] == grp) break;
            }
            if (i == pClient->contacts.groups.size() - 1) break;
            ICQGroup *s_grp = pClient->contacts.groups[i+1];
            pClient->contacts.groups[i+1] = grp;
            pClient->contacts.groups[i] = s_grp;
            refresh();
            QListViewItem *item = findGroupItem(grp_id);
            if (item) setCurrentItem(item);
            return;
        }
    }
}

void UserView::doubleClick(QListViewItem *item)
{
    if (bList) return;
    UserViewItemBase *item_base = static_cast<UserViewItemBase*>(item);
    switch (item_base->type()){
    case 1:{
            UserViewItem *ui = static_cast<UserViewItem*>(item);
            pMain->userFunction(ui->m_uin, mnuAction);
            break;
        }
    case 2:
        item->setOpen(!item->isOpen());
        break;
    }
}

void UserView::paintEmptyArea(QPainter *p, const QRect &r)
{
    const QPixmap *pix = transparent->background(colorGroup().base());
    int w = 0;
    bool bTile = false;
    int cx = 0;
    int cy = 0;
    int h = r.height();
    QListViewItem *item = firstChild();
    if (item) h = item->height();
    int ch = h;
    if (!bgPict.isNull()){
        switch (pMain->BackgroundMode){
        case bgModeContactLeft:
            w = bgPict.width();
            break;
        case bgModeContactScale:
            w = width();
            break;
        case bgModeWndTop:
            w = bgPict.width();
            ch = bgPict.height();
            cy = r.top();
            bTile = true;
            break;
        case bgModeWndBottom:
            w = bgPict.width();
            ch = bgPict.height();
            cy = r.top() + (bgPict.height() - height());
            bTile = true;
            break;
        case bgModeWndCenter:
            w = bgPict.width();
            ch = bgPict.height();
            cy = r.top() + (bgPict.height() - height()) / 2;
            bTile = true;
            break;
        case bgModeWndScale:
            w = width();
            ch = height();
            cy = r.top();
            break;
        }
    }
    if (w){
        int wImg = w;
        if (bTile) w = width();
        if (bgPict.hasAlphaBuffer()){
            QPixmap bPict(w, r.height());
            QPainter pBuf(&bPict);
            if (pix){
                QPoint pp(w, r.top());
                pp = viewport()->mapToParent(pp);
                pp = topLevelWidget()->mapFromGlobal(mapToGlobal(pp));
                pBuf.drawTiledPixmap(0, 0, w, h, *pix, pp.x(), pp.y());
            }else{
                pBuf.fillRect(QRect(0, 0, w, h), colorGroup().base());
            }
            drawImage(&pBuf, 0, 0, w, r.height(), cx, cy, wImg, ch);
            pBuf.end();
            p->drawPixmap(0, r.top(), bPict);
        }else{
            drawImage(p, 0, r.top(), w, r.height(), cx, cy, wImg, ch);
        }
        w += r.left();
        if (w < r.right()){
            if (pix){
                QPoint pp(w, r.top());
                pp = viewport()->mapToParent(pp);
                pp = topLevelWidget()->mapFromGlobal(mapToGlobal(pp));
                p->drawTiledPixmap(w, r.top(), r.right() - w, r.bottom(), *pix, pp.x(), pp.y());
            }else{
                p->fillRect(QRect(w, r.top(), r.right(), r.bottom()), colorGroup().base());
            }
        }
    }else{
        if (pix){
            QPoint pp(topLevelWidget()->mapFromGlobal(mapToGlobal(r.topLeft())));
            p->drawTiledPixmap(r.x(), r.y(), r.width(), r.height(), *pix, pp.x(), pp.y());
        }
        else
        {
            p->fillRect(r, colorGroup().base());
        }
    }
}

void UserView::clear()
{
    QListView::clear();
    memset(m_counts, 0, sizeof(m_counts));
}

void UserView::refresh()
{
    if (bFloaty){
        QListViewItem *item = firstChild();
        if (item == NULL) return;
        UserViewItem *userItem = static_cast<UserViewItem*>(item);
        ICQUser *u = pClient->getUser(userItem->m_uin);
        userItem->update(u);
        return;
    }
    bool isUpdates = viewport()->isUpdatesEnabled();
    viewport()->setUpdatesEnabled(false);
    clear();
    fill();
    if (!isUpdates) return;
    viewport()->setUpdatesEnabled(true);
    viewport()->repaint();
}

void UserView::fill()
{
    if (bFloaty) return;
    list<ICQUser*>::iterator it;
    ICQContactList &contacts = pClient->contacts;
    if (m_bGroupMode){
        vector<ICQGroup*>::iterator grp_it;
        unsigned n = 0;
        for (grp_it = contacts.groups.begin(); grp_it != contacts.groups.end(); grp_it++){
            new GroupViewItem(*grp_it, n, this);
            n++;
        }
        new GroupViewItem(NULL, 0x10000, this);
    }
    for (it = contacts.users.begin(); it != contacts.users.end(); it++){
        if (bList && ((*it)->Type != USER_TYPE_ICQ)) continue;
        addUserItem(*it);
    }
}

UserViewItem *UserView::findUserItem(unsigned long uin)
{
    for (QListViewItemIterator it(this); it.current(); it++){
        if (static_cast<UserViewItemBase*>(it.current())->type() != 1) continue;
        if (static_cast<UserViewItem*>(it.current())->m_uin != uin) continue;
        return static_cast<UserViewItem*>(it.current());
    }
    return NULL;
}

GroupViewItem *UserView::findGroupItem(unsigned short grpId)
{
    for (QListViewItemIterator it(this); it.current(); it++){
        if (static_cast<UserViewItemBase*>(it.current())->type() != 2) continue;
        if (static_cast<GroupViewItem*>(it.current())->m_id != grpId) continue;
        return static_cast<GroupViewItem*>(it.current());
    }
    return NULL;
}

void UserView::resizeEvent(QResizeEvent *e)
{
    setColumnWidth(0, e->size().width());
    QListView::resizeEvent(e);
    repaint();
}

void UserView::styleChange(QStyle &oldStyle)
{
    QListView::styleChange(oldStyle);
    repaint();
}

void UserView::setOpen(bool bOpen)
{
    for (QListViewItemIterator it(this); it.current(); it++){
        if (static_cast<UserViewItemBase*>(it.current())->type() != 2) continue;
        it.current()->setOpen(bOpen);
    }
}

bool UserView::isUserShow(ICQUser *u)
{
    if (u == NULL) return false;
    if (u->IgnoreId || u->bIsTemp) return false;
    if (u->Uin == pClient->owner->Uin) return false;
    if (m_bShowOffline) return true;
    if (u->Type != USER_TYPE_ICQ) return true;
    if (u->unreadMsgs.size()) return true;
    return (u->uStatus != ICQ_STATUS_OFFLINE);
}

void UserView::addUserItem(ICQUser *u)
{
    if (!isUserShow(u)) return;
    if (!m_bGroupMode){
        new UserViewItem(u, this);
        return;
    }
    GroupViewItem *grp = findGroupItem(u->GrpId);
    if (grp == NULL) return;
    new UserViewItem(u, grp);
}

void UserView::setGroupMode(bool bGroupMode)
{
    if (bGroupMode == m_bGroupMode) return;
    m_bGroupMode = bGroupMode;
    refresh();
}

void UserView::setShowOffline(bool bShowOffline)
{
    if (bFloaty) return;
    if (bShowOffline == m_bShowOffline) return;
    bool bUpdates = viewport()->isUpdatesEnabled();
    viewport()->setUpdatesEnabled(false);
    m_bShowOffline = bShowOffline;
    ICQContactList &contacts = pClient->contacts;
    list<ICQUser*>::iterator it;
    if (m_bShowOffline){
        for (it = contacts.users.begin(); it != contacts.users.end(); it++){
            UserViewItem *item = findUserItem((*it)->Uin);
            if (item == NULL) addUserItem(*it);
        }
    }else{
        for (it = contacts.users.begin(); it != contacts.users.end(); it++){
            if (isUserShow(*it)) continue;
            UserViewItem *item = findUserItem((*it)->Uin);
            if (item) delete item;
        }
    }
    if (!bUpdates) return;
    viewport()->setUpdatesEnabled(true);
    viewport()->repaint();
}

void UserView::updateUser(unsigned long uin, bool bFull)
{
    ICQUser *u = pClient->getUser(uin);
    if (u && u->bIsTemp) return;
    UserViewItem *item = findUserItem(uin);
    if (bFloaty){
        if (item == NULL) return;
    }else{
        if ((u == NULL) || u->IgnoreId){
            if (item) delete item;
            return;
        }
        if (item && bFull){
            delete item;
            item = NULL;
        }
        if (!isUserShow(u))
        {
            delete item;
            return;
        }
        if (item == NULL)
        {
            addUserItem(u);
            return;
        }
    }
    item->update(u);
    sort();
    if (item->parent()) item->parent()->sort();
    repaint();
    userChanged();
}

void UserView::messageRead(ICQMessage *msg)
{
    updateUser(msg->getUin(), false);
}

void UserView::processEvent(ICQEvent *e)
{
    switch (e->type()){
    case EVENT_GROUP_CREATED:
        refresh();
        if (!bList){
            pMain->setShow(true);
            grp_id = e->Uin();
            grpFunction(mnuGrpRename);
        }
        return;
    case EVENT_GROUP_CHANGED:
        refresh();
        return;
    case EVENT_USER_DELETED:
    case EVENT_USERGROUP_CHANGED:
    case EVENT_STATUS_CHANGED:
    case EVENT_INFO_CHANGED:{
            if (bList){
                ICQUser *u = pClient->getUser(e->Uin());
                if ((u == NULL) || (u->Type != USER_TYPE_ICQ)) break;
            }
            updateUser(e->Uin(), e->type() == EVENT_USERGROUP_CHANGED);
            break;
        }
    }
}

void UserView::messageReceived(ICQMessage *msg)
{
    updateUser(msg->getUin(), false);
}

void UserView::setGroupExpand(unsigned short grpId, bool bState)
{
    ICQContactList &contacts = pClient->contacts;
    if (grpId == 0){
        contacts.Expand = bState;
        return;
    }
    ICQGroup *g = pClient->getGroup(grpId);
    if (g == NULL) return;
    static_cast<SIMGroup*>(g)->Expand = bState;
}

void UserView::incStateCount(unsigned state)
{
    if (m_bGroupMode) return;
    unsigned *counter = m_counts + state;
    if ((*counter)++) return;
    if (bFloaty) return;
    QString name;
    switch (state){
    case STATE_ONLINE:
        name = i18n("Online");
        break;
    case STATE_OFFLINE:
        name = i18n("Offline");
        break;
    case STATE_NOTINLIST:
        name = i18n("Not in list");
        break;
    }
    new DivItem(name, QString::number(state) + "0", this);
    sort();
}

void UserView::decStateCount(unsigned state)
{
    if (m_bGroupMode) return;
    unsigned *counter = m_counts + state;
    if (--(*counter)) return;
    for (QListViewItemIterator it(this); it.current(); it++){
        if (it.current()->text(1) == (QString::number(state) + "0")){
            delete it.current();
            break;
        }
    }
}

UserViewItem *UserView::findUserItem(QPoint p)
{
    QListViewItem *list_item = itemAt(viewport()->mapFromGlobal(p));
    if (list_item == NULL) return NULL;
    UserViewItemBase *item = static_cast<UserViewItemBase*>(list_item);
    if (item->type() != 1) return NULL;
    return static_cast<UserViewItem*>(list_item);
}

ICQUser *UserView::findUser(QPoint p)
{
    UserViewItem *item = findUserItem(p);
    if (item == NULL) return NULL;
    return pClient->getUser(item->m_uin);
}

void UserView::dragStart()
{
    if (bList) return;
    startDrag();
}

void UserView::contentsDragEnterEvent(QDragEnterEvent *e)
{
    dragEvent(e, false);
}

void UserView::contentsDragMoveEvent(QDragMoveEvent *e)
{
    dragEvent(e, false);
}

void UserView::contentsDropEvent(QDropEvent *e)
{
    dragEvent(e, true);
}

void UserView::callUserFunction(unsigned long uin, const QString &url, bool bUrl)
{
    QString protocol;
    int p = url.find(QRegExp("^\"?[A-Za-z]+:/"));
    if (p >= 0){
        p = url.find(':');
        protocol = url.left(p);
    }
    if (protocol[0] == '\"')
        protocol = protocol.mid(1);
    if ((protocol == "http") || (protocol == "https") || (protocol == "ftp")){
        pMain->userFunction(uin, mnuURL, (unsigned long)&url);
    }else if (protocol.length() || bUrl){
        pMain->userFunction(uin, mnuFile, (unsigned long)&url);
    }else if ((url.left(2) == "\\\\") || (url.left(3) == "\"\\\\")){
        pMain->userFunction(uin, mnuFile, (unsigned long)&url);
    }else if (url.length()){
        pMain->userFunction(uin, mnuMessage, (unsigned long)&url);
    }
}

void UserView::dragEvent(QDropEvent *e, bool isDrop)
{
    if (bList){
        e->ignore();
        return;
    }
    QListViewItem *list_item = itemAt(contentsToViewport(e->pos()));
    if (list_item == NULL){
        e->ignore();
        return;
    }
    UserViewItemBase *item = static_cast<UserViewItemBase*>(list_item);
    if (item->type() == 3){
        e->ignore();
        return;
    }
    QString text;
    QStringList urls;
    if (QUriDrag::decodeLocalFiles(e, urls) && !urls.isEmpty() && (item->type() == 1)){
        if (isDrop){
            UserViewItem *ui = static_cast<UserViewItem*>(item);
            if (urls.count() > 1){
                for (QStringList::Iterator it = urls.begin(); it != urls.end(); ++it){
                    *it = QString("\"") + *it + "\"";
                }
            }else{
                if (urls[0].find(' ') >= 0)
                    urls[0] = QString("\"") + urls[0] + QString("\"");
            }
            callUserFunction(ui->m_uin, urls.join(" "), true);
        }
        e->acceptAction(true);
        return;
    }
    if (QTextDrag::decode(e, text)){
        unsigned long uin = text.toULong();
        if (uin < 10000) uin = 0;
        if (item->type() == 1){
            UserViewItem *ui = static_cast<UserViewItem*>(item);
            if (uin){
                if (uin == ui->m_uin){
                    e->ignore();
                    return;
                }
                if (isDrop) pMain->userFunction(ui->m_uin, mnuContacts, uin);
            }else if (isDrop){
                callUserFunction(uin, text, false);
            }
            e->acceptAction(true);
            return;
        }
        if ((item->type() == 2) && uin){
            GroupViewItem *gi = static_cast<GroupViewItem*>(item);
            ICQUser *u = pClient->getUser(uin, true);
            ICQGroup *g = pClient->getGroup(gi->m_id);
            if ((u == NULL) || (g == NULL)){
                e->ignore();
                return;
            }
            if (isDrop) pClient->moveUser(u, g);
            e->acceptAction(true);
            return;
        }
    }
    e->ignore();
}

void UserView::itemClicked(QListViewItem *list_item)
{
    if (!bList) return;
    if (list_item == NULL) return;
    UserViewItemBase *item = static_cast<UserViewItemBase*>(list_item);
    int state;
    switch (item->type() ){
    case 1:
        if (item->text(3).toInt() == CHECK_ON){
            item->setText(3, QString::number(CHECK_OFF));
        }else{
            item->setText(3, QString::number(CHECK_ON));
        }
        item->repaint();
        setCurrentItem(item);
        setGrpCheck(item);
        emit checked();
        break;
    case 2:
        state = CHECK_ON;
        if (item->text(3).toInt() == CHECK_ON) state = CHECK_OFF;
        item->setText(3, QString::number(state));
        item->repaint();
        setCurrentItem(item);
        for (list_item = item->firstChild(); list_item; list_item = list_item->nextSibling()){
            if (list_item->text(3).toInt() == state) continue;
            list_item->setText(3, QString::number(state));
            list_item->repaint();
        }
        emit checked();
        break;
    }
}

void UserView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if (!pMain->UseDoubleClick) return;
    QListViewItem *list_item = itemAt(contentsToViewport(e->pos()));
    if (list_item) doubleClick(list_item);
}

void UserView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (bList){
        QListViewItem *list_item = itemAt(contentsToViewport(e->pos()));
        if (list_item) itemClicked(list_item);
        QListView::contentsMouseReleaseEvent(e);
    }
#if QT_VERSION < 300
    if (e->button() == QObject::RightButton){
        QContextMenuEvent contextEvent(e->globalPos());
        viewportContextMenuEvent(&contextEvent);
        return;
    }
    mousePressPos = QPoint(0, 0);
#endif
    if (!bList){
        clearSelection();
        if (!pMain->UseDoubleClick && pressedItem && (pressedItem == itemAt(contentsToViewport(e->pos()))))
            doubleClick(pressedItem);
        pressedItem = NULL;
    }
    QListView::contentsMouseReleaseEvent(e);
}

void UserView::contentsMousePressEvent(QMouseEvent *e)
{
#if QT_VERSION < 300
    if (!bList){
        if ((e->button() == QObject::LeftButton) && !bFloaty){
            mousePressPos = e->pos();
            QTimer::singleShot(QApplication::startDragTime(), this, SLOT(dragStart()));
        }
    }
#endif
    if (e->button() == QObject::LeftButton){
        pressedItem = itemAt(contentsToViewport(e->pos()));
    }
    QListView::contentsMousePressEvent(e);
}

void UserView::contentsMouseMoveEvent(QMouseEvent *e)
{
#if QT_VERSION < 300
    if (e->state() & QObject::LeftButton){
        if (!bFloaty && !bList && !mousePressPos.isNull() && currentItem() &&
                (QPoint(e->pos() - mousePressPos).manhattanLength() > QApplication::startDragDistance())){
            startDrag();
        }
    }
#endif
    QListView::contentsMouseMoveEvent(e);
}

void UserView::check(unsigned long uin)
{
    UserViewItem *item = findUserItem(uin);
    if (item == NULL) return;
    item->setText(3, QString::number(CHECK_ON));
    item->repaint();
    setGrpCheck(item);
}

void UserView::setGrpCheck(QListViewItem *item)
{
    QListViewItem *grp_item = item->parent();
    if (grp_item == NULL) return;
    item = grp_item->firstChild();
    int state = CHECK_NOCHANGE;
    if (item){
        state = item->text(3).toInt();
        for (; item; item = item->nextSibling()){
            if (state != item->text(3).toInt()){
                state = CHECK_NOCHANGE;
                break;
            }
        }
    }
    if (state == grp_item->text(3).toInt()) return;
    grp_item->setText(3, QString::number(state));
    grp_item->repaint();
}

bool UserView::hasChecked()
{
    for (QListViewItem *item = firstChild(); item; item = item->nextSibling())
        if (hasChecked(item)) return true;
    return false;
}

bool UserView::hasChecked(QListViewItem *item)
{
    if (item->text(3).toInt() == CHECK_ON) return true;
    for (item = item->firstChild(); item; item = item->nextSibling())
        if (hasChecked(item)) return true;
    return false;
}

void UserView::fillChecked(ICQMessage *msg)
{
    for (QListViewItem *item = firstChild(); item; item = item->nextSibling())
        fillChecked(item, msg);
}

void UserView::fillChecked(QListViewItem *item, ICQMessage *msg)
{
    if (item->text(3).toInt() == CHECK_ON){
        UserViewItemBase *base_item = static_cast<UserViewItemBase*>(item);
        if (base_item->type() == 1){
            UserViewItem *ui = static_cast<UserViewItem*>(item);
            msg->Uin.push_back(ui->m_uin);
        }
    }
    for (item = item->firstChild(); item; item = item->nextSibling())
        fillChecked(item, msg);
}

void UserView::startDrag()
{
    if (bFloaty) return;
#if QT_VERSION < 300
    if (mousePressPos.isNull()) return;
#endif
    QDragObject *d = dragObject();
    if (d == NULL) return;
    d->dragCopy();
#if QT_VERSION < 300
    mousePressPos = QPoint(0, 0);
#endif
}

class MyTextDrag : public QTextDrag
{
public:
    MyTextDrag(QListView *view, const QString &str);
    ~MyTextDrag();
protected:
    QListView *mView;
};

MyTextDrag::MyTextDrag(QListView *view, const QString &str)
        : QTextDrag(str, view)
{
    mView = view;
}

MyTextDrag::~MyTextDrag()
{
    QTimer::singleShot(0, mView, SLOT(selectionClear()));
}

void UserView::selectionClear()
{
    clearSelection();
}

QDragObject *UserView::dragObject()
{
    UserViewItemBase *item = static_cast<UserViewItemBase*>(currentItem());
    switch (item->type()){
    case 1:
        return new MyTextDrag(this, QString::number(static_cast<UserViewItem*>(item)->m_uin));
    }
    return NULL;
}

void UserView::viewportContextMenuEvent( QContextMenuEvent *e)
{
    if (bList) return;
    QRect rc;
    QPoint pRect;
    QPoint p = e->globalPos();
    QListViewItem *list_item = itemAt(viewport()->mapFromGlobal(p));
    if (list_item == NULL) return;
    UserViewItemBase *item = static_cast<UserViewItemBase*>(list_item);
    switch (item->type()){
    case 1:
        rc = itemRect(item);
        pRect = mapToGlobal(rc.topLeft());
        rc.setRect(pRect.x(), pRect.y(), rc.width(), rc.height());
        pMain->showUserPopup(static_cast<UserViewItem*>(list_item)->m_uin, p, NULL, rc);
        break;
    case 2:
        GroupViewItem *grpItem = static_cast<GroupViewItem*>(list_item);
        grp_id = grpItem->m_id;
        if (grp_id == 0) return;
        ICQGroup *grp = pClient->getGroup(grp_id);
        if (grp == NULL) return;
        unsigned i;
        for (i = 0; i < pClient->contacts.groups.size(); i++){
            if (pClient->contacts.groups[i] == grp) break;
        }
        CGroup g(grp);
        menuGroup->clear();
        menuGroup->insertTitle(g.name(), mnuGrpTitle);
        if (pClient->isLogged()){
            menuGroup->insertItem(Pict("grp_rename"), i18n("Rename"), mnuGrpRename);
            menuGroup->setAccel(QListView::Key_F2, mnuGrpRename);
            menuGroup->insertItem(Pict("remove"), i18n("Delete"), mnuGrpDelete);
            menuGroup->setAccel(QAccel::stringToKey(i18n("Del", "Delete")), mnuGrpDelete);
            menuGroup->insertItem(Pict("grp_create"), i18n("Create group"), mnuGrpCreate);
            menuGroup->setAccel(QAccel::stringToKey(i18n("Ctrl+N", "Create group")), mnuGrpCreate);
            menuGroup->insertSeparator();
        }
        menuGroup->insertItem(Pict("1uparrow"), i18n("Up"), mnuGrpUp);
        menuGroup->setAccel(QListView::CTRL + QListView::Key_Up, mnuGrpUp);
        menuGroup->insertItem(Pict("1downarrow"), i18n("Down"), mnuGrpDown);
        menuGroup->setAccel(QListView::CTRL + QListView::Key_Down, mnuGrpDown);
        menuGroup->insertSeparator();
        menuGroup->insertItem(Pict("grp_expand"), i18n("Expand all"), mnuGrpExpandAll);
        menuGroup->setAccel(QListView::CTRL + QListView::Key_Plus, mnuGrpExpandAll);
        menuGroup->insertItem(Pict("grp_collapse"), i18n("Collapse all"), mnuGrpCollapseAll);
        menuGroup->setAccel(QListView::CTRL + QListView::Key_Minus, mnuGrpCollapseAll);
        menuGroup->setItemEnabled(mnuGrpUp, i > 0);
        menuGroup->setItemEnabled(mnuGrpDown, i < pClient->contacts.groups.size() - 1);
        menuGroup->installEventFilter(this);
        menuGroup->popup(p);
        break;
    }
    clearSelection();
}

void UserView::maybeTip ( const QPoint &p )
{
    QPoint pos = mapToGlobal(p);
    UserViewItem *item = findUserItem(pos);
    if (item == NULL) return;
    CUser user(item->m_uin);
    tip(itemRect(item), user.toolTip());
}

cfgParam UserFloat_Params[] =
    {
        { "Uin", OFFSET_OF(UserFloat, Uin), PARAM_ULONG, 0 },
        { "Left", OFFSET_OF(UserFloat, Left), PARAM_SHORT, 0 },
        { "Top", OFFSET_OF(UserFloat, Top), PARAM_SHORT, 0 },
        { "", 0, 0, 0 }
    };

UserFloat::UserFloat()
        : UserView(NULL, false, false,
                   QObject::WType_TopLevel | QObject::WStyle_Customize | QObject::WStyle_NoBorder |
                   QObject::WStyle_Tool  | QObject::WStyle_StaysOnTop )
{
    ::init(this, UserFloat_Params);
    bFloaty = true;
    transparent = new TransparentTop(this, pMain->UseTransparent, pMain->Transparent);
    m_bShowOffline = true;
    setVScrollBarMode(AlwaysOff);
    bMoveMode = false;
    setSelectionMode(NoSelection);
}

UserFloat::~UserFloat()
{
    transparent = NULL;
}

bool UserFloat::setUin(unsigned long uin)
{
    ICQUser *u = pClient->getUser(uin);
    if (u == NULL) return false;
    addUserItem(u);
    QListViewItemIterator it(this);
    if (!it.current()) return false;
    QSize s;
    s.setWidth(it.current()->width(QFontMetrics(QFont()), this, 0) + 10);
    s.setHeight(it.current()->totalHeight() + 6);
    resize(s);
    Uin = uin;
    return true;
}

void UserFloat::save(QFile &s)
{
    Left = pos().x();
    Top = pos().y();
    ::save(this, UserFloat_Params, s);
}

bool UserFloat::load(QFile &s, string &nextPart)
{
    ::load(this, UserFloat_Params, s, nextPart);
    if (!setUin(Uin)){
        Uin = 0;
        return false;
    }
    move(Left, Top);
    show();
    return true;
}

void UserFloat::userChanged()
{
    QListViewItemIterator it(this);
    if (!it.current()) return;
    QSize s;
    s.setWidth(it.current()->width(QFontMetrics(QFont()), this, 0) + 10);
    s.setHeight(it.current()->totalHeight() + 2);
    resize(s);
}

void UserFloat::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
    if (bMoveMode){
        bMoveMode = false;
        viewport()->releaseMouse();
    }
    mousePos = QPoint();
    UserView::contentsMouseDoubleClickEvent(e);
}

void UserFloat::contentsMousePressEvent(QMouseEvent *e)
{
    if (e->button() == QObject::LeftButton){
        QRect rc(geometry());
        mousePos = e->globalPos() - rc.topLeft();
        Left = pos().x();
        Top = pos().y();
    }
    UserView::contentsMousePressEvent(e);
}

void UserFloat::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (bMoveMode){
        move(e->globalPos() - mousePos);
        bMoveMode = false;
        viewport()->releaseMouse();
        QPoint p(pos().x() - Left, pos().y() - Top);
        if (p.manhattanLength() > 6){
            clearSelection();
            mousePos = QPoint();
            return;
        }
    }
    mousePos = QPoint();
    UserView::contentsMouseReleaseEvent(e);
}

void UserFloat::contentsMouseMoveEvent(QMouseEvent *e)
{
    if (!mousePos.isNull() &&
            (QPoint(e->pos() - mousePos).manhattanLength() > QApplication::startDragDistance())){
        bMoveMode = true;
        viewport()->grabMouse();
    }
    if (bMoveMode)
        move(e->globalPos() - mousePos);
    UserView::contentsMouseMoveEvent(e);
}

void UserFloat::setBackgroundPixmap(const QPixmap &pm)
{
    if (transparent) transparent->updateBackground(pm);
}

#ifndef _WINDOWS
#include "userview.moc"
#endif

