/***************************************************************************
                          userbox.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
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

#include "userbox.h"
#include "msgview.h"
#include "msgedit.h"
#include "mainwin.h"
#include "client.h"
#include "cuser.h"
#include "log.h"
#include "toolbtn.h"
#include "transparent.h"
#include "icons.h"
#include "userview.h"

#include "ui/userinfo.h"

#include <qlayout.h>
#include <qframe.h>
#include <qtabbar.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qwidgetlist.h>

#if USE_KDE
#include <kwin.h>
#endif

class Splitter : public QSplitter
{
public:
    Splitter(QWidget *p) : QSplitter(p) {}
protected:
    virtual QSizePolicy sizePolicy() const;
};

QSizePolicy Splitter::sizePolicy() const
{
    return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

UserTab::UserTab(MsgEdit *wnd, QTabBar *bar, int index)
        : QTab(wnd->userName().replace(QRegExp("&"), "&&"))
{
    isBold = false;
    alias = wnd->userName();
    id = bar->insertTab(this, index);
    wnd->tabId = id;
    wnd->tab = this;
}

UserBox::UserBox(unsigned long grpId)
        : QMainWindow(NULL, NULL,
                      (WType_TopLevel | WStyle_Customize | WStyle_NormalBorder |
                       WStyle_Title | WStyle_SysMenu)
#ifdef USE_KDE
                      | (pMain->UserWindowInTaskManager() ? WStyle_Minimize : 0)
#endif
                     ),
        GrpId(this, "Group"),
        CurrentUser(this, "CurrentUser"),
        mLeft(this, "Left"),
        mTop(this, "Top"),
        mWidth(this, "Width"),
        mHeight(this, "Height"),
        ToolbarDock(this, "ToolbarDock", "Top"),
        ToolbarOffset(this, "ToolbarOffset"),
        ToolbarY(this, "ToolbarY")
{
    ToolbarDock = pMain->UserBoxToolbarDock();
    ToolbarOffset = pMain->UserBoxToolbarOffset();
    ToolbarY = pMain->UserBoxToolbarY();
    users = NULL;
    GrpId = grpId;
    msgView = NULL;
    setWFlags(WDestructiveClose);
    infoWnd = NULL;
    historyWnd = NULL;
    transparent = new TransparentTop(this, pMain->UseTransparentContainer, pMain->TransparentContainer);
    menuUser = new QPopupMenu(this);
    menuUser->setCheckable(true);
    connect(menuUser, SIGNAL(activated(int)), this, SLOT(selectedUser(int)));
    curWnd = NULL;
    frm = new QFrame(this);
    setCentralWidget(frm);
    lay = new QVBoxLayout(frm);
    vSplitter = new QSplitter(Horizontal, frm);
    vSplitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    lay->addWidget(vSplitter);
    splitter = new QSplitter(Vertical, vSplitter);
    modeChanged(pMain->SimpleMode());
    frmUser = new QFrame(splitter);
    layUser = new QVBoxLayout(frmUser);
    tabSplitter = new Splitter(frm);
    tabs = new UserTabBar(tabSplitter);
    status = new QStatusBar(tabSplitter);
    tabSplitter->setResizeMode(status, QSplitter::KeepSize);
    lay->addWidget(tabSplitter);
    setIcon(Pict(pClient->getStatusIcon()));
    connect(tabs, SIGNAL(selected(int)), this, SLOT(selectedUser(int)));
    connect(tabs, SIGNAL(showUserPopup(int, QPoint)), this, SLOT(showUserPopup(int, QPoint)));
    toolbar = new QToolBar(this);
    toolbar->setHorizontalStretchable(true);
    toolbar->setVerticalStretchable(true);
    menuType = new QPopupMenu(this);
    connect(menuType, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));
    btnType = new PictButton(toolbar);
    btnType->setPopup(menuType);
    btnType->setPopupDelay(0);
    toolbar->addSeparator();
    btnUser = new PictButton(toolbar);
    btnUser->setPopup(menuUser);
    btnUser->setPopupDelay(0);
    toolbar->addSeparator();
    btnIgnore = new QToolButton(toolbar);
    btnIgnore->setIconSet(Icon("ignorelist"));
    btnIgnore->setTextLabel(i18n("Add to ignore list"));
    connect(btnIgnore, SIGNAL(clicked()), this, SLOT(toIgnore()));
    menuGroup = new QPopupMenu(this);
    connect(menuGroup, SIGNAL(aboutToShow()), this, SLOT(showGrpMenu()));
    connect(menuGroup, SIGNAL(activated(int)), this, SLOT(moveUser(int)));
    btnGroup = new CToolButton(toolbar);
    btnGroup->setIconSet(Icon("grp_on"));
    btnGroup->setTextLabel(i18n("Move to group"));
    btnGroup->setPopup(menuGroup);
    btnGroup->setPopupDelay(0);
    toolbar->addSeparator();
    btnInfo = new QToolButton(toolbar);
    btnInfo->setIconSet(Icon("info"));
    btnInfo->setTextLabel(i18n("User info"));
    btnInfo->setToggleButton(true);
    connect(btnInfo, SIGNAL(toggled(bool)), this, SLOT(toggleInfo(bool)));
    btnHistory = new QToolButton(toolbar);
    btnHistory->setIconSet(Icon("history"));
    btnHistory->setTextLabel(i18n("History"));
    btnHistory->setToggleButton(true);
    connect(btnHistory, SIGNAL(toggled(bool)), this, SLOT(toggleHistory(bool)));
    btnQuit = new QToolButton(Icon("exit"), i18n("Close"), "", this, SLOT(quit()), toolbar);
    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(pClient, SIGNAL(messageRead(ICQMessage*)), this, SLOT(messageRead(ICQMessage*)));
    connect(pClient, SIGNAL(messageReceived(ICQMessage*)), this, SLOT(messageReceived(ICQMessage*)));
    connect(pMain, SIGNAL(iconChanged()), this, SLOT(iconChanged()));
    connect(pMain, SIGNAL(wmChanged()), this, SLOT(wmChanged()));
    connect(pMain, SIGNAL(modeChanged(bool)), this, SLOT(modeChanged(bool)));
    connect(this, SIGNAL(toolBarPositionChanged(QToolBar*)), this, SLOT(toolBarChanged(QToolBar*)));
    setGroupButtons();
    wmChanged();
    adjustPos();
    adjustToolbar();
}

void UserBox::wmChanged()
{
#if USE_KDE
    if (pMain->UserWindowInTaskManager()){
        KWin::clearState(winId(), NET::SkipTaskbar);
    }else{
        KWin::setState(winId(), NET::SkipTaskbar);
    }
#endif
}

void UserBox::showUsers(bool bShow, unsigned long uin)
{
    if (bShow){
        if (users == NULL){
            QWidget *fw = qApp->focusWidget();
            if (fw) fw->releaseMouse();
            users = new UserView(vSplitter, true, false);
            vSplitter->setResizeMode(users, QSplitter::KeepSize);
            users->show();
            users->fill();
            if (uin) users->check(uin);
            users->setFocus();
        }
        connect(users, SIGNAL(checked()), curWnd, SLOT(textChanged()));
    }else{
        if (users == NULL) return;
        delete users;
        users = NULL;
    }
}

void UserBox::setBackgroundPixmap(const QPixmap &pm)
{
    transparent->updateBackground(pm);
}

void UserBox::iconChanged()
{
    ICQUser *u = pClient->getUser(curWnd->Uin);
    if (u){
        pMain->adjustUserMenu(menuType, u, true);
        btnType->setPopup(menuType);
        btnUser->setState(Client::getUserIcon(u), curWnd->userName());
        setIcon(Pict(Client::getUserIcon(u)));
    }
    adjustUserMenu(true);
}

unsigned UserBox::count()
{
    return wnds.size();
}

void UserBox::moveUser(int grp)
{
    if (curWnd == NULL) return;
    pMain->m_uin = curWnd->Uin();
    pMain->moveUser(grp);
}

void UserBox::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    QTimer::singleShot(50, this, SLOT(setGroupButtons()));
}

void UserBox::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    pMain->UserBoxWidth = size().width();
    pMain->UserBoxHeight = size().height();
}

void UserBox::moveEvent(QMoveEvent *e)
{
    QMainWindow::moveEvent(e);
    pMain->UserBoxX = pos().x();
    pMain->UserBoxY = pos().y();
}

QString UserBox::containerName()
{
    if (GrpId == ContainerAllUsers)
        return i18n("All users");
    if (GrpId < 0x10000){
        ICQGroup *grp = pClient->getGroup(GrpId);
        if (grp){
            CGroup g(grp);
            return g.name();
        }
    }
    if (curWnd == NULL) return "???";
    CUser u(curWnd->Uin);
    return u.name();
}

UserBox::~UserBox()
{
    removeChilds();
    if (pMain) pMain->destroyBox(this);
}

void UserBox::saveInfo(ICQUser *u)
{
    if ((curWnd == NULL) || curWnd->Uin()) return;
    curWnd->setUin(u->Uin());
    ICQEvent e(EVENT_INFO_CHANGED, u->Uin());
    processEvent(&e);
    pMain->adjustUserMenu(menuType, u, false);
    btnType->setPopup(menuType);
    btnUser->setState(Client::getUserIcon(u), curWnd->userName());
    setIcon(Pict(Client::getUserIcon(u)));
    setGroupButtons();
}

void UserBox::toggleInfo(bool bShow)
{
    bool oldState = isUpdatesEnabled();
    if (bShow){
        if (curWnd == NULL) return;
        setUpdatesEnabled(false);
        btnHistory->setOn(false);
        disconnect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
        btnType->setState("info", i18n("User info"));
        if (infoWnd == NULL){
            infoWnd = new UserInfo(frm, curWnd->Uin);
            connect(infoWnd, SIGNAL(saveInfo(ICQUser*)), this, SLOT(saveInfo(ICQUser*)));
            splitter->hide();
            lay->insertWidget(0, infoWnd);
            infoWnd->show();
        }
    }else if (infoWnd){
        if (curWnd && (curWnd->Uin() == 0)){
            curWnd->close();
            return;
        }
        setUpdatesEnabled(false);
        delete infoWnd;
        infoWnd = NULL;
        splitter->show();
        if (curWnd){
            connect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
            curWnd->action(mnuAction);
        }
    }else{
        oldState = false;
    }
    if (oldState){
        setUpdatesEnabled(true);
        repaint();
    }
}

void UserBox::hideHistory()
{
    btnHistory->setOn(false);
    curWnd->showMessage(msgShowId);
}

void UserBox::toggleHistory(bool bShow)
{
    bool oldState = isUpdatesEnabled();
    if (bShow){
        if ((curWnd == NULL) || (curWnd->Uin() == 0)) return;
        setUpdatesEnabled(false);
        btnInfo->setOn(false);
        disconnect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
        btnType->setState("history", i18n("History"));
        if (historyWnd == NULL){
            historyWnd = new HistoryView(frm, curWnd->Uin);
            connect(historyWnd, SIGNAL(goMessage(unsigned long, unsigned long)), this, SLOT(showMessage(unsigned long, unsigned long)));
            splitter->hide();
            historyWnd->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
            lay->insertWidget(0, historyWnd);
            historyWnd->show();
        }
    }else if (historyWnd){
        setUpdatesEnabled(false);
        delete historyWnd;
        historyWnd = NULL;
        splitter->show();
        if (curWnd){
            connect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
            curWnd->action(mnuAction);
        }
    }else{
        oldState = false;
    }
    if (oldState){
        setUpdatesEnabled(true);
        repaint();
    }
}

void UserBox::typeChanged(int)
{
    btnHistory->setOn(false);
    btnInfo->setOn(false);
}

void UserBox::quit()
{
    btnInfo->setOn(false);
    btnHistory->setOn(false);
    MsgEdit *wnd = getWnd(tabs->currentTab());
    closeUser(wnd->Uin());
}

void UserBox::removeChilds()
{
    list<MsgEdit*>::iterator it;
    for (it = wnds.begin(); it != wnds.end(); it = wnds.begin()){
        MsgEdit *wnd = *it;
        wnds.remove(wnd);
        tabs->removeTab(wnd->tab);
        delete wnd;
    }
}

void UserBox::destroyChild(int id)
{
    MsgEdit *wnd = getWnd(id);
    if (wnd == NULL)
        return;
    if (wnd == curWnd) curWnd = NULL;
    menuUser->removeItem(id);
    btnUser->setPopup(menuUser);
    tabs->removeTab(wnd->tab);
    wnds.remove(wnd);
    if (curWnd) return;
    if (wnds.begin() == wnds.end()){
        QTimer::singleShot(10, this, SLOT(close()));
        return;
    }
    selectedUser((*wnds.begin())->tabId);
}

MsgEdit *UserBox::getWnd(int id)
{
    if (id == -1) return NULL;
    list<MsgEdit*>::iterator it;
    for (it = wnds.begin(); it != wnds.end(); it++){
        if ((*it)->tabId == id) return *it;
    }
    return NULL;
}

bool UserBox::haveUser(unsigned long uin)
{
    return (getChild(uin, false) != NULL);
}

MsgEdit *UserBox::getChild(unsigned long uin, bool bCreate)
{
    list<MsgEdit*>::iterator it;
    if (uin){
        for (it = wnds.begin(); it != wnds.end(); it++){
            MsgEdit *e = *it;
            if (e->Uin == uin) return *it;
        }
    }
    if (!bCreate) return NULL;
    MsgEdit *wnd = new MsgEdit(frmUser, uin);
    layUser->insertWidget(0, wnd);
    wnd->hide();
    wnds.push_back(wnd);
    connect(wnd, SIGNAL(destroyChild(int)), this, SLOT(destroyChild(int)));
    if (msgView) msgView->addUnread(uin);
    return wnd;
}

void UserBox::save(std::ostream &s)
{
    mLeft = pos().x();
    mTop = pos().y();
    mWidth = size().width();
    mHeight = size().height();
    MsgEdit *wnd = getWnd(tabs->currentTab());
    if (wnd) CurrentUser = wnd->Uin;
    getToolbarPosition();
    ConfigArray::save(s);
    list<MsgEdit*>::iterator it;
    for (it = wnds.begin(); it != wnds.end(); it++){
        s << "[UserTab]\n";
        (*it)->save(s);
    }
}

void UserBox::getToolbarPosition()
{
    ToolBarDock tDock;
    int index;
    bool nl;
    int extraOffset;
    getLocation(toolbar, tDock, index, nl, extraOffset);
    ToolbarOffset = (short)extraOffset;
    switch (tDock){
    case Minimized:
        ToolbarDock = "Minimized";
        break;
    case Bottom:
        ToolbarDock = "Bottom";
        break;
    case Left:
        ToolbarDock = "Left";
        break;
    case Right:
        ToolbarDock = "Right";
        break;
    case TornOff:
        ToolbarDock = "TornOff";
        ToolbarOffset = toolbar->geometry().left();
        ToolbarY = toolbar->geometry().top();
        break;
    default:
        ToolbarDock = "Top";
    }
}

void UserBox::toolBarChanged(QToolBar*)
{
    getToolbarPosition();
    pMain->UserBoxToolbarDock = ToolbarDock;
    if (ToolbarDock == "TornOff"){
        pMain->UserBoxToolbarOffset = ToolbarOffset();
        pMain->UserBoxToolbarY = ToolbarY();
    }
}

bool UserBox::load(std::istream &s, string &part)
{
    ConfigArray::load(s, part);
    int curTab = -1;
    for (;;){
        if (part.size() == 0) break;
        if (part != string("[UserTab]"))
            break;
        MsgEdit *wnd = getChild(0);
        if (wnd->load(s, part) && (wnd->tab == NULL)){
            new UserTab(wnd, tabs);
            if (wnd->Uin == CurrentUser) curTab = wnd->tabId;
            btnUser->setPopup(menuUser);
            continue;
        }
        wnds.remove(wnd);
    }
    adjustUserMenu(true);
    lay->invalidate();
    if (tabs->count() == 0) return false;
    adjustPos();
    if (curTab != -1) selectedUser(curTab);
    adjustToolbar();
    setShow();
    return true;
}

void UserBox::adjustToolbar()
{
    ToolBarDock tDock = Top;
    if (ToolbarDock == "Minimized"){
        tDock = Minimized;
    }else if (ToolbarDock == "Bottom"){
        tDock = Bottom;
    }else if (ToolbarDock == "Left"){
        tDock = Left;
    }else if (ToolbarDock == "Right"){
        tDock = Right;
    }else if (ToolbarDock == "TornOff"){
        tDock = TornOff;
    }
    moveToolBar(toolbar, tDock, false, 0, ToolbarOffset);
    if (tDock == TornOff){
        toolbar->move(ToolbarOffset, ToolbarY);
        toolbar->show();
    }
}

void UserBox::adjustPos()
{
    bool bReposition = false;
    if ((mLeft() == 0) && (mTop() == 0)){
        if ((pMain->UserBoxX() == 0) && (pMain->UserBoxY() == 0)){
            mLeft = pos().x();
            mTop  = pos().y();
        }else{
            mLeft = pMain->UserBoxX();
            mTop  = pMain->UserBoxY();
            bReposition = true;
        }
    }
    if ((mWidth() == 0) || (mHeight() == 0)){
        mWidth  = (short)pMain->UserBoxWidth();
        mHeight = (short)pMain->UserBoxHeight();
    }
    if (mLeft() < 5) mLeft = 5;
    if (mTop() < 5) mTop = 5;
    if (mLeft > QApplication::desktop()->width() - 5) mLeft = QApplication::desktop()->width() - 5;
    if (mTop > QApplication::desktop()->height() - 5) mTop = QApplication::desktop()->height() - 5;
    if (mWidth > QApplication::desktop()->width() - 5) mWidth = QApplication::desktop()->width() - 5;
    if (mHeight > QApplication::desktop()->height() - 5) mHeight = QApplication::desktop()->height() - 5;
    if (bReposition){
        for (unsigned n = 0; n < 32; n++){
            if (mLeft() > QApplication::desktop()->width() - mWidth())
                mLeft = 0;
            if (mTop() > QApplication::desktop()->height() - mHeight())
                mTop = 0;
            if (mLeft() < 0)
                mLeft = 0;
            if (mTop() < 0)
                mTop  = 0;
            bool bOK = true;
            QWidgetList *list = QApplication::topLevelWidgets();
            QWidgetListIt it( *list );
            QWidget *w;
            while ((w=it.current()) != 0){
                ++it;
                if (w == this) continue;
                if (!w->inherits("UserBox")) continue;
                if ((w->x() == mLeft()) && (w->y() == mTop())){
                    bOK = false;
                    break;
                }
            }
            delete list;
            if (bOK) break;
            mLeft += 23;
            mTop += 22;
        }
    }
    if (mLeft() && mTop()){
        short saveX = pMain->UserBoxX();
        short saveY = pMain->UserBoxY();
        move(mLeft, mTop);
        pMain->UserBoxX = saveX;
        pMain->UserBoxY = saveY;
    }
    if (mWidth() && mHeight()){
        unsigned short saveWidth = pMain->UserBoxWidth();
        unsigned short saveHeight = pMain->UserBoxHeight();
        resize(mWidth, mHeight);
        pMain->UserBoxWidth  = saveWidth;
        pMain->UserBoxHeight = saveHeight;
    }
}

void UserBox::messageRead(ICQMessage *msg)
{
    ICQUser *u = pClient->getUser(msg->getUin());
    if (u == NULL) return;
    if (u->unreadMsgs.size()) return;
    MsgEdit *wnd = getChild(msg->getUin(), false);
    if (wnd == NULL) return;
    UserTab *tab = wnd->tab;
    if (tab == NULL) return;
    tab->isBold = false;
    tabs->repaint();
}

void UserBox::toIgnore()
{
    if (curWnd == NULL) return;
    QPoint p = btnIgnore->mapToGlobal(btnIgnore->rect().topLeft());
    QRect rc(p.x(), p.y(), btnIgnore->width(), btnIgnore->height());
    pMain->m_uin = curWnd->Uin();
    pMain->m_rc = rc;
    pMain->moveUser(mnuGroupIgnore);
}

void UserBox::messageReceived(ICQMessage *msg)
{
    if (msgView == NULL) return;
    if (getChild(msg->getUin(), false) == NULL) return;
    bool bUnread = false;
    ICQUser *u = pClient->getUser(msg->getUin());
    if (u){
        for (list<unsigned long>::iterator it = u->unreadMsgs.begin(); it != u->unreadMsgs.end(); it++){
            if ((*it) == msg->Id){
                bUnread = true;
                break;
            }
        }
        if (curWnd && (msg->getUin() == curWnd->Uin()) &&
                (qApp->activeWindow() == this) &&
                ((msg->Type() == ICQ_MSGxMSG) || (msg->Type() == ICQ_MSGxURL) ||
                 (msg->Type() == ICQ_MSGxSMS))){
            bUnread = false;
            pClient->markAsRead(msg);
        }
        msgView->addMessage(msg, bUnread, true);
    }
}

void UserBox::processEvent(ICQEvent *e)
{
    switch (e->type()){
    case EVENT_MESSAGE_RECEIVED:{
            if (e->state == ICQEvent::Fail) break;
            ICQMessage *msg = e->message();
            if (msg == NULL) break;
            MsgEdit *wnd = getChild(msg->getUin(), false);
            if (wnd == NULL) break;
            UserTab *tab = wnd->tab;
            if (tab == NULL) break;
            ICQUser *u = pClient->getUser(msg->getUin());
            if (u == NULL) break;
            list<unsigned long>::iterator it;
            for (it = u->unreadMsgs.begin(); it != u->unreadMsgs.end(); it++)
                if ((*it) == msg->Id) break;
            if (it == u->unreadMsgs.end()) break;

            tab->isBold = true;
            tabs->repaint();
            break;
        }
    case EVENT_USER_DELETED:{
            MsgEdit *wnd = getChild(e->Uin(), false);
            if (wnd == NULL) break;
            wnd->close();
            break;
        }
    case EVENT_USERGROUP_CHANGED:
        if (curWnd && (e->Uin() == curWnd->Uin()))
            setGroupButtons();
        break;
    case EVENT_INFO_CHANGED:{
            if (curWnd && (e->Uin() == curWnd->Uin()))
                setGroupButtons();
            statusChanged(e->Uin());
            MsgEdit *wnd = getChild(e->Uin(), false);
            if (wnd == NULL) break;
            UserTab *tab = wnd->tab;
            if (tab == NULL) break;
            CUser u(e->Uin());
            if (u.name() == tab->alias) break;
            list<MsgEdit*>::iterator it;
            int index = 0;
            for (it = wnds.begin(); it != wnds.end(); it++, index++)
                if ((*it)->Uin == e->Uin()) break;
            tabs->removeTab(tab);
            new UserTab(wnd, tabs, index);
            adjustUserMenu(true);
            break;
        }
    case EVENT_STATUS_CHANGED:{
            if (e->Uin() == pClient->Uin())
                setGroupButtons();
            statusChanged(e->Uin());
            break;
        }
    }
}

void UserBox::setGroupButtons()
{
    ICQUser *u = NULL;
    if (curWnd) u = pClient->getUser(curWnd->Uin);
    if (u == NULL){
        btnIgnore->hide();
        btnGroup->hide();
        return;
    }
    btnGroup->show();
    if (u->GrpId()){
        btnIgnore->hide();
    }else{
        btnIgnore->show();
    }
    if (u->Type == USER_TYPE_ICQ){
        btnIgnore->setEnabled(pClient->m_state == ICQClient::Logged);
        btnGroup->setEnabled(pClient->m_state == ICQClient::Logged);
    }else{
        btnIgnore->setEnabled(true);
        btnGroup->setEnabled(true);
    }
    if ((tabs->count() > 1) && !tabs->isVisible() && isVisible()) tabs->show();
}

void UserBox::statusChanged(unsigned long uin)
{
    MsgEdit *wnd = getWnd(tabs->currentTab());
    if (wnd == NULL) return;
    if (wnd->Uin != uin) return;
    ICQUser *u = pClient->getUser(curWnd->Uin);
    if (u){
        btnUser->setState(Client::getUserIcon(u), curWnd->userName());
        setIcon(Pict(Client::getUserIcon(u)));
        pMain->adjustUserMenu(menuType, u, false);
        btnType->setPopup(menuType);
    }
}

unsigned long UserBox::currentUser()
{
    if (curWnd == NULL) return 0;
    return curWnd->Uin;
}

void UserBox::selectedUser(int id)
{
    if (tabs->count() <= 1){
        if (tabs->isVisible()) tabs->hide();
    }else{
        if (!tabs->isVisible() && isVisible()) tabs->show();
    }
    MsgEdit *wnd = getWnd(id);
    if (wnd == NULL){
        log(L_WARN, "User for %u not found");
        return;
    }
    if (curWnd == wnd) return;
    tabs->setCurrentTab(wnd->tabId);
    setCaption(wnd->userName());
    if (curWnd){
        btnHistory->setOn(false);
        btnInfo->setOn(false);
        curWnd->hide();
        disconnect(menuType, SIGNAL(activated(int)), curWnd, SLOT(action(int)));
        disconnect(curWnd, SIGNAL(setStatus(const QString&)), status, SLOT(message(const QString&)));
        disconnect(curWnd, SIGNAL(setStatus(const QString&, int)), status, SLOT(message(const QString&, int)));
        disconnect(curWnd, SIGNAL(setSendState(bool)), btnType, SLOT(setEnabled(bool)));
        disconnect(curWnd, SIGNAL(setSendState(bool)), btnQuit, SLOT(setEnabled(bool)));
        disconnect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
        if (msgView){
            disconnect(curWnd, SIGNAL(showMessage(unsigned long, unsigned long)), msgView, SLOT(setMessage(unsigned long, unsigned long)));
            disconnect(curWnd, SIGNAL(addMessage(ICQMessage*, bool, bool)), msgView, SLOT(addMessage(ICQMessage*, bool, bool)));
        }
        showUsers(false, 0);
    }
    status->message("");
    curWnd = wnd;
    connect(menuType, SIGNAL(activated(int)), curWnd, SLOT(action(int)));
    connect(curWnd, SIGNAL(setStatus(const QString&)), status, SLOT(message(const QString&)));
    connect(curWnd, SIGNAL(setStatus(const QString&, int)), status, SLOT(message(const QString&, int)));
    connect(curWnd, SIGNAL(setSendState(bool)), btnType, SLOT(setEnabled(bool)));
    connect(curWnd, SIGNAL(setSendState(bool)), btnQuit, SLOT(setEnabled(bool)));
    connect(curWnd, SIGNAL(setMessageType(const QString&, const QString&)), btnType, SLOT(setState(const QString&, const QString&)));
    if (msgView){
        connect(curWnd, SIGNAL(showMessage(unsigned long, unsigned long)), msgView, SLOT(setMessage(unsigned long, unsigned long)));
        connect(curWnd, SIGNAL(addMessage(ICQMessage*, bool, bool)), msgView, SLOT(addMessage(ICQMessage*, bool, bool)));
    }
    connect(curWnd, SIGNAL(showUsers(bool, unsigned long)), this, SLOT(showUsers(bool, unsigned long)));
    curWnd->markAsRead();
    curWnd->show();
    curWnd->setState();
    lay->invalidate();
    ICQUser *u = pClient->getUser(curWnd->Uin);
    if (u){
        pMain->adjustUserMenu(menuType, u, false);
        btnType->setPopup(menuType);
        btnUser->setState(Client::getUserIcon(u), curWnd->userName());
        setIcon(Pict(Client::getUserIcon(u)));
    }
    showUsers(curWnd->bMultiply, curWnd->Uin);
    setGroupButtons();
    adjustUserMenu(false);
}

void UserBox::adjustUserMenu(bool bRescan)
{
    if (bRescan) menuUser->clear();
    list<MsgEdit*>::iterator it;
    for (it = wnds.begin(); it != wnds.end(); it++){
        if (bRescan) menuUser->insertItem((*it)->userName(), (*it)->tabId);
        menuUser->setItemChecked((*it)->tabId, (*it)->tabId == tabs->currentTab());
    }
    btnUser->setPopup(menuUser);
}

bool UserBox::closeUser(unsigned long uin)
{
    MsgEdit *wnd = getChild(uin, false);
    if (wnd == NULL) return false;
    wnd->close();
    if (msgView) msgView->deleteUser(uin);
    return true;
}

void UserBox::showUser(unsigned long uin, int function, unsigned long param)
{
    MsgEdit *wnd = getChild(uin);
    if (wnd->tab == NULL){
        new UserTab(wnd, tabs);
        menuUser->insertItem(wnd->userName(), wnd->tabId);
    }
    tabs->setCurrentTab(wnd->tabId);
    if (curWnd == NULL) selectedUser(wnd->tabId);
    lay->invalidate();
    switch (function){
    case mnuInfo:
        btnInfo->setOn(true);
        break;
    case mnuHistory:
        btnHistory->setOn(true);
        break;
    default:
        btnInfo->setOn(false);
        btnHistory->setOn(false);
        wnd->action(function);
        if (param) wnd->setParam(param);
    }
    setShow();
}

void UserBox::setShow()
{
    show();
    showNormal();
#if USE_KDE
    KWin::setOnDesktop(winId(), KWin::currentDesktop());
#endif
    raise();
#if USE_KDE
    KWin::setActiveWindow(winId());
#endif
}

bool UserBox::isShow()
{
    if (!isVisible()) return false;
#if USE_KDE
    return (KWin::info(winId()).desktop == KWin::currentDesktop())
           || KWin::info(winId()).onAllDesktops;
#else
    return true;
#endif
}

bool UserBox::event(QEvent *e)
{
    if ((e->type() == QEvent::WindowActivate) && curWnd)
        curWnd->markAsRead();
    return QMainWindow::event(e);
}

void UserBox::showUserPopup(int id, QPoint pos)
{
    MsgEdit *wnd = getWnd(id);
    if (wnd == NULL){
        log(L_WARN, "Popup not found %u", id);
        return;
    }
    QPoint p(0, 0);
    p = tabs->mapToGlobal(p);
    QRect rc(pos.x(), p.y(), 0, tabs->height());
    pMain->showUserPopup(wnd->Uin, pos, menuUser, rc);
}

void UserBox::showMessage(unsigned long uin, unsigned long id)
{
    MsgEdit *wnd = getChild(uin);
    if (wnd == NULL) return;
    msgShowId = id;
    QTimer::singleShot(10, this, SLOT(hideHistory()));
}

void UserBox::showGrpMenu()
{
    if (curWnd == NULL) return;
    pMain->adjustGroupMenu(menuGroup, curWnd->Uin);
}

void UserBox::modeChanged(bool bSimple)
{
    if (bSimple){
        if (msgView){
            delete msgView;
            msgView = NULL;
        }
        return;
    }
    if (msgView) return;
    msgView = new MsgView(splitter);
    splitter->moveToFirst(msgView);
    connect(msgView, SIGNAL(goMessage(unsigned long, unsigned long)), this, SLOT(showMessage(unsigned long, unsigned long)));
    if (curWnd && msgView){
        connect(curWnd, SIGNAL(showMessage(unsigned long, unsigned long)), msgView, SLOT(setMessage(unsigned long, unsigned long)));
        connect(curWnd, SIGNAL(addMessage(ICQMessage*, bool, bool)), msgView, SLOT(addMessage(ICQMessage*, bool, bool)));
    }
    if (splitter->isVisible())
        msgView->show();
    splitter->setResizeMode(msgView, QSplitter::KeepSize);
}

UserTabBar::UserTabBar(QWidget *parent) : QTabBar(parent)
{
    setShape(QTabBar::TriangularBelow);
}

void UserTabBar::paintLabel(QPainter *p, const QRect &rc, QTab *t, bool bFocusRect) const
{
    UserTab *tab = static_cast<UserTab*>(t);
    if (tab->isBold){
        QFont f = font();
        f.setBold(true);
        p->setFont(f);
    }
    QTabBar::paintLabel(p, rc, t, bFocusRect);
}

void UserTabBar::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == RightButton){
        QTab *t = selectTab(e->pos());
        if (t == NULL) return;
        UserTab *tab = static_cast<UserTab*>(t);
        emit showUserPopup(tab->id, e->globalPos());
        return;
    }
    QTabBar::mousePressEvent(e);
}

#ifndef _WINDOWS
#include "userbox.moc"
#endif

