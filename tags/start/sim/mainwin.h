/***************************************************************************
                          mainwin.h  -  description
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

#ifndef _MAINWIN_H
#define _MAINWIN_H

#include "defs.h"

#include <fstream>

#include <qmainwindow.h>
#include <qtimer.h>

#include "cfg.h"

class Skin;
class DockWnd;
class KPopupMenu;
class UserView;
class CToolButton;
class PictButton;
class QTimer;
class UserFloat;
class UserBox;
class QPopupMenu;
class QToolBar;
class Themes;
class SearchDialog;
class SetupDialog;
class ICQMessage;
class ICQUser;
class TransparentTop;

const int mnuAction = 0;
const int mnuMessage = 1;
const int mnuURL = 2;
const int mnuSMS = 3;
const int mnuFile = 4;
const int mnuContacts = 5;
const int mnuAuth = 6;
const int mnuChat = 7;
const int mnuFloating = 10;
const int mnuClose = 11;
const int mnuInfo = 12;
const int mnuHistory = 13;
const int mnuSearch = 14;
const int mnuDelete = 15;
const int mnuGroups = 16;
const int mnuMail = 17;
const int mnuContainers = 18;
const int mnuActionInt = 19;
const int mnuGrpCreate = 21;
const int mnuGrpCollapseAll = 22;
const int mnuGrpExpandAll = 23;
const int mnuGrpRename = 24;
const int mnuGrpDelete = 25;
const int mnuGrpUp = 26;
const int mnuGrpDown = 27;
const int mnuAbout = 30;
const int mnuAboutKDE = 31;
const int mnuGrpTitle = 0x10000;

const unsigned long mnuGroupVisible = 0x10001;
const unsigned long mnuGroupInvisible = 0x10002;
const unsigned long mnuGroupIgnore = 0x10003;
const unsigned long mnuOnTop = 0x10004;

class ICQEvent;
class QDialog;
class KAboutKDE;

class MainWindow : public QMainWindow, public ConfigArray
{
    Q_OBJECT
public:
    MainWindow(const char *name = NULL);
    ~MainWindow();

    ConfigBool   Show;
    ConfigBool	 OnTop;

    ConfigBool   ShowOffline;
    ConfigBool   GroupMode;

    ConfigShort  mLeft;
    ConfigShort  mTop;
    ConfigShort  mWidth;
    ConfigShort  mHeight;

    ConfigString UseStyle;

    ConfigULong  AutoAwayTime;
    ConfigULong  AutoNATime;

    ConfigULong  ManualStatus;
    ConfigShort  DivPos;

    ConfigBool   SpellOnSend;

    ConfigString ToolbarDock;
    ConfigShort	 ToolbarOffset;
    ConfigShort  ToolbarY;

    ConfigString IncomingMessage;
    ConfigString IncomingURL;
    ConfigString IncomingSMS;
    ConfigString IncomingAuth;
    ConfigString IncomingFile;
    ConfigString IncomingChat;
    ConfigString FileDone;
    ConfigString OnlineAlert;
    ConfigString BirthdayReminder;

    ConfigString UrlViewer;
    ConfigString MailClient;
    ConfigString SoundPlayer;

    ConfigBool   UseTransparent;
    ConfigULong  Transparent;
    ConfigBool	 UseTransparentContainer;
    ConfigULong	 TransparentContainer;

    ConfigBool	 NoShowAway;
    ConfigBool	 NoShowNA;
    ConfigBool	 NoShowOccupied;
    ConfigBool	 NoShowDND;
    ConfigBool	 NoShowFFC;

    ConfigBool	 UseSystemFonts;

    ConfigString FontFamily;
    ConfigUShort FontSize;
    ConfigUShort FontWeight;
    ConfigBool   FontItalic;

    ConfigString FontMenuFamily;
    ConfigUShort FontMenuSize;
    ConfigUShort FontMenuWeight;
    ConfigBool   FontMenuItalic;

    ConfigULong	 ColorSend;
    ConfigULong	 ColorReceive;

    ConfigShort	 ChatWidth;
    ConfigShort  ChatHeight;

    ConfigBool	 CloseAfterSend;
    ConfigBool	 UserWindowInTaskManager;

    ConfigBool	 NotInListExpand;

    ConfigString Icons;

    ConfigBool   XOSD_on;
    ConfigShort  XOSD_pos;
    ConfigShort  XOSD_offset;
    ConfigULong  XOSD_color;
    ConfigString XOSD_font;
    ConfigUShort XOSD_timeout;

    bool 	     init();

    QPopupMenu   *menuStatus;
    QPopupMenu	 *menuPhone;
    QPopupMenu	 *menuPhoneLocation;
    QPopupMenu	 *menuPhoneStatus;
    KPopupMenu   *menuFunction;
    KPopupMenu   *menuUser;
    QPopupMenu	 *menuGroup;
    QPopupMenu   *menuContainers;
    QPopupMenu   *menuGroups;

    UserView     *users;

    CToolButton  *btnShowOffline;
    CToolButton  *btnGroupMode;
    PictButton   *btnStatus;
    Themes *themes;

    void setShow(bool bState);
    bool isShow();
    void buildFileName(string &s, const char *name);
    void playSound(const char *wav);

    string homeDir;

    void adjustUserMenu(QPopupMenu *menu, ICQUser *u, bool bHaveTitle);
    void adjustGroupMenu(QPopupMenu *menu, unsigned long uin);
    void destroyBox(UserBox*);
    unsigned long m_uin;
    QRect m_rc;

    void setFonts();
    void changeColors();
    void changeWm();

signals:
    void transparentChanged();
    void colorsChanged();
    void setupInit();
    void iconChanged();
    void wmChanged();
public slots:
    void quit();
    void setup();
    void phonebook();
    void search();
    void toggleShow();
    void showPopup(QPoint);
    void saveState();
    void toggleGroupMode();
    void toggleShowOffline();
    void setGroupMode(bool);
    void setShowOffline(bool);
    void setStatus(int);
    void showUserPopup(unsigned long uin, QPoint, QPopupMenu*, const QRect&);
    void userFunction(int);
    void userFunction(unsigned long uin, int, unsigned long param=0);
    void goURL(const char*);
    void sendMail(unsigned long);
    void toggleOnTop();
    void moveUser(int);
    void changeTransparent();
    void changeIcons(int);
protected slots:
    void realSetStatus();
    void autoAway();
    void setToggle();
    void blink();
    void processEvent(ICQEvent*);
    void dockDblClicked();
    void setPhoneLocation(int);
    void setPhoneStatus(int);
    void toContainer(int);
    void addNonIM();
    void showGroupPopup(QPoint);
    void deleteUser(int);
    void ignoreUser(int);
    void about();
    void about_kde();
    void dialogFinished();
    void timerExpired();
protected:
    char realTZ;

    int lockFile;

    bool bBlinkState;

    list<UserFloat*> floating;
    UserFloat *findFloating(unsigned long uin, bool bDelete = false);

    bool m_bAutoAway, m_bAutoNA;
    int  m_autoStatus;
    bool noToggle;
    QTimer *autoAwayTimer;
    QTimer *blinkTimer;
    void setIcons();
    void setDock(bool bState);
    unsigned long uinMenu;
    void setOnTop();
    void ownerChanged();
    void saveContacts();
    void setStatusIcon(QPixmap *p);
    void addStatusItem(int status);
    void setStatusItem(int status);
    bool	bQuit;
    DockWnd	*dock;
    virtual void closeEvent(QCloseEvent*);
    void addMessageType(QPopupMenu *menu, int type, int id, bool bAdd, bool bHaveTitle);
    QToolBar *toolbar;
    list<UserBox*> containers;
    SearchDialog *searchDlg;
    SetupDialog *setupDlg;
    bool bInLogin;

    void loadMenu();
    void showUser(unsigned long uin, int function, unsigned long param=0);
    void closeUser(unsigned long uin);

    TransparentTop *transparent;
#if USE_KDE
    QDialog      *mAboutApp;
    KAboutKDE    *mAboutKDE;
#endif
};

extern MainWindow *pMain;

#endif

