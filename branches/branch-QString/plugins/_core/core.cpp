/***************************************************************************
                          core.cpp  -  description
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

#include "core.h"
#include "simapi.h"
#include "cfgdlg.h"
#include "mainwin.h"
#include "userview.h"
#include "commands.h"
#include "usercfg.h"
#include "interfacecfg.h"
#include "search.h"
#include "logindlg.h"
#include "newprotocol.h"
#include "status.h"
#include "statuswnd.h"
#include "manager.h"
#include "connectionsettings.h"
#include "container.h"
#include "userwnd.h"
#include "msgedit.h"
#include "fontedit.h"
#include "history.h"
#include "historywnd.h"
#include "msgcfg.h"
#include "smscfg.h"
#include "tmpl.h"
#include "autoreply.h"
#include "textshow.h"
#include "filetransfer.h"
#include "declinedlg.h"
#include "xsl.h"
#include "userhistorycfg.h"
#include "ballonmsg.h"
#include "icons.h"
#include "kdeisversion.h"

#include <qtimer.h>
#include <qapplication.h>
#include <qwidgetlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qthread.h>
#include <qtextcodec.h>
#include <qprocess.h>

#include <time.h>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

using namespace std;
using namespace SIM;

#ifdef WIN32

class LockThread : public QThread
{
public:
    LockThread(HANDLE hEvent);
    HANDLE hEvent;
protected:
    void run();
};

#endif

class FileLock : public QFile
{
public:
    FileLock(const QString &name);
    ~FileLock();
    bool lock(bool bSend);
protected:
#ifdef WIN32
    LockThread	*m_thread;
#else
    bool m_bLock;
#endif
};

Plugin *createCorePlugin(unsigned base, bool, ConfigBuffer *config)
{
    Plugin *plugin = new CorePlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Interface"),
        I18N_NOOP("System interface"),
        VERSION,
        createCorePlugin,
        PLUGIN_DEFAULT | PLUGIN_NODISABLE | PLUGIN_RELOAD
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

#if !defined(WIN32) && !defined(USE_KDE)

struct loaded_domain;

struct loaded_l10nfile
{
    const char *filename;
    int decided;
    const void *data;
    loaded_l10nfile() : filename(0), decided(0), data(0) {}
};


void k_nl_load_domain (struct loaded_l10nfile *domain_file);
void k_nl_unload_domain (struct loaded_domain *domain);
char *k_nl_find_msg (struct loaded_l10nfile *domain_file, const char *msgid);

class SIMTranslator : public QTranslator
{
public:
    SIMTranslator(QObject *parent, const QString &filename);
    ~SIMTranslator();
    virtual QTranslatorMessage findMessage(const char* context,
                                           const char *sourceText,
                                           const char* message) const;
    void load ( const QString & filename);
protected:
    QCString fName;
    loaded_l10nfile domain;
};

SIMTranslator::SIMTranslator(QObject *parent, const QString &filename)
        : QTranslator(parent, "simtranslator")
{
    load(filename);
}

SIMTranslator::~SIMTranslator()
{
    if (domain.data)
        k_nl_unload_domain((struct loaded_domain *)domain.data);
}

void SIMTranslator::load(const QString &filename)
{
    fName = QFile::encodeName(filename);
    domain.filename = fName.data();
    k_nl_load_domain(&domain);
}

QTranslatorMessage SIMTranslator::findMessage(const char* context,
        const char *sourceText,
        const char* message) const
{
    QTranslatorMessage res;
    char *text = NULL;
    if ((sourceText == NULL) || (*sourceText == 0))
        return res;
    if (message && *message){
        QCString s;
        s = "_: ";
        s += message;
        s += "\n";
        s += sourceText;
        text = k_nl_find_msg((struct loaded_l10nfile*)&domain, s);
    }
    if ((text == NULL) && context && *context && message && *message){
        QCString s;
        s = "_: ";
        s += context;
        s += "\n";
        s += message;
        text = k_nl_find_msg((struct loaded_l10nfile*)&domain, s);
    }
    if (text == NULL)
        text = k_nl_find_msg((struct loaded_l10nfile*)&domain, sourceText);
    if (text)
        res.setTranslation(QString::fromUtf8(text));
    return res;
}

#endif

/*
typedef struct CoreData
{
    char		*Profile;
    unsigned	SavePasswd;
    unsigned	NoShow;
    unsigned	ShowPanel;
    unsigned	ManualStatus;
    unsigned	Invisible;
    long		geometry[5];
    long		toolBarState[7];
    void		*Buttons;
    void		*Menues;
    unsigned	ShowOnLine;
    unsigned	GroupMode;
    unsigned	UseDblClick;
    unsigned	UseSysColors;
    unsigned	ColorOnline;
    unsigned	ColorOffline;
    unsigned	ColorAway;
    unsigned	ColorNA;
    unsigned	ColorDND;
    unsigned	ColorGroup;
    unsigned	GroupSeparator;
    char		*Lang;
    unsigned	ContainerMode;
    unsigned	SendOnEnter;
    unsigned	containerGeo[5];
    unsigned	containerBar[7];
    unsigned	ContainerStatusSize;
    char		*Containers;
    void		*Container;
    unsigned	CopyMessages;
    unsigned	EditHeight;
    unsigned	editBar[7];
    unsigned	EditBackground;
    unsigned	EditForeground;
    char		*EditFont;
    unsigned	OwnColors;
    unsigned	UseSmiles;
    unsigned	CloseSend;
    unsigned	HistoryPage;
    unsigned	HistoryDirection;
    unsigned	historySize[2];
    long		historyBar[7];
    char		*HistorySearch;
    char		*Unread;
    void		*NoShowAutoReply;
    unsigned	SortMode;
    unsigned	CloseTransfer;
	char		*HistoryStyle;
} CoreData;
*/
static DataDef coreData[] =
    {
        { "Profile", DATA_STRING, 1, 0 },
        { "SavePasswd", DATA_BOOL, 1, DATA(1) },
        { "NoShow", DATA_BOOL, 1, 0 },
        { "ShowPanel", DATA_BOOL, 1, DATA(1) },
        { "ManualStatus", DATA_ULONG, 1, DATA(1) },
        { "", DATA_ULONG, 1, 0 },		// StatusTime
        { "Invisible", DATA_BOOL, 1, 0 },
        { "Geometry", DATA_LONG, 5, DATA(-1) },
        { "ToolBar", DATA_LONG, 7, 0 },
        { "Buttons", DATA_STRLIST, 1, 0 },
        { "Menues", DATA_STRLIST, 1, 0 },
        { "ShowOnLine", DATA_BOOL, 1, 0 },
        { "GroupMode", DATA_ULONG, 1, DATA(1) },
        { "UseDblClick", DATA_BOOL, 1, 0 },
        { "UseSysColors", DATA_BOOL, 1, DATA(1) },
        { "ColorOnline", DATA_ULONG, 1, 0 },
        { "ColorOffline", DATA_ULONG, 1, 0 },
        { "ColorAway", DATA_ULONG, 1, 0 },
        { "ColorNA", DATA_ULONG, 1, 0 },
        { "ColorDND", DATA_ULONG, 1, 0 },
        { "ColorGroup", DATA_ULONG, 1, 0 },
        { "GroupSeparator", DATA_BOOL, 1, DATA(1) },
        { "Lang", DATA_STRING, 1, 0 },
        { "ContainerMode", DATA_ULONG, 1, DATA(2) },
        { "SendOnEnter", DATA_BOOL, 1, 0 },
        { "ShowOwnerName", DATA_BOOL, 1, 0 },
        { "ContainerGeometry", DATA_LONG, 5, DATA(-1) },
        { "ContainerBar", DATA_LONG, 7, 0 },
        { "ContainerStatusSize", DATA_ULONG, 1, 0 },
        { "Containers", DATA_STRING, 1, 0 },
        { "Container", DATA_STRLIST, 1, 0 },
        { "CopyMessages", DATA_ULONG, 1, DATA(3) },
        { "EditHeight", DATA_ULONG, 1, DATA(100) },
        { "EditBar", DATA_LONG, 7, 0 },
        { "EditForeground", DATA_ULONG, 1, DATA(0xffffff) },
        { "EditBackground", DATA_ULONG, 1, DATA(0x000000) },
        { "EditFont", DATA_STRING, 1, 0 },
        { "EditSaveFont", DATA_BOOL, 1, 0 },
        { "OwnColors", DATA_BOOL, 1, 0 },
        { "UseSmiles", DATA_BOOL, 1, DATA(1) },
        { "UseExtViewer", DATA_BOOL, 1, 0 },
#ifdef WIN32
        { "ExtViewer", DATA_STRING, 1, "notepad" },
#else /*WIN32*/
#ifdef USE_KDE
        { "ExtViewer", DATA_STRING, 1, "kate" },
#else/*USE_KDE*/
        { "ExtViewer", DATA_STRING, 1, "gvim" },
#endif/*USE_KDE*/
#endif/*WIN32*/
        { "CloseSend", DATA_BOOL, 1, 0 },
        { "HistoryPage", DATA_ULONG, 1, DATA(100) },
        { "HistoryDirection", DATA_BOOL, 1, 0 },
        { "HistorySize", DATA_ULONG, 2, 0 },
        { "HistoryBar", DATA_LONG, 7, 0 },
        { "HistorySearch", DATA_UTF, 1, 0 },
        { "Unread", DATA_STRING, 1, 0 },
        { "NoShowAutoReply", DATA_STRLIST, 1, 0 },
        { "SortMode", DATA_ULONG, 1, DATA(0x00020301) },
        { "CloseTransfer", DATA_BOOL, 1, 0 },
        { "HistoryStyle", DATA_STRING, 1, "SIM" },
        { "AuthStyle", DATA_ULONG, 1, DATA(1) },
        { "VisibleStyle", DATA_ULONG, 1, DATA(2) },
        { "InvisibleStyle", DATA_ULONG, 1, DATA(4) },
        { "SmallGroupFont", DATA_BOOL, 1, DATA(1) },
        { "ShowAllEncodings", DATA_BOOL, 1, 0 },
        { "ShowEmptyGroup", DATA_BOOL, 1, DATA(1) },
        { "NoJoinAlert", DATA_BOOL, 1, 0 },
        { "EnableSpell", DATA_BOOL, 1, 0 },
        { "RemoveHistory", DATA_BOOL, 1, DATA(1) },
        { "SearchGeometry", DATA_LONG, 5, DATA(0) },
        { "SearchClient", DATA_STRING, 1, DATA(0) },
        { "NoScroller", DATA_BOOL, 1, DATA(0) },
        { "CfgGeometry", DATA_LONG, 5, DATA(0) },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

/*
typedef struct CoreUserData
{
    unsigned	OfflineOpen;
    unsigned	OnlineOpen;
    unsigned	SMSTranslit;
    char		*SMSSignatureBefore;
    char		*SMSSignatureAfter;
	unsigned	OpenOnReceive;
	unsigned	OpenOnOnline;
	char		*IncomingPath;
	unsigned	AcceptMode;
	unsigned	OverwriteFiles;
	char		*DeclineMessage;
} CoreUserData;

*/
static DataDef coreUserData[] =
    {
        { "LogStatus", DATA_BOOL, 1, 0 },
        { "LogMessage", DATA_BOOL, 1, DATA(1) },
        { "OpenNewMessage", DATA_ULONG, 1, DATA(1) },
        { "OpenOnOnline", DATA_BOOL, 1, 0 },
        { "IncomingPath", DATA_UTF, 1, "Incoming Files" },
        { "AcceptMode", DATA_ULONG, 1, 0 },
        { "OverwriteFiles", DATA_BOOL, 1, 0 },
        { "DeclineMessage", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static DataDef smsUserData[] =
    {
        { "SMSSignatureBefore", DATA_UTF, 1, 0 },
        { "SMSSignatureAfter", DATA_UTF, 1, "\n&MyName;" },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static DataDef arUserData[] =
    {
        { "AutoReply", DATA_UTFLIST, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

/*
typedef struct ListUserData
{
    unsigned	OfflineOpen;
    unsigned	OnlineOpen;
	unsigned	ShowAlways;
} ListUserData;
*/

static DataDef listUserData[] =
    {
        { "OfflineOpen", DATA_BOOL, 1, DATA(1) },
        { "OnlineOpen", DATA_BOOL, 1, DATA(1) },
        { "ShowAlways", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

/*
typedef struct TranslitUserData
{
    unsigned	Translit;
} TranslitUserData;
*/

static DataDef translitUserData[] =
    {
        { "Translit", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static DataDef historyUserData[] =
    {
        { "CutSize", DATA_BOOL, 1, 0 },
        { "MaxSize", DATA_ULONG, 1, DATA(2) },
        { "CutDays", DATA_BOOL, 1, 0 },
        { "Days", DATA_ULONG, 1, DATA(90) },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

CorePlugin *CorePlugin::m_plugin = NULL;

static QWidget *getInterfaceSetup(QWidget *parent, void *data)
{
    return new MessageConfig(parent, data);
}

static QWidget *getSMSSetup(QWidget *parent, void *data)
{
    return new SMSConfig(parent, data);
}

static QWidget *getHistorySetup(QWidget *parent, void *data)
{
    return new UserHistoryCfg(parent, data);
}

typedef struct autoReply
{
    unsigned	status;
    const char	*text;
} autoReply;

static autoReply autoReplies[] =
    {
        { STATUS_AWAY, I18N_NOOP(
              "I am currently away from ICQ.\n"
              "Please leave your message and I will get back to you as soon as I return!"
          ) },
        { STATUS_NA, I18N_NOOP(
              "I am out'a here.\n"
              "See you tomorrow!"
          ) },
        { STATUS_DND, I18N_NOOP(
              "Please do not disturb me now. Disturb me later."
          ) },
        { STATUS_FFC, I18N_NOOP(
              "We'd love to hear what you have to say. Join our chat."
          ) },
        { STATUS_ONLINE, I18N_NOOP(
              "I'm here."
          ) },
        { STATUS_OFFLINE, I18N_NOOP(
              "I'm offline."
          ) },
        { 0, NULL }
    };

CorePlugin::CorePlugin(unsigned base, ConfigBuffer *config)
        : Plugin(base), EventReceiver(HighPriority)
{
    m_plugin = this;
    historyXSL = NULL;
    m_bIgnoreEvents = false;
    m_alert = NULL;

    load_data(coreData, &data, config);
    setStatusTime(time(NULL));

    user_data_id	 = getContacts()->registerUserData("core", coreUserData);
    sms_data_id		 = getContacts()->registerUserData("sms", smsUserData);
    ar_data_id		 = getContacts()->registerUserData("ar", arUserData);
    list_data_id	 = getContacts()->registerUserData("list", listUserData);
    translit_data_id = getContacts()->registerUserData("translit", translitUserData);
    history_data_id  = getContacts()->registerUserData("history", historyUserData);

    m_translator = NULL;
    m_statusWnd  = NULL;
    m_status	 = NULL;
    m_main		 = NULL;
    m_cfg		 = NULL;
    m_search	 = NULL;
    m_view		 = NULL;
    m_manager	 = NULL;
    m_focus		 = NULL;
    m_lock		 = NULL;
    m_bInit		 = false;
    m_nClients	 = 0;
    m_nClientsMenu  = 0;
    m_nResourceMenu = 0;
    m_RegNew = false;
    m_HistoryThread = NULL;

    loadDir();

    m_tmpl	= new Tmpl(this);
    m_cmds	= new Commands;
    boundTypes();

    Event eToolbar(EventToolbarCreate, (void*)ToolBarMain);
    eToolbar.process();
    Event eToolbarContainer(EventToolbarCreate, (void*)ToolBarContainer);
    eToolbarContainer.process();
    Event eToolbarTextEdit(EventToolbarCreate, (void*)ToolBarTextEdit);
    eToolbarTextEdit.process();
    Event eToolbarMsgEdit(EventToolbarCreate, (void*)ToolBarMsgEdit);
    eToolbarMsgEdit.process();
    Event eToolbarHistory(EventToolbarCreate, (void*)BarHistory);
    eToolbarHistory.process();
    Event eMenu(EventMenuCreate, (void*)MenuMain);
    eMenu.process();
    Event eMenuPhones(EventMenuCreate, (void*)MenuPhones);
    eMenuPhones.process();
    Event eMenuLocation(EventMenuCreate, (void*)MenuLocation);
    eMenuLocation.process();
    Event eMenuPhoneState(EventMenuCreate, (void*)MenuPhoneState);
    eMenuPhoneState.process();
    Event eMenuFileDecline(EventMenuCreate, (void*)MenuFileDecline);
    eMenuFileDecline.process();
    Event eMenuMailList(EventMenuCreate, (void*)MenuMailList);
    eMenuMailList.process();
    Event eMenuPhoneList(EventMenuCreate, (void*)MenuPhoneList);
    eMenuPhoneList.process();
    Event eMenuStatusWnd(EventMenuCreate, (void*)MenuStatusWnd);
    eMenuStatusWnd.process();
    Event eMenuEncoding(EventMenuCreate, (void*)MenuEncoding);
    eMenuEncoding.process();
    Event eMenuSearch(EventMenuCreate, (void*)MenuSearchItem);
    eMenuSearch.process();
    Event eMenuSearchGroups(EventMenuCreate, (void*)MenuSearchGroups);
    eMenuSearchGroups.process();
    Event eMenuSearchOptions(EventMenuCreate, (void*)MenuSearchOptions);
    eMenuSearchOptions.process();

    Command cmd;
    cmd->id          = CmdConfigure;
    cmd->text        = I18N_NOOP("Setup");
    cmd->icon        = "configure";
    cmd->bar_id      = ToolBarMain;
    cmd->menu_id     = MenuMain;
    cmd->menu_grp    = 0x8080;

    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

    cmd->id			 = CmdUnread;
    cmd->text		 = I18N_NOOP("Unread messages");
    cmd->icon		 = "message";
    cmd->bar_id		 = 0;
    cmd->menu_grp	 = 0x3000;
    cmd->flags		 = COMMAND_IMPORTANT | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id          = CmdQuit;
    cmd->text        = I18N_NOOP("Quit");
    cmd->icon        = "exit";
    cmd->bar_id      = ToolBarMain;
    cmd->menu_id     = MenuMain;
    cmd->menu_grp    = 0x10080;
    cmd->flags		 = COMMAND_IMPORTANT;
    eCmd.process();

    cmd->id			 = CmdMenu;
    cmd->text        = I18N_NOOP("&Menu");
    cmd->icon        = "1downarrow";
    cmd->bar_id      = ToolBarMain;
    cmd->bar_grp     = 0x8000;
    cmd->menu_id     = 0;
    cmd->menu_grp    = 0;
    cmd->popup_id    = MenuMain;
    eCmd.process();

    cmd->id          = CmdSearch;
    cmd->text        = I18N_NOOP("Search / Add contact");
    cmd->icon		 = "find";
    cmd->bar_id		 = ToolBarMain;
    cmd->bar_grp	 = 0;
    cmd->menu_id	 = MenuMain;
    cmd->menu_grp    = 0x2080;
    cmd->popup_id	 = 0;
    cmd->flags		 = COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			 = CmdSendSMS;
    cmd->text		 = I18N_NOOP("&Send SMS");
    cmd->icon		 = "sms";
    cmd->menu_grp	 = 0x2081;
    cmd->flags		 = COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			 = CmdPhones;
    cmd->text		 = I18N_NOOP("&Phone service");
    cmd->bar_id		 = 0;
    cmd->bar_grp	 = 0;
    cmd->icon		 = "phone";
    cmd->menu_grp	 = 0x60F0;
    cmd->popup_id	 = MenuPhones;
    cmd->flags		 = COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			 = CmdMessageType;
    cmd->text		 = I18N_NOOP("Message");
    cmd->icon		 = "message";
    cmd->bar_id		 = ToolBarContainer;
    cmd->bar_grp	 = 0x2000;
    cmd->menu_id	 = 0;
    cmd->menu_grp	 = 0;
    cmd->popup_id	 = MenuMessage;
    cmd->flags		 = BTN_PICT;
    eCmd.process();

    cmd->id			 = CmdContainerContact;
    cmd->text		 = I18N_NOOP("Contact");
    cmd->icon		 = "empty";
    cmd->bar_grp	 = 0x6000;
    cmd->popup_id	 = MenuContainerContact;
    eCmd.process();

    cmd->id			 = CmdContactGroup;
    cmd->text		 = I18N_NOOP("Group");
    cmd->icon		 = "grp_on";
    cmd->bar_grp	 = 0x7000;
    cmd->flags		 = COMMAND_DEFAULT;
    cmd->popup_id	 = MenuContactGroup;
    eCmd.process();

    cmd->id			 = CmdInfo;
    cmd->text		 = I18N_NOOP("User &info");
    cmd->icon		 = "info";
    cmd->popup_id	 = 0;
    cmd->bar_grp	 = 0x8000;
    eCmd.process();

    cmd->id			 = CmdHistory;
    cmd->text		 = I18N_NOOP("&History");
    cmd->icon		 = "history";
    cmd->popup_id	 = 0;
    cmd->bar_grp	 = 0x8010;
    eCmd.process();

    cmd->id			 = CmdClose;
    cmd->text		 = I18N_NOOP("&Close");
    cmd->icon		 = "exit";
    cmd->accel		 = "Esc";
    cmd->bar_grp	 = 0xF000;
    eCmd.process();

    Event eMenuContainerContact(EventMenuCreate, (void*)MenuContainerContact);
    eMenuContainerContact.process();

    cmd->id			= CmdContainerContacts;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->menu_id	= MenuContainerContact;
    cmd->menu_grp	= 0x1000;
    cmd->accel		= QString::null;
    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    Event eMenuMessage(EventMenuCreate, (void*)MenuMessage);
    eMenuMessage.process();

    MsgEdit::setupMessages();

    cmd->id			= CmdContactClients;
    cmd->text		= "_";
    cmd->icon		= "NULL";
    cmd->menu_id	= MenuMessage;
    cmd->menu_grp	= 0x30FF;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdSend;
    cmd->text		= I18N_NOOP("&Send");
    cmd->icon		= "mail_generic";
    cmd->menu_id	= 0;
    cmd->menu_grp	= 0;
    cmd->bar_id		= ToolBarMsgEdit;
    cmd->bar_grp	= 0x8000;
    cmd->flags		= BTN_PICT | COMMAND_CHECK_STATE;
    eCmd.process();

    QStringList smiles;
    getIcons()->getSmiles(smiles);
    unsigned flags = 0;
    QString smile_icon;
    if (smiles.empty()){
        flags = BTN_HIDE;
    }else{
        smile_icon = smiles.front();
    }

    cmd->id         = CmdSmile;
    cmd->text		= I18N_NOOP("&Insert smile");
    cmd->icon		= smile_icon;
    cmd->bar_grp	= 0x7000;
    cmd->flags		= COMMAND_CHECK_STATE | flags;
    eCmd.process();

    cmd->id			= CmdTranslit;
    cmd->text		= I18N_NOOP("Send in &translit");
    cmd->icon		= "translit";
    cmd->icon_on	= "translit";
    cmd->bar_grp	= 0x7010;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdSendClose;
    cmd->text		= I18N_NOOP("C&lose after send");
    cmd->icon		= "fileclose";
    cmd->icon_on	= "fileclose";
    cmd->bar_grp	= 0x7020;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMultiply;
    cmd->text		= I18N_NOOP("&Multiply send");
    cmd->icon		= "1rightarrow";
    cmd->icon_on	= "1leftarrow";
    cmd->bar_grp	= 0xF010;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdBgColor;
    cmd->text		= I18N_NOOP("Back&ground color");
    cmd->icon		= "bgcolor";
    cmd->icon_on	= QString::null;
    cmd->bar_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdFgColor;
    cmd->text		= I18N_NOOP("&Foreground color");
    cmd->icon		= "fgcolor";
    cmd->bar_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdBold;
    cmd->text		= I18N_NOOP("&Bold");
    cmd->icon		= "text_bold";
    cmd->icon_on	= "text_bold";
    cmd->bar_grp	= 0x1002;
    eCmd.process();

    cmd->id			= CmdItalic;
    cmd->text		= I18N_NOOP("&Italic");
    cmd->icon		= "text_italic";
    cmd->icon_on	= "text_italic";
    cmd->bar_grp	= 0x1003;
    eCmd.process();

    cmd->id			= CmdUnderline;
    cmd->text		= I18N_NOOP("&Underline");
    cmd->icon		= "text_under";
    cmd->icon_on	= "text_under";
    cmd->bar_grp	= 0x1004;
    eCmd.process();

    cmd->id			= CmdFont;
    cmd->text		= I18N_NOOP("Select &font");
    cmd->icon		= "text";
    cmd->icon_on	= QString::null;
    cmd->bar_grp	= 0x1005;
    eCmd.process();

    cmd->id			= CmdFileName;
    cmd->text		= I18N_NOOP("Select &file");
    cmd->icon		= "file";
    cmd->icon_on	= QString::null;
    cmd->bar_grp	= 0x1010;
    cmd->flags		= BTN_EDIT | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdPhoneNumber;
    cmd->text		= I18N_NOOP("&Phone number");
    cmd->icon		= "cell";
    cmd->icon_on	= QString::null;
    cmd->bar_grp	= 0x1020;
    cmd->flags		= BTN_COMBO | BTN_NO_BUTTON | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdNextMessage;
    cmd->text		= I18N_NOOP("&Next");
    cmd->icon		= "message";
    cmd->bar_grp	= 0x8000;
    cmd->flags		= BTN_PICT | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdHistoryDirection;
    cmd->text		= I18N_NOOP("&Direction");
    cmd->icon		= "1uparrow";
    cmd->icon_on	= "1downarrow";
    cmd->bar_id		= BarHistory;
    cmd->bar_grp	= 0x2000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdHistoryFind;
    cmd->text		= I18N_NOOP("&Filter");
    cmd->icon		= "filter";
    cmd->icon_on	= "filter";
    cmd->bar_grp	= 0x3000;
    cmd->flags		= BTN_COMBO_CHECK;
    eCmd.process();

    cmd->id			= CmdHistoryPrev;
    cmd->text		= I18N_NOOP("&Previous page");
    cmd->icon		= "1leftarrow";
    cmd->icon_on	= QString::null;
    cmd->bar_grp	= 0x5000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdHistoryNext;
    cmd->text		= I18N_NOOP("&Next page");
    cmd->icon		= "1rightarrow";
    cmd->bar_grp	= 0x5001;
    eCmd.process();

    cmd->id			= CmdHistorySave;
    cmd->text		= I18N_NOOP("&Save as text");
    cmd->icon		= "filesave";
    cmd->accel		= "Ctrl+S";
    cmd->bar_grp	= 0x6000;
    eCmd.process();

    cmd->id			= CmdBgColor;
    cmd->text		= I18N_NOOP("Back&ground color");
    cmd->icon		= "bgcolor";
    cmd->icon_on	= QString::null;
    cmd->bar_id		= ToolBarTextEdit;
    cmd->bar_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdFgColor;
    cmd->text		= I18N_NOOP("&Foreground color");
    cmd->icon		= "fgcolor";
    cmd->bar_grp	= 0x1010;
    eCmd.process();

    cmd->id			= CmdBold;
    cmd->text		= I18N_NOOP("&Bold");
    cmd->icon		= "text_bold";
    cmd->icon_on	= "text_bold";
    cmd->bar_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdItalic;
    cmd->text		= I18N_NOOP("&Italic");
    cmd->icon		= "text_italic";
    cmd->icon_on	= "text_italic";
    cmd->bar_grp	= 0x2010;
    eCmd.process();

    cmd->id			= CmdUnderline;
    cmd->text		= I18N_NOOP("&Underline");
    cmd->icon		= "text_under";
    cmd->icon_on	= "text_under";
    cmd->bar_grp	= 0x2020;
    eCmd.process();

    cmd->id			= CmdFont;
    cmd->text		= I18N_NOOP("Select &font");
    cmd->icon		= "text";
    cmd->icon_on	= "text";
    cmd->bar_grp	= 0x3000;
    eCmd.process();

    Event eMenuGroup(EventMenuCreate, (void*)MenuGroup);
    eMenuGroup.process();

    Event eMenuContact(EventMenuCreate, (void*)MenuContact);
    eMenuContact.process();

    Event eMenuContactGroup(EventMenuCreate, (void*)MenuContactGroup);
    eMenuContactGroup.process();

    Event eMenuMsgView(EventMenuCreate, (void*)MenuMsgView);
    eMenuMsgView.process();

    Event eMenuMsgCommand(EventMenuCreate, (void*)MenuMsgCommand);
    eMenuMsgCommand.process();

    cmd->id			= CmdMsgOpen;
    cmd->text		= I18N_NOOP("&Open message");
    cmd->icon		= "message";
    cmd->icon_on	= QString::null;
    cmd->menu_id	= MenuMsgView;
    cmd->menu_grp	= 0x1000;
    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgSpecial;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdCopy;
    cmd->text		= I18N_NOOP("&Copy");
    cmd->icon		= "editcopy";
    cmd->menu_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdDeleteMessage;
    cmd->text		= I18N_NOOP("&Delete message");
    cmd->icon		= "remove";
    cmd->menu_grp	= 0x3000;
    eCmd.process();

    cmd->id			= CmdCutHistory;
    cmd->text		= I18N_NOOP("&Cut history");
    cmd->menu_grp	= 0x3001;
    eCmd.process();

    cmd->id			= CmdMsgQuote;
    cmd->text		= I18N_NOOP("&Quote");
    cmd->icon		= "empty";
    cmd->menu_id	= MenuMsgCommand;
    cmd->menu_grp	= 0x1002;

    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgQuote + CmdReceived;
    cmd->bar_id		= ToolBarMsgEdit;
    cmd->bar_grp	= 0x1041;
    cmd->flags		= BTN_PICT | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgForward;
    cmd->text		= I18N_NOOP("&Forward");
    cmd->menu_id	= MenuMsgCommand;
    cmd->menu_grp	= 0x1003;
    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgForward + CmdReceived;
    cmd->bar_id		= ToolBarMsgEdit;
    cmd->bar_grp	= 0x1042;
    cmd->flags		= BTN_PICT | COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgAnswer;
    cmd->text		= I18N_NOOP("&Answer");
    cmd->icon		= "mail_generic";
    cmd->bar_grp	= 0x8000;
    eCmd.process();

    Event eMenuTextEdit(EventMenuCreate, (void*)MenuTextEdit);
    eMenuTextEdit.process();

    Event eMenuGroups(EventMenuCreate, (void*)MenuGroups);
    eMenuGroups.process();

    Event eMenuContainer(EventMenuCreate, (void*)MenuContainer);
    eMenuContainer.process();

    cmd->id			= CmdUndo;
    cmd->text		= I18N_NOOP("&Undo");
    cmd->accel		= "Ctrl+Z";
    cmd->icon		= "undo";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x1000;
    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdRedo;
    cmd->text		= I18N_NOOP("&Redo");
    cmd->accel		= "Ctrl+Y";
    cmd->icon		= "redo";
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdCut;
    cmd->text		= I18N_NOOP("Cu&t");
    cmd->icon		= "editcut";
    cmd->accel		= "Ctrl+X";
    cmd->menu_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdCopy;
    cmd->text		= I18N_NOOP("&Copy");
    cmd->icon		= "editcopy";
    cmd->accel		= "Ctrl+C";
    cmd->menu_grp	= 0x2001;
    eCmd.process();

    cmd->id			= CmdPaste;
    cmd->text		= I18N_NOOP("&Paste");
    cmd->icon		= "editpaste";
    cmd->accel		= "Ctrl+V";
    cmd->menu_grp	= 0x2002;
    eCmd.process();

    cmd->id			= CmdClear;
    cmd->text		= I18N_NOOP("Clear");
    cmd->icon		= QString::null;
    cmd->accel		= QString::null;
    cmd->menu_grp	= 0x3000;
    eCmd.process();

    cmd->id			= CmdSelectAll;
    cmd->text		= I18N_NOOP("Select All");
    cmd->accel		= "Ctrl+A";
    cmd->menu_grp	= 0x3001;
    eCmd.process();

#ifdef USE_KDE
#if KDE_IS_VERSION(3,2,0)
    cmd->id		= CmdEnableSpell;
    cmd->text		= I18N_NOOP("Enable spell check");
    cmd->accel		= QString::null;
    cmd->menu_grp	= 0x4000;
    eCmd.process();

    cmd->id		= CmdSpell;
    cmd->text		= I18N_NOOP("Spell check");
    cmd->menu_grp	= 0x4001;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();
#endif
#endif

    cmd->id			= user_data_id + 1;
    cmd->text		= I18N_NOOP("&Messages");
    cmd->accel		= QString::null;
    cmd->icon		= "message";
    cmd->icon_on	= QString::null;
    cmd->param		= (void*)getInterfaceSetup;
    Event ePrefMsg(EventAddPreferences, cmd);
    ePrefMsg.process();

    cmd->id			= sms_data_id + 1;
    cmd->text		= I18N_NOOP("&SMS");
    cmd->icon		= "cell";
    cmd->icon_on	= QString::null;
    cmd->param		= (void*)getSMSSetup;
    Event ePrefSMS(EventAddPreferences, cmd);
    ePrefSMS.process();

    cmd->id			= history_data_id + 1;
    cmd->text		= I18N_NOOP("&History");
    cmd->icon		= "history";
    cmd->icon_on	= QString::null;
    cmd->param		= (void*)getHistorySetup;
    Event ePrefHistory(EventAddPreferences, cmd);
    ePrefHistory.process();

    cmd->id          = CmdOnline;
    cmd->text        = I18N_NOOP("Show &offline");
    cmd->icon        = "online_off";
    cmd->icon_on     = "online_on";
    cmd->bar_id      = ToolBarMain;
    cmd->bar_grp     = 0x4000;
    cmd->menu_id	 = MenuMain;
    cmd->menu_grp    = 0;
    if (getShowOnLine())
        cmd->flags |= COMMAND_CHECKED;
    eCmd.process();

    cmd->id			= CmdGrpOff;
    cmd->text		= I18N_NOOP("Do&n't show groups");
    cmd->icon		= "grp_off";
    cmd->icon_on	= QString::null;
    cmd->bar_id		= 0;
    cmd->menu_id	= MenuGroups;
    cmd->menu_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdGrpMode1;
    cmd->text		= I18N_NOOP("Group mode 1");
    cmd->icon		= "grp_on";
    eCmd.process();

    cmd->id			= CmdGrpMode2;
    cmd->text		= I18N_NOOP("Group mode 2");
    eCmd.process();

    cmd->id			= CmdOnline;
    cmd->text		= I18N_NOOP("Show &offline");
    cmd->icon		= "online_off";
    cmd->icon_on	= "online_on";
    cmd->menu_grp	= 0x8000;
    eCmd.process();

    cmd->id			= CmdEmptyGroup;
    cmd->text		= I18N_NOOP("Show &empty groups");
    cmd->icon		= QString::null;
    cmd->icon_on	= QString::null;
    cmd->menu_grp	= 0x8001;
    eCmd.process();

    cmd->id			= CmdGrpCreate;
    cmd->text		= I18N_NOOP("&Create group");
    cmd->icon		= "grp_create";
    cmd->icon_on	= QString::null;
    cmd->menu_grp	= 0xA000;
    eCmd.process();

    cmd->menu_id	= MenuGroup;
    cmd->menu_grp	= 0x4000;
    eCmd.process();

    cmd->id			= CmdGrpRename;
    cmd->text		= I18N_NOOP("&Rename group");
    cmd->icon		= "grp_rename";
    cmd->accel		= "F2";
    cmd->menu_grp	= 0x4001;
    eCmd.process();

    cmd->id			= CmdGrpDelete;
    cmd->text		= I18N_NOOP("&Delete group");
    cmd->icon		= "remove";
    cmd->accel		= "Del";
    cmd->menu_grp	= 0x4002;
    eCmd.process();

    cmd->id			= CmdGrpUp;
    cmd->text		= I18N_NOOP("Up");
    cmd->icon		= "1uparrow";
    cmd->accel		= "Ctrl+Up";
    cmd->menu_grp	= 0x6000;
    eCmd.process();

    cmd->id			= CmdGrpDown;
    cmd->text		= I18N_NOOP("Down");
    cmd->icon		= "1downarrow";
    cmd->accel		= "Ctrl+Down";
    cmd->menu_grp	= 0x6001;
    eCmd.process();

    cmd->id			= CmdGrpTitle;
    cmd->text		= "_";
    cmd->icon		= "grp_on";
    cmd->accel		= QString::null;
    cmd->menu_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE | COMMAND_TITLE;
    eCmd.process();

    cmd->id			= CmdConfigure;
    cmd->text		= I18N_NOOP("Setup");
    cmd->icon		= "configure";
    cmd->menu_grp	= 0xB000;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdContactTitle;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->accel		= QString::null;
    cmd->menu_id	= MenuContact;
    cmd->menu_grp	= 0x1000;
    cmd->popup_id	= 0;
    cmd->flags		= COMMAND_CHECK_STATE | COMMAND_TITLE;
    eCmd.process();

    cmd->id			= CmdUnread;
    cmd->menu_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdSendMessage;
    cmd->menu_grp	= 0x2000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdClose;
    cmd->text		= "&Close";
    cmd->icon		= "exit";
    cmd->menu_id	= MenuContact;
    cmd->menu_grp	= 0xF000;
    eCmd.process();

    cmd->id			= CmdContactGroup;
    cmd->text		= I18N_NOOP("Group");
    cmd->icon		= "grp_on";
    cmd->menu_grp	= 0x8000;
    cmd->popup_id	= MenuContactGroup;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdContactRename;
    cmd->text		= I18N_NOOP("&Rename");
    cmd->icon		= QString::null;
    cmd->menu_grp	= 0x8001;
    cmd->popup_id	= 0;
    cmd->accel		= "F2";
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdContactDelete;
    cmd->text		= I18N_NOOP("&Delete");
    cmd->icon		= "remove";
    cmd->menu_grp	= 0x8002;
    cmd->accel		= "Del";
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdShowAlways;
    cmd->text		= I18N_NOOP("Show &always");
    cmd->icon		= QString::null;
    cmd->menu_grp	= 0x8003;
    cmd->accel		= QString::null;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id         = CmdFetchAway;
    cmd->text       = I18N_NOOP("&Fetch away message");
    cmd->icon       = "message";
    cmd->menu_grp   = 0x8020;
    cmd->flags      = COMMAND_CHECK_STATE | BTN_HIDE;
    eCmd.process();

    cmd->id			= CmdInfo;
    cmd->text		= I18N_NOOP("User &info");
    cmd->icon		= "info";
    cmd->menu_grp	= 0x7010;
    cmd->accel		= QString::null;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdHistory;
    cmd->text		= I18N_NOOP("&History");
    cmd->icon		= "history";
    cmd->menu_grp	= 0x7020;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdConfigure;
    cmd->text		= I18N_NOOP("Setup");
    cmd->icon		= "configure";
    cmd->menu_grp	= 0x7020;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdContainer;
    cmd->text		= I18N_NOOP("To container");
    cmd->icon		= QString::null;
    cmd->popup_id	= MenuContainer;
    cmd->menu_grp	= 0x8010;
    cmd->accel		= QString::null;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdContainer;
    cmd->text		= "_";
    cmd->popup_id	= 0;
    cmd->menu_id	= MenuContainer;
    cmd->menu_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdContactGroup;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->accel		= QString::null;
    cmd->menu_id	= MenuContactGroup;
    cmd->menu_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdLocation;
    cmd->text		= I18N_NOOP("&Location");
    cmd->menu_id	= MenuPhones;
    cmd->menu_grp	= 0x1000;
    cmd->popup_id   = MenuLocation;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdPhoneState;
    cmd->text		= I18N_NOOP("&Status");
    cmd->menu_id	= MenuPhones;
    cmd->menu_grp   = 0x1010;
    cmd->popup_id   = MenuPhoneState;
    eCmd.process();

    cmd->id			= CmdPhoneBook;
    cmd->text		= I18N_NOOP("&Phone book");
    cmd->menu_id	= MenuPhones;
    cmd->menu_grp   = 0x1020;
    cmd->popup_id   = 0;
    eCmd.process();

    cmd->id			= CmdPhoneNoShow;
    cmd->text		= I18N_NOOP("&No show");
    cmd->menu_id	= MenuPhoneState;
    cmd->menu_grp   = 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdPhoneAvailable;
    cmd->text		= I18N_NOOP("&Available");
    cmd->icon		= "phone";
    cmd->menu_id	= MenuPhoneState;
    cmd->menu_grp   = 0x1001;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdPhoneBusy;
    cmd->text		= I18N_NOOP("&Busy");
    cmd->icon		= "nophone";
    cmd->menu_id	= MenuPhoneState;
    cmd->menu_grp   = 0x1002;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdLocation;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->menu_id	= MenuLocation;
    cmd->menu_grp   = 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdDeclineWithoutReason;
    cmd->text		= I18N_NOOP("Decline file without reason");
    cmd->icon		= QString::null;
    cmd->menu_id	= MenuFileDecline;
    cmd->menu_grp   = 0x1000;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdDeclineReasonBusy;
    cmd->text		= I18N_NOOP("Sorry, I'm busy right now, and can not respond to your request");
    cmd->menu_grp   = 0x1001;
    eCmd.process();

    cmd->id			= CmdDeclineReasonLater;
    cmd->text		= I18N_NOOP("Sorry, I'm busy right now, but I'll be able to respond to you later");
    cmd->menu_grp   = 0x1002;
    eCmd.process();

    cmd->id			= CmdDeclineReasonInput;
    cmd->text		= I18N_NOOP("Enter a decline reason");
    cmd->menu_grp   = 0x1004;
    eCmd.process();

    cmd->id			= CmdEditList;
    cmd->text		= I18N_NOOP("&Edit");
    cmd->icon		= "mail_generic";
    cmd->menu_id	= MenuMailList;
    cmd->menu_grp	= 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdRemoveList;
    cmd->text		= I18N_NOOP("&Delete");
    cmd->icon		= "remove";
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdEditList;
    cmd->text		= I18N_NOOP("&Edit");
    cmd->icon		= "phone";
    cmd->menu_id	= MenuPhoneList;
    cmd->menu_grp	= 0x1000;
    eCmd.process();

    cmd->id			= CmdRemoveList;
    cmd->text		= I18N_NOOP("&Delete");
    cmd->icon		= "remove";
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdStatusWnd;
    cmd->text		= "_";
    cmd->icon		= QString::null;
    cmd->menu_id	= MenuStatusWnd;
    eCmd.process();

    cmd->id			 = CmdChangeEncoding;
    cmd->text		 = "_";
    cmd->menu_id	 = MenuEncoding;
    cmd->menu_grp	 = 0x1000;
    eCmd.process();

    cmd->id			 = CmdAllEncodings;
    cmd->text		 = I18N_NOOP("&Show all encodings");
    cmd->menu_id	 = MenuEncoding;
    cmd->menu_grp	 = 0x8000;
    eCmd.process();

    cmd->id			 = CmdChangeEncoding;
    cmd->text		 = I18N_NOOP("Change &encoding");
    cmd->icon		 = "encoding";
    cmd->menu_id	 = 0;
    cmd->menu_grp	 = 0;
    cmd->bar_id		 = ToolBarContainer;
    cmd->bar_grp	 = 0x8080;
    cmd->popup_id	 = MenuEncoding;
    eCmd.process();

    cmd->id			 = CmdChangeEncoding;
    cmd->text		 = I18N_NOOP("Change &encoding");
    cmd->icon		 = "encoding";
    cmd->menu_id	 = 0;
    cmd->menu_grp	 = 0;
    cmd->bar_id		 = BarHistory;
    cmd->bar_grp	 = 0x8080;
    cmd->popup_id	 = MenuEncoding;
    eCmd.process();

    cmd->id			 = CmdContactGroup;
    cmd->text		 = I18N_NOOP("Add to &group");
    cmd->icon		 = QString::null;
    cmd->menu_id	 = MenuSearchItem;
    cmd->menu_grp	 = 0x2000;
    cmd->bar_id		 = 0;
    cmd->bar_grp	 = 0;
    cmd->popup_id	 = MenuSearchGroups;
    cmd->flags		 = COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			 = CmdSearchOptions;
    cmd->text		 = "_";
    cmd->icon		 = QString::null;
    cmd->menu_id	 = MenuSearchItem;
    cmd->menu_grp	 = 0x3000;
    cmd->popup_id	 = 0;
    cmd->flags		 = COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			 = CmdSearchInfo;
    cmd->text		 = I18N_NOOP("&Info");
    cmd->icon		 = "info";
    cmd->menu_id	 = MenuSearchOptions;
    cmd->menu_grp	 = 0x3000;
    cmd->popup_id	 = 0;
    cmd->flags		 = COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			 = CmdSearchMsg;
    cmd->text		 = I18N_NOOP("Send &message");
    cmd->icon		 = "message";
    cmd->menu_id	 = MenuSearchOptions;
    cmd->menu_grp	 = 0x3001;
    eCmd.process();

    cmd->id			 = CmdContactGroup;
    cmd->text		 = "_";
    cmd->flags		 = COMMAND_CHECK_STATE;
    cmd->menu_id	 = MenuSearchGroups;
    cmd->menu_grp	 = 0x1000;
    eCmd.process();
}

void CorePlugin::initData()
{
    delete historyXSL;
    historyXSL = new XSL(getHistoryStyle());
    if ((getEditBackground() == 0) && (getEditForeground() == 0)){
        QPalette pal = QApplication::palette();
        QColorGroup cg = pal.normal();
        setEditBackground(cg.color(QColorGroup::Base).rgb() & 0xFFFFFF);
        setEditForeground(cg.color(QColorGroup::Text).rgb() & 0xFFFFFF);
    }
    editFont = FontEdit::str2font(getEditFont(), QApplication::font());
    setAutoReplies();
}

void CorePlugin::setAutoReplies()
{
    ARUserData *data = (ARUserData*)getContacts()->getUserData(ar_data_id);
    for (autoReply *a = autoReplies; a->text; a++){
        const QString &t = get_str(data->AutoReply, a->status);
        if (t.isEmpty())
            set_str(&data->AutoReply, a->status, i18n(a->text));
    }
}

CorePlugin::~CorePlugin()
{
    destroy();
    delete m_lock;
    delete m_cmds;
    delete m_tmpl;
    delete m_status;
    delete historyXSL;
    delete m_HistoryThread;

    getContacts()->unregisterUserData(history_data_id);
    getContacts()->unregisterUserData(translit_data_id);
    getContacts()->unregisterUserData(list_data_id);
    getContacts()->unregisterUserData(ar_data_id);
    getContacts()->unregisterUserData(sms_data_id);
    getContacts()->unregisterUserData(user_data_id);

    free_data(coreData, &data);
    removeTranslator();
}

QString CorePlugin::poFile(const char *lang)
{
#ifdef WIN32
    QString s = "po\\" + QString(lang).lower() + ".qm";
    QFile f(app_file(s));
    if (!f.exists())
        return "";
#else
    string s = PREFIX "/share/locale/";
    string l;
    if (lang)
        l = lang;
    char *p = (char*)(l.c_str());
    char *r = strchr(p, '.');
    if (r) *r = 0;
    s += l.c_str();
    s += "/LC_MESSAGES/sim.mo";
    QFile f(QFile::decodeName(s.c_str()));
    if (!f.exists()){
        r = strchr(p, '_');
        if (r) *r = 0;
        s = PREFIX "/share/locale/";
        s += l.c_str();
        s += "/LC_MESSAGES/sim.mo";
        f.setName(QFile::decodeName(s.c_str()));
        if (!f.exists()) return "";
    }
#endif
    return f.name();
}

void CorePlugin::installTranslator()
{
    m_translator = NULL;
    QString lang = getLang();
    if (lang == "-")
        return;
    if (lang.length() == 0){
#ifdef WIN32
        char buff[256];
        int res = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, buff, sizeof(buff));
        if (res){
            lang += (char)tolower(buff[0]);
            lang += (char)tolower(buff[1]);
        }
#else
#ifdef USE_KDE
        return;
#else
        char *p = getenv("LANG");
        if (p){
            for (; *p; p++){
                if (*p == '.') break;
                lang += *p;
            }
        }
#endif
#endif
    }
    QString po = poFile(lang.ascii());
    if (po.isEmpty())
        return;
#if !defined(WIN32) && !defined(USE_KDE)
    m_translator = new SIMTranslator(NULL, po);
#else
    m_translator = new QTranslator(NULL);
    m_translator->load(po);
#endif
    qApp->installTranslator(m_translator);
#if !defined(WIN32) && !defined(USE_KDE)
    resetPlural();
#endif
    Event e(EventLanguageChanged, m_translator);
    e.process();
}

void CorePlugin::removeTranslator()
{
    if (m_translator){
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
#if !defined(WIN32) && !defined(USE_KDE)
        resetPlural();
#endif
        Event e(EventLanguageChanged, NULL);
        e.process();
    }
}

typedef struct msgIndex
{
    unsigned	contact;
    unsigned	type;
} msgIndex;

typedef struct msgCount
{
    unsigned	count;
    unsigned	index;
} msgCount;

bool operator < (const struct msgIndex &a, const struct msgIndex &b)
{
    if (a.contact < b.contact)
        return true;
    if (a.contact > b.contact)
        return false;
    return a.type < b.type;
}

typedef map<msgIndex, msgCount> MAP_COUNT;

void CorePlugin::getWays(vector<clientContact> &ways, Contact *contact)
{
    clientData *data;
    ClientDataIterator it(contact->clientData);
    while ((data = ++it) != NULL){
        clientData *data1;
        ClientDataIterator it1(contact->clientData);
        bool bOK = true;
        while ((data1 = ++it1) != NULL){
            if (data1 == data)
                break;
            if (data->Sign.toULong() != data1->Sign.toULong())
                continue;
            if (it.client()->compareData(data, data1)){
                bOK = false;
                break;
            }
        }
        if (!bOK)
            continue;
        clientContact c;
        c.client = it.client();
        c.data   = data;
        c.bNew   = false;
        ways.push_back(c);
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            Client *client = getContacts()->getClient(i);
            if (client == it.client())
                continue;
            Contact *clContact;
            clientData *data1 = data;
            if (client->isMyData(data1, clContact)){
                if ((clContact == contact) || (clContact == NULL)){
                    clientContact c;
                    c.client = client;
                    c.data   = data1;
                    c.bNew   = (clContact == NULL);
                    ways.push_back(c);
                }
            }
        }
    }
}

static const char *helpList[] =
    {
        "&IP;",
        I18N_NOOP("ip-address"),
        "&Mail;",
        I18N_NOOP("e-mail"),
        "&Phone;",
        I18N_NOOP("phone"),
        "&Nick;",
        I18N_NOOP("contact nick"),
        "&Unread;",
        I18N_NOOP("number of unread messages from this contact"),
        "&Status;",
        I18N_NOOP("contact status"),
        "&TimeStatus;",
        I18N_NOOP("time of set status"),
        "&IntervalStatus;",
        I18N_NOOP("time from set status"),
        NULL,
    };

#if 0
I18N_NOOP("male", "%1 wrote:" )
I18N_NOOP("female", "%1 wrote:" )
#endif

void *CorePlugin::processEvent(Event *e)
{
    switch (e->type()){
    case EventIconChanged:{
            QStringList smiles;
            getIcons()->getSmiles(smiles);
            unsigned flags = 0;
            QString smile_icon;
            if (smiles.empty()){
                flags = BTN_HIDE;
            }else{
                smile_icon = smiles.front();
            }
            Command cmd;
            cmd->id			= CmdSmile;
            cmd->text		= I18N_NOOP("&Insert smile");
            cmd->icon		= smile_icon;
            cmd->bar_id		= ToolBarMsgEdit;
            cmd->bar_grp	= 0x7000;
            cmd->flags		= COMMAND_CHECK_STATE | flags;
            Event eCmd(EventCommandChange, cmd);
            eCmd.process();
            return NULL;
        }
    case EventJoinAlert:
        if (!getNoJoinAlert() && (m_alert == NULL)){
            Command cmd;
            cmd->id = CmdStatusBar;
            Event eWidget(EventCommandWidget, cmd);
            QWidget *widget = (QWidget*)(eWidget.process());
            if (widget == NULL)
                return e->param();
            raiseWindow(widget->topLevelWidget());
            QStringList l;
            l.append(i18n("OK"));
            m_alert = new BalloonMsg(NULL,
                                     quoteString(
                                         i18n("At loading contact list contacts with identical names were automatically joined.\n"
                                              "If it is wrong, you can separate them. "
                                              "For this purpose in contact menu choose the necessary name and choose a command \"Separate\".")),
                                     l, widget, NULL, false, true, 150, i18n("Don't show this message in next time"));
            connect(m_alert, SIGNAL(finished()), this, SLOT(alertFinished()));
        }
        return e->param();
    case EventGroupChanged:
        if (m_bIgnoreEvents)
            return e->param();
        break;
    case EventDeleteMessage:{
            Message *msg = (Message*)(e->param());
            History::del(msg);
            return e->param();
        }
    case EventRewriteMessage:{
            Message *msg = (Message*)(e->param());
            History::rewrite(msg);
            return NULL;
        }
    case EventTmplHelp:{
            QString *str = (QString*)(e->param());
            for (const char **p = helpList; *p;){
                *str += *(p++);
                *str += " - ";
                *str += i18n(*(p++));
                *str += "\n";
            }
            *str += "\n";
            *str += i18n("`<command>` - call <command> and substitute command output\n");
            return e->param();
        }
    case EventTmplHelpList:
        return helpList;
    case EventARRequest:{
            ARRequest *r = (ARRequest*)(e->param());
            ARUserData *ar;
            QString tmpl;
            if (r->contact){
                ar = (ARUserData*)(r->contact->userData.getUserData(ar_data_id, false));
                if (ar)
                    tmpl = get_str(ar->AutoReply, r->status);
                if (tmpl.isEmpty()){
                    ar = NULL;
                    Group *grp = getContacts()->group(r->contact->getGroup());
                    if (grp)
                        ar = (ARUserData*)(grp->userData.getUserData(ar_data_id, false));
                    if (ar)
                        tmpl = get_str(ar->AutoReply, r->status);
                }
            }
            if (tmpl.isEmpty()){
                ar = (ARUserData*)(getContacts()->getUserData(ar_data_id));
                tmpl = get_str(ar->AutoReply, r->status);
                if (tmpl.isEmpty())
                    tmpl = get_str(ar->AutoReply, STATUS_AWAY);
            }
            TemplateExpand t;
            t.contact	= r->contact;
            t.param		= r->param;
            t.receiver	= r->receiver;
            t.tmpl		= tmpl;
            Event eTmpl(EventTemplateExpand, &t);
            eTmpl.process();
            return e->param();
        }
    case EventSaveState:{
            ARUserData *ar = (ARUserData*)getContacts()->getUserData(ar_data_id);
            for (autoReply *a = autoReplies; a->text; a++){
                QString t = get_str(ar->AutoReply, a->status);
                if (t == i18n(a->text))
                    set_str(&ar->AutoReply, a->status, QString::null);
            }
            e->process(this);
            setAutoReplies();
            return this;
        }
    case EventPluginChanged:{
            pluginInfo *info = (pluginInfo*)(e->param());
            if (info->plugin == this){
                QString profile = getProfile();
                free_data(coreData, &data);
                load_data(coreData, &data, info->cfg);
                setStatusTime(time(NULL));
                if (info->cfg){
                    delete info->cfg;
                    info->cfg = NULL;
                }
                setProfile(profile);
                removeTranslator();
                installTranslator();
                initData();
            }
            break;
        }
    case EventInit:
        if (!m_bInit && !init(true))
            return (void*)ABORT_LOADING;
        QTimer::singleShot(0, this, SLOT(checkHistory()));
        return NULL;
    case EventQuit:
        destroy();
        m_cmds->clear();
        return NULL;
    case EventHomeDir:{
            QString *cfg = (QString*)(e->param());
            QString fname = *cfg;
            QString profile;
#ifdef WIN32
            if ((fname[1] != ':') && (fname.left(2) != "\\\\"))
#else
            if (fname[0] != '/')
#endif
                profile = getProfile();
            if (profile.length())
                profile += "/";
            profile += fname;
            if (profile.isEmpty()){
                *cfg = "";
            }else{
                *cfg = profile;
            }
            Event eProfile(EventHomeDir, cfg);
            if (!eProfile.process(this))
                *cfg = app_file(*cfg);
            makedir(QFile::encodeName(*cfg).data());
            return cfg;
        }
    case EventAddPreferences:{
            CommandDef *cmd = (CommandDef*)(e->param());
            cmd->menu_id = MenuGroup;
            Event eCmd(EventCommandCreate, cmd);
            eCmd.process();
            cmd->menu_id = MenuContact;
            eCmd.process();
            preferences.add(cmd);
            return e->param();
        }
    case EventRemovePreferences:{
            unsigned long id = (unsigned long)(e->param());
            Event eCmd(EventCommandRemove, (void*)id);
            eCmd.process();
            preferences.erase(id);
            return e->param();
        }
    case EventClientsChanged:
        if (m_bInit)
            loadMenu();
    case EventClientChanged:		// FALLTHROW
        if (getContacts()->nClients()){
            unsigned i;
            for (i = 0; i < getContacts()->nClients(); i++)
                if (getContacts()->getClient(i)->getCommonStatus())
                    break;
            if (i >= getContacts()->nClients()){
                Client *client = getContacts()->getClient(0);
                setManualStatus(client->getManualStatus());
                client->setCommonStatus(true);
                Event e(EventClientChanged, client);
                e.process();
            }
        }
        return NULL;
    case EventCreateMessageType:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if (cmd->menu_grp){
                cmd->menu_id = MenuMessage;
                cmd->flags   = COMMAND_CHECK_STATE;
                Event eCmd(EventCommandCreate, cmd);
                eCmd.process();
            }
            if (cmd->param){
                MessageDef *mdef = (MessageDef*)(cmd->param);
                if (mdef->cmdReceived){
                    for (const CommandDef *c = mdef->cmdReceived; !c->text.isEmpty(); c++){
                        CommandDef cmd = *c;
                        if (cmd.icon == NULL){
                            cmd.icon   = "empty";
                            cmd.flags |= BTN_PICT;
                        }
                        cmd.id += CmdReceived;
                        cmd.menu_id  = 0;
                        cmd.menu_grp = 0;
                        cmd.flags	|= COMMAND_CHECK_STATE;
                        Event eCmd(EventCommandCreate, &cmd);
                        eCmd.process();
                    }
                }
                if (mdef->cmdSent){
                    for (const CommandDef *c = mdef->cmdSent; !c->text.isEmpty(); c++){
                        CommandDef cmd = *c;
                        if (cmd.icon == NULL){
                            cmd.icon = "empty";
                            cmd.flags |= BTN_PICT;
                        }
                        cmd.id += CmdReceived;
                        cmd.menu_id  = 0;
                        cmd.menu_grp = 0;
                        cmd.flags	|= COMMAND_CHECK_STATE;
                        Event eCmd(EventCommandCreate, &cmd);
                        eCmd.process();
                    }
                }
            }
            messageTypes.add(cmd);
            QString name = typeName(cmd->text);
            MAP_TYPES::iterator itt = types.find(name);
            if (itt == types.end()){
                types.insert(MAP_TYPES::value_type(name, cmd->id));
            }else{
                (*itt).second = cmd->id;
            }
            return e->param();
        }
    case EventRemoveMessageType:{
            unsigned long id = (unsigned long)(e->param());
            CommandDef *def;
            def = CorePlugin::m_plugin->messageTypes.find(id);
            if (def){
                MessageDef *mdef = (MessageDef*)(def->param);
                if (mdef->cmdReceived){
                    for (const CommandDef *c = mdef->cmdReceived; !c->text.isEmpty(); c++){
                        Event eCmd(EventCommandRemove, (void*)(c->id + CmdReceived));
                        eCmd.process();
                    }
                }
                if (mdef->cmdSent){
                    for (const CommandDef *c = mdef->cmdSent; !c->text.isEmpty(); c++){
                        Event eCmd(EventCommandRemove, (void*)(c->id + CmdReceived));
                        eCmd.process();
                    }
                }
            }
            for (MAP_TYPES::iterator itt = types.begin(); itt != types.end(); ++itt){
                if ((*itt).second == id){
                    types.erase(itt);
                    break;
                }
            }
            Event eCmd(EventCommandRemove, (void*)id);
            eCmd.process();
            messageTypes.erase(id);
            return e->param();
        }
    case EventContactDeleted:{
            Contact *contact = (Contact*)(e->param());
            clearUnread(contact->id());
            History::remove(contact);
            return NULL;
        }
    case EventContactChanged:{
            if (m_bIgnoreEvents)
                return e->param();
            Contact *contact = (Contact*)(e->param());
            if (contact->getIgnore())
                clearUnread(contact->id());
            return NULL;
        }
    case EventMessageAcked:{
            Message *msg = (Message*)(e->param());
            if (msg->baseType() == MessageFile){
                QWidget *w = new FileTransferDlg(static_cast<FileMessage*>(msg));
                raiseWindow(w);
            }
            return NULL;
        }
    case EventMessageDeleted:{
            Message *msg = (Message*)(e->param());
            History::del(msg->id());
            for (list<msg_id>::iterator it = unread.begin(); it != unread.end(); ++it){
                msg_id &m = *it;
                if (m.id == msg->id()){
                    unread.erase(it);
                    break;
                }
            }
            return NULL;
        }
    case EventMessageReceived:{
            Message *msg = (Message*)(e->param());
            Contact *contact = getContacts()->contact(msg->contact());
            if (contact){
                if (msg->getTime() == 0){
                    msg->setTime(time(NULL));
                }
                unsigned type = msg->baseType();
                if (type == MessageStatus){
                    CoreUserData *data = (CoreUserData*)(contact->getUserData(CorePlugin::m_plugin->user_data_id));
                    if ((data == NULL) || !data->LogStatus.toBool())
                        return NULL;
                }else if (type == MessageFile){
                    CoreUserData *data = (CoreUserData*)(contact->getUserData(CorePlugin::m_plugin->user_data_id));
                    if (data){
                        if (data->AcceptMode.toULong() == 1){
                            QString dir = data->IncomingPath.str();
                            if (!dir.isEmpty() && !dir.endsWith("/") && !dir.endsWith("\\"))
                                dir += '/';
                            dir = user_file(dir);
                            messageAccept ma;
                            ma.msg       = msg;
                            ma.dir       = dir;
                            ma.overwrite = data->OverwriteFiles.toBool() ? Replace : Skip;
                            Event e(EventMessageAccept, &ma);
                            e.process();
                            return msg;
                        }
                        if (data->AcceptMode.toULong() == 2){
                            messageDecline md;
                            md.msg    = msg;
                            md.reason = data->DeclineMessage.str();
                            Event e(EventMessageDecline, &md);
                            e.process();
                            return msg;
                        }
                    }
                }else{
                    contact->setLastActive(time(NULL));
                    Event e(EventContactStatus, contact);
                    e.process();
                }
            }
        }
    case EventSent:{
            Message *msg = (Message*)(e->param());
            CommandDef *def = messageTypes.find(msg->type());
            if (def){
                History::add(msg, typeName(def->text));
                if ((e->type() == EventMessageReceived) && (msg->type() != MessageStatus)){
                    msg_id m;
                    m.id = msg->id();
                    m.contact = msg->contact();
                    m.client = msg->client();
                    m.type = msg->baseType();
                    unread.push_back(m);
                    if (msg->getFlags() & MESSAGE_NOVIEW)
                        return NULL;
                    Contact *contact = getContacts()->contact(msg->contact());
                    if (contact && (contact->getFlags() & CONTACT_TEMPORARY)){
                        contact->setFlags(contact->getFlags() & ~CONTACT_TEMPORARY);
                        Event e(EventContactChanged, contact);
                        e.process();
                    }
                    if (contact){
                        CoreUserData *data = (CoreUserData*)(contact->getUserData(user_data_id));
                        if (data->OpenNewMessage.toULong()){
                            if (data->OpenNewMessage.toULong() == NEW_MSG_MINIMIZE)
                                msg->setFlags(msg->getFlags() | MESSAGE_NORAISE);
                            Event e(EventOpenMessage, &msg);
                            e.process();
                        }
                    }
                }
            } else {
                log(L_WARN,"No CommandDef for message %u found!",msg->type());
            }
            return NULL;
        }
    case EventDefaultAction:{
            unsigned long contact_id = (unsigned long)(e->param());
            unsigned index = 0;
            for (list<msg_id>::iterator it = CorePlugin::m_plugin->unread.begin(); it != CorePlugin::m_plugin->unread.end(); ++it, index++){
                if ((*it).contact != contact_id)
                    continue;
                Command cmd;
                cmd->id = CmdUnread + index;
                cmd->menu_id = MenuMain;
                Event e(EventCommandExec, cmd);
                return e.process();
            }
            Event eMenu(EventGetMenuDef, (void*)MenuMessage);
            CommandsDef *cmdsMsg = (CommandsDef*)(eMenu.process());
            CommandsList itc(*cmdsMsg, true);
            CommandDef *c;
            while ((c = ++itc) != NULL){
                c->param = (void*)(contact_id);
                Event eCheck(EventCheckState, c);
                if (eCheck.process()){
                    Event eCmd(EventCommandExec, c);
                    return eCmd.process();
                }
            }
            return NULL;
        }
    case EventLoadMessage:{
            MessageID *m = (MessageID*)(e->param());
            Message *msg = History::load(m->id, m->client, m->contact);
            return msg;
        }
    case EventOpenMessage:{
            Message **msg = (Message**)(e->param());
            if ((*msg)->getFlags() & MESSAGE_NOVIEW)
                return NULL;
            Contact *contact = getContacts()->contact((*msg)->contact());
            m_focus = qApp->focusWidget();
            if (m_focus)
                connect(m_focus, SIGNAL(destroyed()), this, SLOT(focusDestroyed()));
            if (contact == NULL)
                return NULL;
            UserWnd		*userWnd	= NULL;
            Container	*container	= NULL;
            QWidgetList  *list = QApplication::topLevelWidgets();
            QWidgetListIt itw(*list);
            QWidget * w;
            bool bNew = false;
            while ((w = itw.current()) != NULL){
                if (w->inherits("Container")){
                    container =  static_cast<Container*>(w);
                    if (getContainerMode() == 0){
                        if (container->isReceived() != (((*msg)->getFlags() & MESSAGE_RECEIVED) != 0)){
                            container = NULL;
                            ++itw;
                            continue;
                        }
                    }
                    userWnd = container->wnd(contact->id());
                    if (userWnd)
                        break;
                    container = NULL;
                }
                ++itw;
            }
            delete list;
            if (userWnd == NULL){
                if (contact->getFlags() & CONTACT_TEMP){
                    contact->setFlags(contact->getFlags() & ~CONTACT_TEMP);
                    Event e(EventContactChanged, contact);
                    e.process();
                }
                userWnd = new UserWnd(contact->id(), NULL, (*msg)->getFlags() & MESSAGE_RECEIVED, (*msg)->getFlags() & MESSAGE_RECEIVED);
                if (getContainerMode() == 3){
                    QWidgetList  *list = QApplication::topLevelWidgets();
                    QWidgetListIt it(*list);
                    QWidget * w;
                    while ((w = it.current()) != NULL){
                        if (w->inherits("Container")){
                            container =  static_cast<Container*>(w);
                            break;
                        }
                        ++it;
                    }
                    delete list;
                    if (container == NULL){
                        container = new Container(1);
                        bNew = true;
                    }
                }else if (getContainerMode() == 2){
                    unsigned id = contact->getGroup() + CONTAINER_GRP;
                    QWidgetList  *list = QApplication::topLevelWidgets();
                    QWidgetListIt it(*list);
                    QWidget * w;
                    while ((w = it.current()) != NULL){
                        if (w->inherits("Container")){
                            container =  static_cast<Container*>(w);
                            if (container->getId() == id)
                                break;
                            container = NULL;
                        }
                        ++it;
                    }
                    delete list;
                    if (container == NULL){
                        container = new Container(id);
                        bNew = true;
                    }
                }else{
                    unsigned max_id = 0;
                    QWidgetList  *list = QApplication::topLevelWidgets();
                    QWidgetListIt it(*list);
                    QWidget * w;
                    while ((w = it.current()) != NULL){
                        if (w->inherits("Container")){
                            container =  static_cast<Container*>(w);
                            if (!(container->getId() & CONTAINER_GRP)){
                                if (max_id < container->getId())
                                    max_id = container->getId();
                            }
                        }
                        ++it;
                    }
                    delete list;
                    container = new Container(max_id + 1);
                    bNew = true;
                    if (getContainerMode() == 0)
                        container->setReceived((*msg)->getFlags() & MESSAGE_RECEIVED);
                }
                container->addUserWnd(userWnd, ((*msg)->getFlags() & MESSAGE_NORAISE) == 0);
            }else{
                if (((*msg)->getFlags() & MESSAGE_NORAISE) == 0)
                    container->raiseUserWnd(userWnd);
            }
            container->setNoSwitch(true);
            userWnd->setMessage(msg);
            if ((*msg)->getFlags() & MESSAGE_NORAISE){
                if (bNew){
                    container->m_bNoRead = true;
#ifdef WIN32
                    ShowWindow(container->winId(), SW_SHOWMINNOACTIVE);
#else
                    container->init();
                    container->showMinimized();
#endif
                }
                if (m_focus)
                    m_focus->setFocus();
            }else{
                container->init();
                container->show();
                raiseWindow(container);
            }
            container->setNoSwitch(false);
            if (m_focus)
                disconnect(m_focus, SIGNAL(destroyed()), this, SLOT(focusDestroyed()));
            m_focus = NULL;
            return e->param();
        }
    case EventContactOnline:{
            Contact *contact = getContacts()->contact((unsigned long)(e->param()));
            if (contact){
                CoreUserData *data = (CoreUserData*)(contact->getUserData(user_data_id));
                if (data->OpenOnOnline.toBool()){
                    Message *msg = new Message(MessageGeneric);
                    msg->setContact(contact->id());
                    Event e(EventOpenMessage, &msg);
                    e.process();
                }
            }
            return NULL;
        }
    case EventCheckState:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if (cmd->menu_id == MenuEncoding){
                if (cmd->id == CmdChangeEncoding){
                    Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                    if (contact == NULL)
                        return NULL;
                    QTextCodec *codec = getContacts()->getCodec(contact);
                    unsigned nEncoding = 3;
                    QStringList main;
                    QStringList nomain;
                    QStringList::Iterator it;
                    const ENCODING *enc;
                    for (enc = getContacts()->getEncodings(); enc->language; enc++){
                        if (enc->bMain){
                            main.append(i18n(enc->language) + " (" + enc->codec + ")");
                            nEncoding++;
                            continue;
                        }
                        if (!getShowAllEncodings())
                            continue;
                        nomain.append(i18n(enc->language) + " (" + enc->codec + ")");
                        nEncoding++;
                    }
                    CommandDef *cmds = new CommandDef[nEncoding];
                    cmd->param = cmds;
                    cmd->flags |= COMMAND_RECURSIVE;
                    nEncoding = 0;
                    cmds[nEncoding].id = 1;
                    cmds[nEncoding].text = I18N_NOOP("System");
                    if (!strcmp(codec->name(), "System"))
                        cmds[nEncoding].flags = COMMAND_CHECKED;
                    nEncoding++;
                    main.sort();
                    for (it = main.begin(); it != main.end(); ++it){
                        QString str = *it;
                        int n = str.find('(');
                        str = str.mid(n + 1);
                        n = str.find(')');
                        str = str.left(n);
                        if (str == codec->name())
                            cmds[nEncoding].flags = COMMAND_CHECKED;
                        cmds[nEncoding].id = nEncoding + 1;
                        cmds[nEncoding].text = "_";
                        cmds[nEncoding].text_wrk = (*it);
                        nEncoding++;
                    }
                    if (!getShowAllEncodings())
                        return e->param();
                    cmds[nEncoding++].text = "_";
                    nomain.sort();
                    for (it = nomain.begin(); it != nomain.end(); ++it){
                        QString str = *it;
                        int n = str.find('(');
                        str = str.mid(n + 1);
                        n = str.find(')');
                        str = str.left(n);
                        if (str == codec->name())
                            cmds[nEncoding].flags = COMMAND_CHECKED;
                        cmds[nEncoding].id = nEncoding;
                        cmds[nEncoding].text = "_";
                        cmds[nEncoding].text_wrk = (*it);
                        nEncoding++;
                    }
                    return e->param();
                }
                if (cmd->id == CmdAllEncodings){
                    cmd->flags &= ~COMMAND_CHECKED;
                    if (getShowAllEncodings())
                        cmd->flags |= COMMAND_CHECKED;
                    return e->param();
                }
            }
            if (cmd->id == CmdEnableSpell){
                cmd->flags &= ~COMMAND_CHECKED;
                if (getEnableSpell())
                    cmd->flags |= COMMAND_CHECKED;
                return e->param();
            }
            if (cmd->id == CmdSendClose){
                cmd->flags &= ~COMMAND_CHECKED;
                if (getCloseSend())
                    cmd->flags |= COMMAND_CHECKED;
                return NULL;
            }
            if ((cmd->id == CmdFileAccept) || (cmd->id == CmdFileDecline)){
                Message *msg = (Message*)(cmd->param);
                if (msg->getFlags() & MESSAGE_TEMP)
                    return e->param();
                return NULL;
            }
            if (cmd->id == CmdContactClients){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                vector<clientContact> ways;
                getWays(ways, contact);
                if (cmd->menu_id == MenuMessage){
                    unsigned n = ways.size();
                    if (n < 1)
                        return NULL;
                    if (n == 1){
                        QString resources = ways[0].client->resources(ways[0].data);
                        if (resources.isEmpty())
                            return NULL;
                        QString wrk = resources;
                        unsigned n = 0;
                        while (!wrk.isEmpty()){
                            getToken(wrk, ';');
                            n++;
                        }
                        CommandDef *cmds = new CommandDef[n + 2];
                        cmds[0].text = "_";
                        n = 1;
                        while (!resources.isEmpty()){
                            unsigned long id = CmdContactResource + n;
                            if (n > m_nResourceMenu){
                                m_nResourceMenu = n;
                                Event eMenu(EventMenuCreate, (void*)id);
                                eMenu.process();
                                Command cmd;
                                cmd->id			= CmdContactClients;
                                cmd->text		= "_";
                                cmd->menu_id	= id;
                                cmd->menu_grp	= 0x1000;
                                cmd->flags		= COMMAND_CHECK_STATE;
                                Event eCmd(EventCommandCreate, cmd);
                                eCmd.process();
                            }
                            cmds[n].id		 = id;
                            cmds[n].text	 = "_";
                            cmds[n].popup_id = id;
                            QString res = getToken(resources, ';');
                            cmds[n].icon     = (const char*)(getToken(res, ',').toULong());
                            QString t = ways[0].client->contactName(ways[0].data);
                            t += '/' + res;
                            cmds[n].text_wrk = t;
                            n++;
                        }
                        cmd->param = cmds;
                        cmd->flags |= COMMAND_RECURSIVE;
                        return e->param();
                    }
                    CommandDef *cmds = new CommandDef[n + 2];
                    cmds[0].text = "_";
                    n = 1;
                    for (vector<clientContact>::iterator itw = ways.begin(); itw != ways.end(); ++itw, n++){
                        unsigned long id  = CmdContactClients + n;
                        if (n > m_nClientsMenu){
                            m_nClientsMenu = n;
                            Event eMenu(EventMenuCreate, (void*)id);
                            eMenu.process();

                            Command cmd;
                            cmd->id			= CmdContactClients;
                            cmd->text		= "_";
                            cmd->menu_id	= id;
                            cmd->menu_grp	= 0x1000;
                            cmd->flags		= COMMAND_CHECK_STATE;
                            Event eCmd(EventCommandCreate, cmd);
                            eCmd.process();

                            cmd->id			= CmdSeparate;
                            cmd->text		= I18N_NOOP("&Separate");
                            cmd->menu_grp	= 0x2000;
                            cmd->flags		= COMMAND_DEFAULT;
                            eCmd.process();
                        }
                        cmds[n].id		 = id;
                        cmds[n].text	 = "_";
                        cmds[n].popup_id = id;
                        unsigned long status = STATUS_UNKNOWN;
                        unsigned style = 0;
                        QString statusIcon;
                        if ((*itw).bNew){
                            void *data = (*itw).data;
                            Client *client = contact->clientData.activeClient(data, (*itw).client);
                            if (client == NULL){
                                client = (*itw).client;
                                data   = (*itw).data;
                            }
                            client->contactInfo(data, status, style, statusIcon);
                        }else{
                            (*itw).client->contactInfo((*itw).data, status, style, statusIcon);
                        }
                        cmds[n].icon = statusIcon;
                        QString t = (*itw).client->contactName((*itw).data);
                        bool bFrom = false;
                        for (unsigned i = 0; i < getContacts()->nClients(); i++){
                            Client *client = getContacts()->getClient(i);
                            if (client == (*itw).client)
                                continue;
                            Contact *contact;
                            clientData *data = (*itw).data;
                            if (client->isMyData(data, contact)){
                                bFrom = true;
                                break;
                            }
                        }
                        if (bFrom){
                            t += ' ';
                            t += i18n("from %1") .arg((*itw).client->name());
                        }
                        cmds[n].text_wrk = t;
                    }
                    cmd->param = cmds;
                    cmd->flags |= COMMAND_RECURSIVE;
                    return e->param();
                }
                if (cmd->menu_id > CmdContactResource){
                    unsigned nRes = cmd->menu_id - CmdContactResource - 1;
                    unsigned n;
                    for (n = 0; n < ways.size(); n++){
                        QString resources = ways[n].client->resources(ways[n].data);
                        while (!resources.isEmpty()){
                            getToken(resources, ';');
                            if (nRes-- == 0){
                                clientContact &cc = ways[n];
                                Event eMenu(EventGetMenuDef, (void*)MenuMessage);
                                CommandsDef *cmdsMsg = (CommandsDef*)(eMenu.process());
                                unsigned nCmds = 0;
                                {
                                    CommandsList it(*cmdsMsg, true);
                                    while (++it)
                                        nCmds++;
                                }
                                CommandDef *cmds = new CommandDef[nCmds];
                                nCmds = 0;

                                CommandsList it(*cmdsMsg, true);
                                CommandDef *c;
                                while ((c = ++it) != NULL){
                                    if ((c->id == MessageSMS) && (cc.client->protocol()->description()->flags & PROTOCOL_NOSMS))
                                        continue;
                                    if (!cc.client->canSend(c->id, cc.data)){
                                        CheckSend cs;
                                        cs.id     = c->id;
                                        cs.data   = cc.data;
                                        cs.client = cc.client;
                                        Event e(EventCheckSend, &cs);
                                        if (!e.process())
                                            continue;
                                    }
                                    cmds[nCmds] = *c;
                                    cmds[nCmds].id      = c->id;
                                    cmds[nCmds].flags	= COMMAND_DEFAULT;
                                    cmds[nCmds].menu_id = cmd->menu_id;
                                    nCmds++;
                                }
                                cmd->param = cmds;
                                cmd->flags |= COMMAND_RECURSIVE;
                                return e->param();
                            }
                        }
                    }
                    return NULL;
                }
                unsigned n = cmd->menu_id - CmdContactClients - 1;
                if (n >= ways.size())
                    return NULL;
                clientContact &cc = ways[n];

                Event eMenu(EventGetMenuDef, (void*)MenuMessage);
                CommandsDef *cmdsMsg = (CommandsDef*)(eMenu.process());
                unsigned nCmds = 0;
                {
                    CommandsList it(*cmdsMsg, true);
                    while (++it)
                        nCmds++;
                }
                QString resources = cc.client->resources(cc.data);
                if (!resources.isEmpty()){
                    nCmds++;
                    while (!resources.isEmpty()){
                        getToken(resources, ';');
                        nCmds++;
                    }
                }

                CommandDef *cmds = new CommandDef[nCmds];
                nCmds = 0;

                CommandsList it(*cmdsMsg, true);
                CommandDef *c;
                while ((c = ++it) != NULL){
                    if ((c->id == MessageSMS) && (cc.client->protocol()->description()->flags & PROTOCOL_NOSMS))
                        continue;
                    if (!cc.client->canSend(c->id, cc.data)){
                        CheckSend cs;
                        cs.id     = c->id;
                        cs.data   = cc.data;
                        cs.client = cc.client;
                        Event e(EventCheckSend, &cs);
                        if (!e.process())
                            continue;
                    }
                    cmds[nCmds] = *c;
                    cmds[nCmds].id      = c->id;
                    cmds[nCmds].flags	= COMMAND_DEFAULT;
                    cmds[nCmds].menu_id = cmd->menu_id;
                    nCmds++;
                }
                resources = cc.client->resources(cc.data);
                if (!resources.isEmpty()){
                    cmds[nCmds++].text = "_";
                    unsigned nRes = 1;
                    for (unsigned i = 0; i < n; i++){
                        resources = ways[i].client->resources(ways[i].data);
                        while (!resources.isEmpty()){
                            getToken(resources, ';');
                            unsigned long id = CmdContactResource + nRes;
                            if (nRes > m_nResourceMenu){
                                m_nResourceMenu = nRes;
                                Event eMenu(EventMenuCreate, (void*)id);
                                eMenu.process();
                                Command cmd;
                                cmd->id			= CmdContactClients;
                                cmd->text		= "_";
                                cmd->menu_id	= id;
                                cmd->menu_grp	= 0x1000;
                                cmd->flags		= COMMAND_CHECK_STATE;
                                Event eCmd(EventCommandCreate, cmd);
                                eCmd.process();
                            }
                            nRes++;
                        }
                    }
                    resources = cc.client->resources(cc.data);
                    while (!resources.isEmpty()){
                        unsigned long id = CmdContactResource + nRes;
                        if (nRes > m_nResourceMenu){
                            m_nResourceMenu = nRes;
                            Event eMenu(EventMenuCreate, (void*)id);
                            eMenu.process();
                            Command cmd;
                            cmd->id			= CmdContactClients;
                            cmd->text		= "_";
                            cmd->menu_id	= id;
                            cmd->menu_grp	= 0x1000;
                            cmd->flags		= COMMAND_CHECK_STATE;
                            Event eCmd(EventCommandCreate, cmd);
                            eCmd.process();
                        }
                        cmds[nCmds].id		 = id;
                        cmds[nCmds].text	 = "_";
                        cmds[nCmds].popup_id = id;
                        QString res = getToken(resources, ';');
                        cmds[nCmds].icon     = (const char*)(getToken(res, ',').toULong());
                        QString t = cc.client->contactName(ways[0].data);
                        t += '/' + res;
                        cmds[nCmds++].text_wrk = t;
                        nRes++;
                    }
                }
                cmd->param = cmds;
                cmd->flags |= COMMAND_RECURSIVE;

                return e->param();
            }
            if (cmd->menu_id == MenuContainer){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact){
                    unsigned nContainers = 1;
                    QWidgetList  *list = QApplication::topLevelWidgets();
                    QWidgetListIt it(*list);
                    QWidget * w;
                    while ((w = it.current()) != NULL){
                        if (w->inherits("Container"))
                            nContainers++;
                        ++it;
                    }
                    CommandDef *cmds = new CommandDef[nContainers + 1];
                    unsigned n = 0;
                    QWidgetListIt it1(*list);
                    while ((w = it1.current()) != NULL){
                        if (w->inherits("Container")){
                            Container *c = static_cast<Container*>(w);
                            cmds[n] = *cmd;
                            cmds[n].icon = QString::null;
                            cmds[n].id = c->getId();
                            cmds[n].flags = COMMAND_DEFAULT;
                            cmds[n].text_wrk = c->name();
                            if (c->wnd(contact->id()))
                                cmds[n].flags |= COMMAND_CHECKED;
                            n++;
                        }
                        ++it1;
                    }
                    cmds[n].icon = QString::null;
                    cmds[n].id = NEW_CONTAINER;
                    cmds[n].flags = COMMAND_DEFAULT;
                    cmds[n].text = I18N_NOOP("&New");
                    delete list;
                    cmd->param = cmds;
                    cmd->flags |= COMMAND_RECURSIVE;
                    return e->param();
                }
            }
            if (cmd->menu_id == MenuMessage){
                cmd->flags &= ~COMMAND_CHECKED;
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact){
                    vector<clientContact> ways;
                    getWays(ways, contact);
                    for (vector<clientContact>::iterator it = ways.begin(); it != ways.end(); ++it){
                        if ((cmd->id == MessageSMS) && ((*it).client->protocol()->description()->flags & PROTOCOL_NOSMS))
                            return NULL;
                        if ((*it).client->canSend(cmd->id, (*it).data)){
                            return e->param();
                        }
                    }
                    if ((cmd->id == MessageSMS) && !ways.empty()){
                        vector<clientContact>::iterator it;
                        for (it = ways.begin(); it != ways.end(); ++it){
                            if (((*it).client->protocol()->description()->flags & PROTOCOL_NOSMS) == 0)
                                break;
                        }
                        if (it == ways.end())
                            return NULL;
                    }
                }
                for (unsigned i = 0; i < getContacts()->nClients(); i++){
                    if (getContacts()->getClient(i)->canSend(cmd->id, NULL))
                        return e->param();
                }
                return NULL;
            }
            if (cmd->menu_id == MenuMsgCommand){
                Message *msg = (Message*)(cmd->param);
                switch (cmd->id){
                case CmdMsgQuote:
                case CmdMsgForward:
                    if ((msg->getFlags() & MESSAGE_RECEIVED) == 0)
                        return NULL;
                    QString p = msg->presentation();
                    if (!p.isEmpty()){
                        unsigned type = msg->baseType();
                        switch (type){
                        case MessageFile:
                            return NULL;
                        }
                        cmd->flags &= ~COMMAND_CHECKED;
                        return e->param();
                    }
                    break;
                }
                return NULL;
            }
            if (cmd->menu_id == MenuPhoneState){
                cmd->flags &= ~COMMAND_CHECKED;
                if (cmd->id == CmdPhoneNoShow + getContacts()->owner()->getPhoneStatus())
                    cmd->flags |= COMMAND_CHECKED;
                return e->param();
            }
            if ((cmd->menu_id == MenuLocation) && (cmd->id == CmdLocation)){
                unsigned n = 2;
                QString phones = getContacts()->owner()->getPhones();
                while (!phones.isEmpty()){
                    getToken(phones, ';');
                    n++;
                }
                CommandDef *cmds = new CommandDef[n];
                n = 0;
                cmds[n].id      = CmdLocation;
                cmds[n].text    = I18N_NOOP("Not available");
                cmds[n].menu_id = MenuLocation;
                phones = getContacts()->owner()->getPhones();
                bool bActive = false;
                while (!phones.isEmpty()){
                    n++;
                    QString item = getToken(phones, ';', false);
                    item = getToken(item, '/', false);
                    QString number = getToken(item, ',');
                    getToken(item, ',');
                    unsigned long icon = getToken(item, ',').toULong();
                    cmds[n].id   = CmdLocation + n;
                    cmds[n].text = "_";
                    cmds[n].menu_id  = MenuLocation;
                    cmds[n].text_wrk = number;
                    if (!item.isEmpty()){
                        cmds[n].flags = COMMAND_CHECKED;
                        bActive = true;
                    }
                    switch (icon){
                    case PHONE:
                        cmds[n].icon = "phone";
                        break;
                    case FAX:
                        cmds[n].icon = "fax";
                        break;
                    case CELLULAR:
                        cmds[n].icon = "cell";
                        break;
                    case PAGER:
                        cmds[n].icon = "pager";
                        break;
                    }
                }
                if (!bActive)
                    cmds[0].flags = COMMAND_CHECKED;
                cmd->param = cmds;
                cmd->flags |= COMMAND_RECURSIVE;
                return e->param();
            }
            if (cmd->id == CmdUnread){
                unsigned contact_id = 0;
                if (cmd->menu_id == MenuContact){
                    Contact *contact = (Contact*)(e->param());
                    contact_id = contact->id();
                }
                MAP_COUNT count;
                MAP_COUNT::iterator itc;
                CommandDef *def;
                unsigned n = 0;
                for (list<msg_id>::iterator it = unread.begin(); it != unread.end(); ++it, n++){
                    if (contact_id && ((*it).contact != contact_id))
                        continue;
                    msgIndex m;
                    m.contact = (*it).contact;
                    m.type    = (*it).type;
                    itc = count.find(m);
                    if (itc == count.end()){
                        msgCount c;
                        c.index = n;
                        c.count = 1;
                        count.insert(MAP_COUNT::value_type(m, c));
                    }else{
                        msgCount &c = (*itc).second;
                        c.index = n;
                        c.count++;
                    }
                }
                if (count.empty())
                    return NULL;
                CommandDef *cmds = new CommandDef[count.size() + 1];
                n = 0;
                for (itc = count.begin(); itc != count.end(); ++itc, n++){
                    cmds[n].id = CmdUnread + (*itc).second.index;
                    def = messageTypes.find((*itc).first.type);
                    if (def == NULL)
                        continue;
                    MessageDef *mdef = (MessageDef*)(def->param);
                    cmds[n].icon = def->icon;
                    QString msg = i18n(mdef->singular, mdef->plural, (*itc).second.count);
                    if (!msg) {
                        log(L_ERROR, "Message is missing some definitions! Text: %s, ID: %lu",
                            (const char *)def->text.local8Bit(), def->id);
                        int cnt = (*itc).second.count;
                        msg = QString("%1").arg(cnt);
                    }
                    if ((*itc).second.count == 1){
                        int n = msg.find("1 ");
                        if (n == 0){
                            msg = msg.left(1).upper() + msg.mid(1);
                        }else{
                            msg = msg.left(n - 1);
                        }
                    }
                    if (contact_id == 0){
                        Contact *contact = getContacts()->contact((*itc).first.contact);
                        if (contact == NULL)
                            continue;
                        msg = i18n("%1 from %2")
                              .arg(msg)
                              .arg(contact->getName());
                    }
                    cmds[n].text_wrk = msg;
                    cmds[n].text = "_";
                }
                cmd->param = cmds;
                cmd->flags |= COMMAND_RECURSIVE;
                return e->param();
            }
            if (cmd->id == CmdSendSMS){
                cmd->flags &= COMMAND_CHECKED;
                for (unsigned i = 0; i < getContacts()->nClients(); i++){
                    Client *client = getContacts()->getClient(i);
                    if (client->canSend(MessageSMS, NULL))
                        return e->param();
                }
                return NULL;
            }
            if (cmd->id == CmdShowPanel){
                cmd->flags &= ~COMMAND_CHECKED;
                if (m_statusWnd)
                    cmd->flags |= COMMAND_CHECKED;
                return e->param();
            }
            if ((cmd->id == CmdContainer) && (cmd->menu_id == MenuContact)){
                if (getContainerMode())
                    return e->param();
                return NULL;
            }
            if (cmd->id == CmdCommonStatus){
                unsigned n = cmd->menu_id - CmdClient;
                if (n >= getContacts()->nClients())
                    return false;
                Client *client = getContacts()->getClient(n);
                cmd->flags &= ~COMMAND_CHECKED;
                if (client->getCommonStatus())
                    cmd->flags |= COMMAND_CHECKED;
                return e->param();
            }
            if (cmd->id == CmdTitle)
                if (cmd->param && adjustClientItem(cmd->menu_id, cmd)){
                    return e->param();
                }else{
                    return NULL;
                }
            if (adjustClientItem(cmd->id, cmd))
                return e->param();
            unsigned n = cmd->menu_id - CmdClient;
            if (n > getContacts()->nClients())
                return NULL;
            Client *client = getContacts()->getClient(n);
            if (cmd->id == CmdInvisible){
                if (client->getInvisible()){
                    cmd->flags |= COMMAND_CHECKED;
                }else{
                    cmd->flags &= ~COMMAND_CHECKED;
                }
                return e->param();
            }
            const CommandDef *curStatus = NULL;
            const CommandDef *d;
            for (d = client->protocol()->statusList(); !d->text.isEmpty(); d++){
                if (d->id == cmd->id)
                    curStatus = d;
            }
            if (curStatus == NULL)
                return 0;
            bool bChecked = false;
            unsigned status = client->getManualStatus();
            bChecked = (status == curStatus->id);
            if (bChecked){
                cmd->flags |= COMMAND_CHECKED;
            }else{
                cmd->flags &= ~COMMAND_CHECKED;
            }
            return e->param();
        }
    case EventCommandExec:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if (cmd->menu_id == MenuEncoding){
                if (cmd->id == CmdAllEncodings){
                    Command c;
                    c->id     = CmdChangeEncoding;
                    c->param  = cmd->param;
                    Event eWidget(EventCommandWidget, c);
                    QToolButton *btn = (QToolButton*)(eWidget.process());
                    if (btn)
                        QTimer::singleShot(0, btn, SLOT(animateClick()));
                    setShowAllEncodings(!getShowAllEncodings());
                    return e->param();
                }
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                QCString codecStr;
                const char *codec = NULL;
                if (cmd->id == 1){
                    codec = "-";
                }else{
                    QStringList main;
                    QStringList nomain;
                    QStringList::Iterator it;
                    const ENCODING *enc;
                    for (enc = getContacts()->getEncodings(); enc->language; enc++){
                        if (enc->bMain){
                            main.append(i18n(enc->language) + " (" + enc->codec + ")");
                            continue;
                        }
                        if (!getShowAllEncodings())
                            continue;
                        nomain.append(i18n(enc->language) + " (" + enc->codec + ")");
                    }
                    QString str;
                    main.sort();
                    int n = cmd->id - 1;
                    for (it = main.begin(); it != main.end(); ++it){
                        if (--n == 0){
                            str = *it;
                            break;
                        }
                    }
                    if (n >= 0){
                        nomain.sort();
                        for (it = nomain.begin(); it != nomain.end(); ++it){
                            if (--n == 0){
                                str = *it;
                                break;
                            }
                        }
                    }
                    if (!str.isEmpty()){
                        int n = str.find('(');
                        str = str.mid(n + 1);
                        n = str.find(')');
                        codecStr = str.left(n).latin1();
                        codec = codecStr;
                    }
                }
                if (codec == NULL)
                    return NULL;
                if (contact->setEncoding(codec)){
                    Event eContact(EventContactChanged, contact);
                    eContact.process();
                    Event eh(EventHistoryConfig, (void*)(contact->id()));
                    eh.process();
                }
                return NULL;
            }
            if (cmd->id == CmdEnableSpell){
                setEnableSpell(cmd->flags & COMMAND_CHECKED);
                return NULL;
            }
            if (cmd->menu_id == MenuMessage){
                Message *msg;
                CommandDef *def = messageTypes.find(cmd->id);
                if (def == NULL)
                    return NULL;
                MessageDef *mdef = (MessageDef*)(def->param);
                if (mdef->create == NULL)
                    return NULL;
                msg = mdef->create(NULL);
                msg->setContact((unsigned long)(cmd->param));
                if (mdef->flags & MESSAGE_SILENT){
                    Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                    if (contact){
                        ClientDataIterator it(contact->clientData);
                        void *data;
                        while ((data = ++it) != NULL){
                            Client *client = it.client();
                            if (client->canSend(msg->type(), data) && client->send(msg, data))
                                break;
                        }
                    }
                    return e->param();
                }
                Event eOpen(EventOpenMessage, &msg);
                eOpen.process();
                delete msg;
                return e->param();
            }
            if (cmd->menu_id == MenuMsgCommand){
                Message *msg = (Message*)(cmd->param);
                QString p;
                switch (cmd->id){
                case CmdMsgQuote:
                case CmdMsgForward:
                    p = msg->presentation();
                    if (p.isEmpty())
                        return NULL;
                    p = unquoteText(p);
                    QStringList l = QStringList::split("\n", p);
                    QStringList::Iterator it;
                    if (l.count() && l.last().isEmpty()){
                        it = l.end();
                        --it;
                        l.remove(it);
                    }
                    for (it = l.begin(); it != l.end(); ++it)
                        (*it) = QString(">") + (*it);
                    p = l.join("\n");
                    Message *m = new Message(MessageGeneric);
                    m->setContact(msg->contact());
                    m->setClient(msg->client());
                    if (cmd->id == CmdMsgForward){
                        QString name;
                        Contact *contact = getContacts()->contact(msg->contact());
                        if (contact)
                            name = contact->getName();
                        p = g_i18n("%1 wrote:", contact) .arg(name) + "\n" + p;
                        m->setFlags(MESSAGE_FORWARD);
                    }else{
                        m->setFlags(MESSAGE_INSERT);
                    }
                    m->setText(p);
                    Event eOpen(EventOpenMessage, &m);
                    eOpen.process();
                    delete m;
                    return e->param();
                }
                return NULL;
            }
            if (cmd->id == CmdGrantAuth){
                Message *from = (Message*)(cmd->param);
                Message *msg = new AuthMessage(MessageAuthGranted);
                msg->setContact(from->contact());
                msg->setClient(from->client());
                Contact *contact = getContacts()->contact(msg->contact());
                if (contact){
                    void *data;
                    ClientDataIterator it(contact->clientData);
                    while ((data = ++it) != NULL){
                        Client *client = it.client();
                        if (!from->client().isEmpty()){
                            if ((client->dataName(data) == from->client()) && client->send(msg, data))
                                return e->param();
                        }else{
                            if (client->canSend(MessageAuthGranted, data) && client->send(msg, data))
                                return e->param();
                        }
                    }
                }
                delete msg;
                return e->param();
            }
            if (cmd->id == CmdRefuseAuth){
                Message *from = (Message*)(cmd->param);
                Message *msg = new AuthMessage(MessageAuthRefused);
                msg->setContact(from->contact());
                msg->setClient(from->client());
                Event eOpen(EventOpenMessage, &msg);
                eOpen.process();
                delete msg;
                return e->param();
            }

            if (cmd->id == CmdSeparate){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                unsigned n = cmd->menu_id - CmdContactClients - 1;
                vector<clientContact> ways;
                getWays(ways, contact);
                if (n >= ways.size())
                    return NULL;
                clientContact &cc = ways[n];
                clientData *data;
                ClientDataIterator it(contact->clientData, cc.client);
                while ((data = ++it) != NULL){
                    if (data == cc.data)
                        break;
                }
                if (data == NULL){
                    data = cc.data;
                    cc.client->createData(data, contact);
                }
                Contact *newContact = getContacts()->contact(0, true);
                newContact->setGroup(contact->getGroup());
                newContact->clientData.join(data, contact->clientData);
                contact->setup();
                newContact->setup();
                Event e1(EventContactChanged, contact);
                e1.process();
                Event e2(EventContactChanged, newContact);
                e2.process();
                return e->param();
            }
            if (cmd->id == CmdSendClose){
                setCloseSend((cmd->flags & COMMAND_CHECKED) != 0);
                return e->param();
            }
            if (cmd->id == CmdSendSMS){
                Contact *contact = getContacts()->contact(0, true);
                contact->setFlags(CONTACT_TEMP);
                contact->setName(i18n("Send SMS"));
                Event eChanged(EventContactChanged, contact);
                eChanged.process();
                Command cmd;
                cmd->id      = MessageSMS;
                cmd->menu_id = MenuMessage;
                cmd->param   = (void*)(contact->id());
                Event eCmd(EventCommandExec, cmd);
                eCmd.process();
                return e->param();
            }
            if (cmd->id == CmdHistory){
                unsigned long id = (unsigned long)(cmd->param);
                if (!getUseExtViewer()){
                    HistoryWindow *wnd = NULL;
                    QWidgetList  *list = QApplication::topLevelWidgets();
                    QWidgetListIt it(*list);
                    QWidget * w;
                    while ((w = it.current()) != NULL){
                           if (w->inherits("HistoryWindow")){
                               wnd =  static_cast<HistoryWindow*>(w);
                               if (wnd->id() == id)
                                   break;
                               wnd = NULL;
                           }
                           ++it;
                    }
                    delete list;
                    if (wnd == NULL){
                        wnd = new HistoryWindow(id);
                        if (data.HistorySize[0].toULong() && data.HistorySize[1].toULong())
                            wnd->resize(data.HistorySize[0].toULong(), data.HistorySize[1].toULong());
                    }
                    raiseWindow(wnd);
                } else{
                    if (!m_HistoryThread)
                        m_HistoryThread = new HistoryThread();
                    m_HistoryThread->set_id(id);
                    m_HistoryThread->set_Viewer(getExtViewer());
                    m_HistoryThread->start();
                }
                return e->param();
            }
            if (cmd->id == CmdConfigure){
                if ((cmd->menu_id == MenuContact) || (cmd->menu_id == MenuGroup)){
                    showInfo(cmd);
                    return e->param();
                }
                if (m_cfg == NULL){
                    m_cfg = new ConfigureDialog;
                    connect(m_cfg, SIGNAL(finished()), this, SLOT(dialogFinished()));
                    if ((data.CfgGeometry[WIDTH].toLong() == 0) || (data.CfgGeometry[HEIGHT].toLong() == 0)){
                        data.CfgGeometry[WIDTH].asLong()  = 500;
                        data.CfgGeometry[HEIGHT].asLong() = 380;
                        restoreGeometry(m_cfg, data.CfgGeometry, false, true);
                    }else{
                        restoreGeometry(m_cfg, data.CfgGeometry, true, true);
                    }
                }
                raiseWindow(m_cfg);
                return e->param();
            }
            if (cmd->id == CmdSearch){
                if (m_search == NULL){
                    m_search = new SearchDialog;
                    connect(m_search, SIGNAL(finished()), this, SLOT(dialogFinished()));
                    if ((data.SearchGeometry[WIDTH].toLong() == 0) || (data.SearchGeometry[HEIGHT].toLong() == 0)){
                        data.SearchGeometry[WIDTH].asLong()  = 500;
                        data.SearchGeometry[HEIGHT].asLong() = 380;
                        restoreGeometry(m_search, data.SearchGeometry, false, true);
                    }else{
                        restoreGeometry(m_search, data.SearchGeometry, true, true);
                    }
                }
                raiseWindow(m_search);
                return NULL;
            }
            if ((cmd->menu_id == MenuContact) || (cmd->menu_id == MenuGroup)){
                if (cmd->id == CmdInfo){
                    showInfo(cmd);
                    return e->param();
                }
                CommandDef *def = preferences.find(cmd->id);
                if (def){
                    showInfo(cmd);
                    return e->param();
                }
            }
            if (cmd->menu_id == MenuPhoneState){
                Contact *owner = getContacts()->owner();
                if (owner->getPhoneStatus() != cmd->id - CmdPhoneNoShow){
                    owner->setPhoneStatus(cmd->id - CmdPhoneNoShow);
                    Event e(EventContactChanged, owner);
                    e.process();
                }
                return e->param();
            }
            if (cmd->menu_id == MenuLocation){
                Contact *owner = getContacts()->owner();
                unsigned n = cmd->id - CmdLocation;
                QString res;
                QString phones = owner->getPhones();
                while (!phones.isEmpty()){
                    QString item = getToken(phones, ';', false);
                    QString v = getToken(item, '/', false);
                    QString number = getToken(v, ',', false);
                    QString type = getToken(v, ',', false);
                    QString icon = getToken(v, ',', false);
                    v = number + "," + type + "," + icon;
                    if (--n == 0)
                        v += ",1";
                    if (!res.isEmpty())
                        res += ";";
                    res += v;
                }
                if (res != owner->getPhones()){
                    owner->setPhones(res);
                    Event e(EventContactChanged, owner);
                    e.process();
                }
                return e->param();
            }
            if (cmd->id == CmdSetup){
                unsigned n = cmd->menu_id - CmdClient;
                if (n >= getContacts()->nClients())
                    return NULL;
                Client *client = getContacts()->getClient(n);
                if (m_cfg == NULL){
                    m_cfg = new ConfigureDialog;
                    connect(m_cfg, SIGNAL(finished()), this, SLOT(dialogFinished()));
                }
                static_cast<ConfigureDialog*>(m_cfg)->raisePage(client);
                raiseWindow(m_cfg);
                return e->param();
            }
            if (cmd->id == CmdPhoneBook){
                if (m_cfg == NULL){
                    m_cfg = new ConfigureDialog;
                    connect(m_cfg, SIGNAL(finished()), this, SLOT(dialogFinished()));
                }
                static_cast<ConfigureDialog*>(m_cfg)->raisePhoneBook();
                raiseWindow(m_cfg);
                return e->param();
            }
            if (cmd->id == CmdCommonStatus){
                unsigned n = cmd->menu_id - CmdClient;
                if (n >= getContacts()->nClients())
                    return NULL;
                Client *client = getContacts()->getClient(n);
                if (cmd->flags & COMMAND_CHECKED){
                    client->setStatus(getManualStatus(), true);
                }else{
                    client->setStatus(client->getManualStatus(), false);
                }
                for (unsigned i = 0; i < getContacts()->nClients(); i++){
                    if (getContacts()->getClient(i)->getCommonStatus())
                        return e->param();
                }
                client = getContacts()->getClient(0);
                if (client){
                    client->setCommonStatus(true);
                    Event e(EventClientChanged, client);
                    e.process();
                }
                return e->param();
            }
            if (cmd->id == CmdChange){
                QTimer::singleShot(0, this, SLOT(selectProfile()));
                return e->param();
            }
            unsigned n = cmd->menu_id - CmdClient;
            if (n < getContacts()->nClients()){
                Client *client = getContacts()->getClient(n);
                if (cmd->id == CmdInvisible){
                    client->setInvisible(!client->getInvisible());
                    return e->param();
                }
                const CommandDef *d;
                const CommandDef *curStatus = NULL;
                for (d = client->protocol()->statusList(); !d->text.isEmpty(); d++){
                    if (d->id == cmd->id)
                        curStatus = d;
                }
                if (curStatus == NULL)
                    return NULL;
                if ((((cmd->id != STATUS_ONLINE) && (cmd->id != STATUS_OFFLINE)) ||
                        (client->protocol()->description()->flags & PROTOCOL_AR_OFFLINE))&&
                        (client->protocol()->description()->flags & (PROTOCOL_AR | PROTOCOL_AR_USER))){
                    QString noShow = CorePlugin::m_plugin->getNoShowAutoReply(cmd->id);
                    if (noShow.isEmpty()){
                        AutoReplyDialog dlg(cmd->id);
                        if (!dlg.exec())
                            return e->param();
                    }
                }
                client->setStatus(cmd->id, false);
                return e->param();
            }
            if ((cmd->id == CmdCM) || (cmd->id == CmdConnections)){
                if (m_manager == NULL){
                    m_manager = new ConnectionManager(false);
                    connect(m_manager, SIGNAL(finished()), this, SLOT(managerFinished()));
                }
                raiseWindow(m_manager);
                return e->param();
            }
            if (cmd->id == CmdFileAccept){
                Message *msg = (Message*)(cmd->param);
                Contact *contact = getContacts()->contact(msg->contact());
                CoreUserData *data = (CoreUserData*)(contact->getUserData(CorePlugin::m_plugin->user_data_id));
                QString dir;
                if (data)
                    dir = data->IncomingPath.str();
                if (!dir.isEmpty() && (!dir.endsWith("/")) && (!dir.endsWith("\\")))
                    dir += '/';
                dir = user_file(dir);
                messageAccept ma;
                ma.msg	     = msg;
                ma.dir	     = dir;
                ma.overwrite = Ask;
                Event e(EventMessageAccept, &ma);
                e.process();
            }
            if (cmd->id == CmdDeclineWithoutReason){
                messageDecline md;
                md.msg    = (Message*)(cmd->param);
                md.reason = QString::null;
                Event e(EventMessageDecline, &md);
                e.process();
            }
            if (cmd->id == CmdDeclineReasonBusy){
                messageDecline md;
                md.msg    = (Message*)(cmd->param);
                md.reason = i18n("Sorry, I'm busy right now, and can not respond to your request");
                Event e(EventMessageDecline, &md);
                e.process();
            }
            if (cmd->id == CmdDeclineReasonLater){
                messageDecline md;
                md.msg    = (Message*)(cmd->param);
                md.reason = i18n("Sorry, I'm busy right now, but I'll be able to respond to you later");
                Event e(EventMessageDecline, &md);
                e.process();
            }
            if (cmd->id == CmdDeclineReasonInput){
                Message *msg = (Message*)(cmd->param);
                QWidgetList  *list = QApplication::topLevelWidgets();
                QWidgetListIt it( *list );
                DeclineDlg *dlg = NULL;
                QWidget *w;
                while ( (w=it.current()) != 0 ) {
                    ++it;
                    if (w->inherits("DeclineDlg")){
                        dlg = static_cast<DeclineDlg*>(w);
                        if (dlg->message()->id() == msg->id())
                            break;
                        dlg = NULL;
                    }
                }
                delete list;
                if (dlg == NULL)
                    dlg = new DeclineDlg(msg);
                raiseWindow(dlg);
            }
            if ((cmd->id >= CmdUnread) && (cmd->id < CmdUnread + unread.size())){
                unsigned n = cmd->id - CmdUnread;
                for (list<msg_id>::iterator it = unread.begin(); it != unread.end(); ++it){
                    if (n-- == 0){
                        Message *msg = History::load((*it).id, (*it).client, (*it).contact);
                        if (msg){
                            msg->setFlags(msg->getFlags() & ~MESSAGE_NORAISE);
                            Event e(EventOpenMessage, &msg);
                            e.process();
                            delete msg;
                            break;
                        }
                    }
                }
                return e->param();
            }
            if ((cmd->menu_id > CmdContactResource) && (cmd->menu_id <= CmdContactResource + 0x100)){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                CommandDef *def = messageTypes.find(cmd->id);
                if (def && contact){
                    unsigned nRes = cmd->menu_id - CmdContactResource - 1;
                    vector<clientContact> ways;
                    getWays(ways, contact);
                    for (unsigned n = 0; n < ways.size(); n++){
                        QString resources = ways[n].client->resources(ways[n].data);
                        while (!resources.isEmpty()){
                            QString res = getToken(resources, ';');
                            if (nRes-- == 0){
                                clientContact &cc = ways[n];
                                clientData *data;
                                ClientDataIterator it(contact->clientData, cc.client);
                                while ((data = ++it) != NULL){
                                    if (data == cc.data)
                                        break;
                                }
                                if (data == NULL){
                                    data = cc.data;
                                    cc.client->createData(data, contact);
                                    Event e(EventContactChanged, contact);
                                    e.process();
                                }
                                getToken(res, ',');
                                MessageDef *mdef = (MessageDef*)(def->param);
                                Message *msg = mdef->create(NULL);
                                msg->setContact((unsigned long)(cmd->param));
                                msg->setClient(cc.client->dataName(data));
                                msg->setResource(res);
                                Event eOpen(EventOpenMessage, &msg);
                                eOpen.process();
                                delete msg;
                                return e->param();
                            }
                        }
                    }
                }
                return NULL;
            }
            if ((cmd->menu_id > CmdContactClients) && (cmd->menu_id <= CmdContactClients + 0x100)){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                CommandDef *def = messageTypes.find(cmd->id);
                if (def && contact){
                    unsigned n = cmd->menu_id - CmdContactClients - 1;
                    vector<clientContact> ways;
                    getWays(ways, contact);
                    if (n < ways.size()){
                        clientContact &cc = ways[n];

                        clientData *data;
                        ClientDataIterator it(contact->clientData, cc.client);
                        while ((data = ++it) != NULL){
                            if (data == cc.data)
                                break;
                        }
                        if (data == NULL){
                            data = cc.data;
                            cc.client->createData(data, contact);
                            Event e(EventContactChanged, contact);
                            e.process();
                        }

                        MessageDef *mdef = (MessageDef*)(def->param);
                        Message *msg = mdef->create(NULL);
                        msg->setContact((unsigned long)(cmd->param));
                        msg->setClient(cc.client->dataName(data));
                        Event eOpen(EventOpenMessage, &msg);
                        eOpen.process();
                        delete msg;
                        return e->param();
                    }
                }
            }
            if (cmd->id == CmdShowPanel){
                setShowPanel((cmd->flags & COMMAND_CHECKED) != 0);
                showPanel();
            }
        }
        return NULL;
    case EventGoURL:{
            QString url = *((QString*)(e->param()));
            QString proto;
            int n = url.find(':');
            if (n < 0)
                return NULL;
            proto = url.left(n);
            url = url.mid(n + 1);
            if (proto == "sms"){
                while (url[0] == '/')
                    url = url.mid(1);
                Contact *contact = getContacts()->contactByPhone(url);
                if (contact){
                    Command cmd;
                    cmd->id		 = MessageSMS;
                    cmd->menu_id = MenuMessage;
                    cmd->param	 = (void*)(contact->id());
                    Event eCmd(EventCommandExec, cmd);
                    eCmd.process();
                }
                return e->param();
            }
            if (proto != "sim")
                return NULL;
            unsigned long contact_id = url.toULong();
            Contact *contact = getContacts()->contact(contact_id);
            if (contact){
                Command cmd;
                cmd->id		 = MessageGeneric;
                cmd->menu_id = MenuMessage;
                cmd->param	 = (void*)contact_id;
                Event eCmd(EventCommandExec, cmd);
                eCmd.process();
            }
        }
        return NULL;
    }
    return NULL;
}

void CorePlugin::showInfo(CommandDef *cmd)
{
    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it( *list );
    UserConfig *cfg = NULL;
    QWidget *w;
    Contact *contact = NULL;
    Group   *group   = NULL;
    unsigned long id = (unsigned long)(cmd->param);
    if (cmd->menu_id == MenuContact){
        contact = getContacts()->contact(id);
        if (contact == NULL)
            return;
    }
    if (cmd->menu_id == MenuGroup){
        group = getContacts()->group(id);
        if (group == NULL)
            return;
    }
    if ((contact == NULL) && (group == NULL))
        return;
    while ( (w=it.current()) != 0 ) {
        ++it;
        if (w->inherits("UserConfig")){
            cfg = static_cast<UserConfig*>(w);
            if ((contact && (cfg->m_contact == contact)) ||
                    (group && (cfg->m_group == group)))
                break;
            cfg = NULL;
        }
    }
    delete list;
    if (cfg == NULL){
        cfg = new UserConfig(contact, group);
        if ((data.CfgGeometry[WIDTH].toLong() == 0) || (data.CfgGeometry[HEIGHT].toLong() == 0)){
            data.CfgGeometry[WIDTH].asLong()  = 500;
            data.CfgGeometry[HEIGHT].asLong() = 380;
        }
        cfg->resize(data.CfgGeometry[WIDTH].toLong(), data.CfgGeometry[HEIGHT].toLong());
    }
    raiseWindow(cfg);
    if (!cfg->raisePage(cmd->id))
        cfg->raiseDefaultPage();
}

void CorePlugin::dialogFinished()
{
    QTimer::singleShot(0, this, SLOT(dialogDestroy()));
}

void CorePlugin::dialogDestroy()
{
    if (m_cfg && !m_cfg->isVisible()){
        delete m_cfg;
        m_cfg = NULL;
    }
    if (m_search && !m_search->isVisible()){
        delete m_search;
        m_search = NULL;
    }
}

QWidget *CorePlugin::createConfigWindow(QWidget *parent)
{
    return new InterfaceConfig(parent);
}

void CorePlugin::hideWindows()
{
    QWidgetList *list = QApplication::topLevelWidgets();
    QWidgetListIt it(*list);
    QWidget * w;
    while ((w=it.current()) != NULL){
        ++it;
        w->hide();
    }
    delete list;
}

void CorePlugin::changeProfile()
{
    QString saveProfile = getProfile();
    destroy();
    getContacts()->clearClients();
    Event eUnload(EventPluginsUnload, static_cast<Plugin*>(this));
    eUnload.process();
    getContacts()->clear();
    preferences.clear();
    Event eLoad(EventPluginsLoad, static_cast<Plugin*>(this));
    eLoad.process();
    Event eInfo(EventGetPluginInfo, (void*)"_core");
    pluginInfo *info = (pluginInfo*)(eInfo.process());
    free_data(coreData, &data);
    load_data(coreData, &data, info->cfg);
    setStatusTime(time(NULL));
    if (info->cfg){
        delete info->cfg;
        info->cfg = NULL;
    }
    setProfile(saveProfile);
    removeTranslator();
    installTranslator();
    initData();
}

void CorePlugin::selectProfile()
{
    Event e(EventSaveState);
    e.process();
    init(false);
    Event e2(EventInit);
    e2.process();
}

bool CorePlugin::init(bool bInit)
{
    m_bInit = bInit;
    bool bLoaded = false;
    bool bRes = true;
    bool bNew = false;
    bool bCmdLineProfile = false;

    CmdParam p;
    p.arg   = "-profile:";
    p.descr = I18N_NOOP("Use specified profile");

    Event e(EventArg, &p);
    e.process();
    QString cmd_line_profile = p.value;
    if (!cmd_line_profile.isEmpty()){
        bCmdLineProfile = true;
        setProfile(QString::null);
        QString profileDir = user_file("");
        profileDir += cmd_line_profile;
        QDir d(profileDir);
        if (d.exists()) {
            bCmdLineProfile = false;
            setProfile(cmd_line_profile);
        }
    }

    p.arg   = "-uin:";
    p.descr = I18N_NOOP("Add new ICQ UIN to profile. You need to specify uin:password");
    p.value = QString::null;

    Event e1(EventArg, &p);
    if (e1.process() && !p.value.isEmpty()) {
        int idx = p.value.find(':');
        QString uin      = p.value.left(idx);
        QString passwd   = (idx != -1) ? p.value.mid(idx + 1) : QString::null;
        setICQUIN(uin);
        setICQPassword(passwd);

        if (!bCmdLineProfile){
            bool bRegistered = false;
            ClientList clients;
            loadClients(clients);
            unsigned i;
            QString clName, clID;
            for (i = 0; i < clients.size(); i++){
                 QString clName=clients[i]->name();
                 clID=clName.right(clName.length()-clName.find('.')-1);
                 if (clID.compare(uin)==0)
                     bRegistered=true;
            }
            setRegNew(!bRegistered);
        } else
           setRegNew(true);
    }
    if ((!bInit || getProfile().isEmpty() || !getNoShow() || !getSavePasswd()) && (cmd_line_profile.isEmpty() || (!cmd_line_profile.isEmpty() && !getSavePasswd()))){
        if (!bInit || m_profiles.size()){
            if (bInit)
                hideWindows();
            LoginDialog dlg(bInit, NULL, "", bInit ? "" : getProfile());
            if (dlg.exec() == 0){
                if (bInit || dlg.isChanged()){
                    Event eAbort(EventPluginsLoad, (void*)ABORT_LOADING);
                    eAbort.process();
                }
                return false;
            }
            if (dlg.isChanged())
                bRes = false;
            bLoaded = true;
        }
    }else if (bInit && !getProfile().isEmpty() && !bCmdLineProfile){
        if (!lockProfile(getProfile(), true)){
            Event eAbort(EventPluginsLoad, (void*)ABORT_LOADING);
            eAbort.process();
            return false;
        }
    }
    if (getProfile().isEmpty() || bCmdLineProfile){
        hideWindows();
        getContacts()->clearClients();

        NewProtocol *pDlg=NULL;
        if (bCmdLineProfile){
            setSavePasswd(true);
            setNoShow(true);
            pDlg = new NewProtocol(NULL,1,getRegNew());
        } else {
            pDlg = new NewProtocol(NULL);
        }
        if (!pDlg->exec() && !pDlg->connected()){
            delete(pDlg);
            Event eAbort(EventPluginsLoad, (void*)ABORT_LOADING);
            eAbort.process();
            return false;
        }
        delete(pDlg);

        Client *client = getContacts()->getClient(0);
        QString profile = client->name();
        setProfile(QString::null);
        QString profileDir = user_file("");
        if (!bCmdLineProfile){
            profileDir += profile;
            for (unsigned i = 1;;i++){
                 QDir d(profileDir + "." + QString::number(i));
                 if (!d.exists()){
                     profile += '.';
                     profile += QString::number(i);
                     break;
                 }
            }
        } else {
            profile = cmd_line_profile;
        }

        setProfile(profile);
        bLoaded = true;
        bRes = false;
        bNew = true;
    }
    if (!bLoaded){
        ClientList clients;
        loadClients(clients);
        clients.addToContacts();
    }
    if (!bNew)
        getContacts()->load();
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *client = getContacts()->getClient(i);
        // "Emulate" contactsLoaded() when we're dealing with a new contact list
        if (bNew)
            client->contactsLoaded();
        if (client->getCommonStatus())
            client->setManualStatus(getManualStatus());
        client->setStatus(client->getManualStatus(), client->getCommonStatus());
    }
    if (getRegNew()&&!bCmdLineProfile){
        hideWindows();
        NewProtocol pDlg(NULL,1,true);
        pDlg.exec();
     }
    if (m_main)
        return true;

    loadUnread();

    m_main = new MainWindow(data.geometry);
    m_view = new UserView;

    if (!bNew){
        QString containers = getContainers();
        while (!containers.isEmpty()){
            Container *c = new Container(0, getContainer(getToken(containers, ',').toULong()));
            c->init();
        }
    }
    clearContainer();
    setContainers(QString::null);

    m_bInit = true;
    loadMenu();
    if (!bRes){
        Event eSave(EventSaveState);
        eSave.process();
        return true;
    }
    return bRes || bNew;
}

void CorePlugin::destroy()
{
    QWidgetList  *l = QApplication::topLevelWidgets();
    QWidgetListIt it(*l);
    QWidget *w;
    list<QWidget*> forRemove;
    while ((w = it.current()) != NULL){
        ++it;
        if (w->inherits("Container") ||
                w->inherits("HistoryWindow") ||
                w->inherits("UserConfig"))
            forRemove.push_back(w);
    }
    delete l;
    for (list<QWidget*>::iterator itr = forRemove.begin(); itr != forRemove.end(); ++itr)
        delete *itr;

    if (m_statusWnd){
        delete m_statusWnd;
        m_statusWnd = NULL;
    }
    if (m_view){
        delete m_view;
        m_view = NULL;
    }
    if (m_cfg){
        delete m_cfg;
        m_cfg = NULL;
    }
    if (m_main){
        delete m_main;
        m_main = NULL;
    }
    if (m_view){
        delete m_view;
        m_view = NULL;
    }
    if (m_search){
        delete m_search;
        m_search = NULL;
    }
    if (m_manager){
        delete m_manager;
        m_manager = NULL;
    }
}

static char CLIENTS_CONF[] = "clients.conf";

void CorePlugin::loadDir()
{
    QString saveProfile = getProfile();
    setProfile(QString::null);
    bool bOK = false;
    QString baseName = user_file("");
    QDir dir(baseName);
    dir.setFilter(QDir::Dirs);
    QStringList list = dir.entryList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it){
        QString entry = *it;
        if (entry[0] == '.')
            continue;
        QString fname = baseName + "/" + entry + "/" + CLIENTS_CONF;
        QFile f(fname);
        if (f.exists()){
            m_profiles.append(entry);
            if (entry == saveProfile)
                bOK = true;
        }
    }
    if (bOK)
        setProfile(saveProfile);
}

static char BACKUP_SUFFIX[] = "~";
QString CorePlugin::getConfig()
{
    QString unread_str;
    for (list<msg_id>::iterator itUnread = unread.begin(); itUnread != unread.end(); ++itUnread){
        msg_id &m = (*itUnread);
        if (!unread_str.isEmpty())
            unread_str += ";";
        unread_str += QString::number(m.contact);
        unread_str += ",";
        unread_str += QString::number(m.id);
        unread_str += ",";
        unread_str += m.client;
    }
    setUnread(unread_str);

    unsigned editBgColor = getEditBackground();
    unsigned editFgColor = getEditForeground();

    QPalette pal = QApplication::palette();
    QColorGroup cg = pal.normal();
    if (((cg.color(QColorGroup::Base).rgb() & 0xFFFFFF) == getEditBackground()) &&
            ((cg.color(QColorGroup::Text).rgb() & 0xFFFFFF) == getEditForeground())){
        setEditBackground(0);
        setEditForeground(0);
    }

    QString ef     = FontEdit::font2str(editFont, false);
    QString def_ef = FontEdit::font2str(QApplication::font(), false);
    setEditFont(ef);
    if ((ef == def_ef) || !getEditSaveFont())
        setEditFont(QString::null);

    clearContainer();
    QString containers;

    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it(*list);
    QWidget * w;
    while ((w = it.current()) != NULL){
        if (w->inherits("Container")){
            Container *c = static_cast<Container*>(w);
            if (c->isReceived()){
                ++it;
                continue;
            }
            if (!containers.isEmpty())
                containers += ',';
            containers += QString::number(c->getId());
            setContainer(c->getId(), c->getState());
        }
        ++it;
    }
    delete list;
    setContainers(containers);
    if (m_main){
        saveGeometry(m_main, data.geometry);
        saveToolbar(m_main->bar, data.toolBarState);
    }
    QString cfg = save_data(coreData, &data);
    QString saveProfile = getProfile();
    setProfile(QString::null);
    QString cfgName = user_file("plugins.conf");
    QFile fCFG(cfgName + BACKUP_SUFFIX); // use backup file for this ...
    if (!fCFG.open(IO_WriteOnly | IO_Truncate)){
        log(L_ERROR, "Can't create %s", (const char*)cfgName.local8Bit());
    }else{
		QTextStream ts(&fCFG);
        ts << "[_core]\n"
           << "enable,"
           << m_base
           << "\n"
           << cfg;

        const int status = fCFG.status();
        const QString errorMessage = fCFG.errorString();
        fCFG.close();
        if (status != IO_Ok) {
            log(L_ERROR, QString("IO error during writting to file %1 : %2").arg(fCFG.name()).arg(errorMessage));
        } else {
            // rename to normal file
            QFileInfo fileInfo(fCFG.name());
            QString desiredFileName = fileInfo.fileName();
            desiredFileName = desiredFileName.left(desiredFileName.length() - strlen(BACKUP_SUFFIX));
#ifdef WIN32
            fileInfo.dir().remove(desiredFileName);
#endif
            if (!fileInfo.dir().rename(fileInfo.fileName(), desiredFileName)) {
                log(L_ERROR, QString("Can't rename file %1 to %2").arg(fileInfo.fileName()).arg(desiredFileName));
            }
        }
    }

    setProfile(saveProfile);
    cfgName = user_file(CLIENTS_CONF);
    QFile f(cfgName + BACKUP_SUFFIX); // use backup file for this ...
    if (!f.open(IO_WriteOnly | IO_Truncate)){
        log(L_ERROR, "Can't create %s", (const char*)cfgName.local8Bit());
    }else{
		QTextStream ts(&f);
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            Client *client = getContacts()->getClient(i);
            Protocol *protocol = client->protocol();
            pluginInfo *info = NULL;
            for (unsigned long n = 0;; n++){
                Event e(EventPluginGetInfo, (void*)n);
                info = (pluginInfo*)e.process();
                if (info == NULL)
                    break;
                if ((info->info == NULL) || !(info->info->flags & (PLUGIN_PROTOCOL & ~PLUGIN_NOLOAD_DEFAULT)))
                    continue;
                if (info->plugin == protocol->plugin())
                    break;
            }
            if (info == NULL)
                continue;
            ts << "["
               << info->name
               << "/"
               << protocol->description()->text
               << "]\n";
            QString line = client->getConfig();
            if (line.length()){
				ts << line << "\n";
            }
        }

        const int status = f.status();
        const QString errorMessage = f.errorString();
        f.close();
        if (status != IO_Ok) {
            log(L_ERROR, QString("IO error during writting to file %1 : %2").arg(f.name()).arg(errorMessage));
        } else {
            // rename to normal file
            QFileInfo fileInfo(f.name());
            QString desiredFileName = fileInfo.fileName();
            desiredFileName = desiredFileName.left(desiredFileName.length() - strlen(BACKUP_SUFFIX));
#ifdef WIN32
            fileInfo.dir().remove(desiredFileName);
#endif
            if (!fileInfo.dir().rename(fileInfo.fileName(), desiredFileName)) {
                log(L_ERROR, QString("Can't rename file %1 to %2").arg(fileInfo.fileName()).arg(desiredFileName));
            }
        }
    }

#ifndef WIN32
    QString dir = user_file("");
    chmod(QFile::encodeName(dir),S_IRUSR | S_IWUSR | S_IXUSR);
#endif
    QString res = save_data(coreData, &data);
    setEditBackground(editBgColor);
    setEditForeground(editFgColor);
    return res;
}

void CorePlugin::loadUnread()
{
    unread.clear();
    QString unread_str = getUnread();
    while (!unread_str.isEmpty()){
        QString item = getToken(unread_str, ';');
        unsigned long contact = getToken(item, ',').toULong();
        unsigned long id = getToken(item, ',').toULong();
        Message *msg = History::load(id, item, contact);
        if (msg == NULL)
            continue;
        msg_id m;
        m.id = id;
        m.contact = contact;
        m.client  = item;
        m.type    = msg->baseType();
        unread.push_back(m);
    }
    setUnread(QString::null);
}

void CorePlugin::clearUnread(unsigned contact_id)
{
    for (list<msg_id>::iterator it = unread.begin(); it != unread.end();){
        if ((*it).contact != contact_id){
            ++it;
            continue;
        }
        unread.erase(it);
        it = unread.begin();
    }
}

Message *CorePlugin::createMessage(const QString &type, ConfigBuffer *cfg)
{
    MAP_TYPES::iterator itt = types.find(type);
    if (itt != types.end()){
        CommandDef *def = messageTypes.find((*itt).second);
        if (def){
            MessageDef *mdef = (MessageDef*)(def->param);
            if (mdef->create){
                Message *msg = (mdef->create)(cfg);
                if (msg)
                    return msg;
            }
        }
    }
    return new Message(MessageGeneric, cfg);
}

void CorePlugin::loadClients(ClientList &clients)
{
    QString cfgName = user_file(CLIENTS_CONF);
    QFile f(cfgName);
    if (!f.open(IO_ReadOnly)){
        log(L_ERROR, "Can't open %s", cfgName.local8Bit().data());
        return;
    }
    ConfigBuffer cfg(&f);
    if (cfg.length() == 0){
        log(L_ERROR, "Can't read %s", cfgName.local8Bit().data());
        return;
    }
    for (;;){
        QString section = cfg.getSection();
        if (section.isEmpty())
            break;
        Client *client = loadClient(section, &cfg);
        if (client)
            clients.push_back(client);
    }
}

Client *CorePlugin::loadClient(const QString &name, ConfigBuffer *cfg)
{
    if (name.isEmpty())
        return NULL;
    QString clientName = name.section('/',1,1);
    QString pluginName = name.section('/',0,0);
    if (pluginName.isEmpty())
        return NULL;
    Event e(EventGetPluginInfo, (void*)pluginName.latin1());
    pluginInfo *info = (pluginInfo*)e.process();
    if (info == NULL){
        log(L_WARN, "Plugin %s not found", pluginName.latin1());
        return NULL;
    }
    if (info->info == NULL){
        Event e(EventLoadPlugin, &pluginName);
        e.process();
    }
    if ((info->info == NULL) || !(info->info->flags & (PLUGIN_PROTOCOL & ~PLUGIN_NOLOAD_DEFAULT))){
        log(L_DEBUG, "Plugin %s no protocol", pluginName.latin1());
        return NULL;
    }
    info->bDisabled = false;
    Event eApply(EventApplyPlugin, &pluginName);
    eApply.process();
    Protocol *protocol;
    ContactList::ProtocolIterator it;
    while ((protocol = ++it) != NULL){
        if (protocol->description()->text == clientName)
            return protocol->createClient(cfg);
    }
    log(L_DEBUG, "Protocol %s not found", clientName.latin1());
    return NULL;
}

bool CorePlugin::adjustClientItem(unsigned id, CommandDef *cmd)
{
    unsigned n = id - CmdClient;
    if (n >= getContacts()->nClients())
        return false;
    Client *client = getContacts()->getClient(n);
    Protocol *protocol = client->protocol();
    const CommandDef *descr = protocol->description();
    cmd->icon		= descr->icon;
    cmd->text_wrk   = clientName(client);
    return true;
}

void CorePlugin::managerFinished()
{
    QTimer::singleShot(0, this, SLOT(destroyManager()));
}

void CorePlugin::destroyManager()
{
    if (m_manager){
        delete m_manager;
        m_manager = NULL;
    }
}

QString CorePlugin::typeName(const QString &name)
{
    QString text = name;
    int n = text.find("&", 0);
    if (n >= 0)
        text.replace(n, 1, "");
    if (!text.length())
        log(L_DEBUG,"defText is empty!");
    return text;
}

void CorePlugin::loadMenu()
{
    Event eMenuRemove(EventMenuRemove, (void*)MenuConnections);
    eMenuRemove.process();

    unsigned nClients = getContacts()->nClients();

    Event eMenu(EventMenuCreate, (void*)MenuConnections);
    eMenu.process();

    Command cmd;
    cmd->id          = CmdCM;
    cmd->text        = I18N_NOOP("Connection manager");
    cmd->menu_id     = MenuConnections;
    cmd->menu_grp    = 0x8000;

    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

    cmd->id			= CmdShowPanel;
    cmd->text		= I18N_NOOP("Show status panel");
    cmd->menu_grp	= 0x8001;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    if (nClients >= 2){
        cmd->id          = CmdConnections;
        cmd->text        = I18N_NOOP("Connections");
        cmd->menu_id     = MenuMain;
        cmd->menu_grp    = 0x8040;
        cmd->popup_id    = MenuConnections;
        cmd->flags		 = COMMAND_DEFAULT;
    }else{
        cmd->id			= CmdConnections;
        cmd->text		= I18N_NOOP("Connection manager");
        cmd->menu_grp	= 0x8040;
        cmd->menu_id	= MenuMain;
        cmd->flags		= COMMAND_DEFAULT;
    }
    eCmd.process();

    cmd->id			= CmdChange;
    cmd->text		= I18N_NOOP("Change profile");
    cmd->menu_grp	= 0x10040;
    cmd->popup_id	= 0;
    eCmd.process();

    if (m_status == NULL)
        m_status = new CommonStatus;

    for (unsigned i = 0; i < m_nClients; i++){
        Event e(EventMenuRemove, (void*)(CmdClient + i));
        e.process();
    }
    for (m_nClients = 0; m_nClients < getContacts()->nClients(); m_nClients++){
        unsigned long menu_id = CmdClient + m_nClients;
        Event e(EventMenuCreate, (void*)menu_id);
        e.process();
        Client *client = getContacts()->getClient(m_nClients);
        Protocol *protocol = client->protocol();
        const CommandDef *cmd = protocol->statusList();
        if (cmd){
            Command c;
            c->id       = CmdTitle;
            c->text     = "_";
            c->menu_id  = menu_id;
            c->menu_grp = 0x0001;
            c->flags    = COMMAND_CHECK_STATE | COMMAND_TITLE;
            Event eCmd(EventCommandCreate, c);
            eCmd.process();
            c->id		= CmdCommonStatus;
            c->text		= I18N_NOOP("Common status");
            c->menu_id	= menu_id;
            c->menu_grp = 0x3000;
            c->flags	= COMMAND_CHECK_STATE;
            eCmd.process();
            c->id		= CmdSetup;
            c->text		= I18N_NOOP("Configure client");
            c->icon		= "configure";
            c->menu_id	= menu_id;
            c->menu_grp = 0x3001;
            c->flags	= COMMAND_DEFAULT;
            eCmd.process();
            c->id		= menu_id;
            c->text		= "_";
            c->icon		= QString::null;
            c->menu_id	= MenuConnections;
            c->menu_grp	= 0x1000 + menu_id;
            c->popup_id	= menu_id;
            c->flags	= COMMAND_CHECK_STATE;
            eCmd.process();
            unsigned id = 0x100;
//            for (; cmd->id; cmd++){
            for (; !cmd->text.isEmpty(); cmd++){
                c = *cmd;
                c->menu_id = menu_id;
                c->menu_grp = id++;
                c->flags = COMMAND_CHECK_STATE;
                eCmd.process();
            }
            if (protocol->description()->flags & PROTOCOL_INVISIBLE){
                c->id		= CmdInvisible;
                c->text		= I18N_NOOP("&Invisible");
                c->icon		= protocol->description()->icon_on;
                c->menu_grp	= 0x1000;
                c->flags	= COMMAND_CHECK_STATE;
                eCmd.process();
            }
        }
    }
    showPanel();
}

void CorePlugin::showPanel()
{
    if (m_main == NULL)
        return;
    bool bShow = getShowPanel();
    if (bShow){
        if (getContacts()->nClients() < 2)
            bShow = false;
    }
    if (bShow){
        if (m_statusWnd == NULL)
            m_statusWnd = new StatusWnd;
        m_statusWnd->show();
        return;
    }
    if (m_statusWnd){
        delete m_statusWnd;
        m_statusWnd = NULL;
    }
}

ClientList::ClientList()
{
}

ClientList::~ClientList()
{
    CorePlugin::m_plugin->m_bIgnoreEvents = true;
    for (ClientList::iterator it = begin(); it != end(); ++it)
        delete *it;
    CorePlugin::m_plugin->m_bIgnoreEvents = false;
}

void ClientList::addToContacts()
{
    for (ClientList::iterator it = begin(); it != end(); ++it)
        getContacts()->addClient(*it);
    clear();
}

unsigned CorePlugin::getContainerMode()
{
    return data.ContainerMode.toULong();
}

void CorePlugin::setContainerMode(unsigned value)
{
    if (value == data.ContainerMode.toULong())
        return;
    data.ContainerMode.asULong() = value;
    emit modeChanged();
}

QString CorePlugin::clientName(Client *client)
{
    QString s = client->name();
    QString res = i18n(getToken(s, '.'));
    res += " ";
    return res + s;
}

void CorePlugin::checkHistory()
{
    Contact *contact;
    ContactList::ContactIterator it;
    while ((contact = ++it) != NULL){
        HistoryUserData *data = (HistoryUserData*)(contact->getUserData(history_data_id));
        if ((data == NULL) || !data->CutDays.toBool())
            continue;
        time_t now = time(NULL);
        now -= data->Days.toULong() * 24 * 60 * 60;
        History::cut(NULL, contact->id(), now);
    }
    QTimer::singleShot(24 * 60 * 60 * 1000, this, SLOT(checkHistory()));
}

void CorePlugin::setManualStatus(unsigned long status)
{
    if (status == getManualStatus())
        return;
    setStatusTime(time(NULL));
    data.ManualStatus.asULong() = status;
}

void CorePlugin::alertFinished()
{
    if (m_alert)
        setNoJoinAlert(m_alert->isChecked());
    m_alert = NULL;
}

void CorePlugin::focusDestroyed()
{
    m_focus = NULL;
}

bool CorePlugin::lockProfile(const QString &profile, bool bSend)
{
    if (profile.isEmpty()){
        if (m_lock){
            delete m_lock;
            m_lock = NULL;
        }
        return true;
    }
    FileLock *lock = new FileLock(user_file(".lock"));
    if (!lock->lock(bSend)){
        delete lock;
        return false;
    }
    if (m_lock)
        delete m_lock;
    m_lock = lock;
    return true;
}

void CorePlugin::showMain()
{
    if (m_main){
        m_main->show();
        raiseWindow(m_main);
    }
}

#ifdef WIN32

LockThread::LockThread(HANDLE _hEvent)
{
    hEvent = _hEvent;
}

void LockThread::run()
{
    for (;;){
        DWORD res = WaitForSingleObject(hEvent, INFINITE);
        if (res == WAIT_ABANDONED)
            break;
        QTimer::singleShot(0, CorePlugin::m_plugin, SLOT(showMain()));
    }
}

// From zlib
// Copyright (C) 1995-2002 Mark Adler

#define BASE 65521L
#define NMAX 5552

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

unsigned adler32(const char *buf, unsigned len)
{
    unsigned long s1 = 0;
    unsigned long s2 = 0;
    int k;
    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
            buf += 16;
            k -= 16;
        }
        if (k != 0) do {
                s1 += *buf++;
                s2 += s1;
            } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

#endif

FileLock::FileLock(const QString &name)
        : QFile(name)
{
#ifdef WIN32
    m_thread = NULL;
#else
m_bLock  = false;
#endif
}

FileLock::~FileLock()
{
#ifdef WIN32
    if (m_thread){
        CloseHandle(m_thread->hEvent);
        m_thread->wait(1000);
        m_thread->terminate();
        delete m_thread;
    }
#else
    close();
    if (m_bLock)
        QFile::remove(name());
#endif
}

#ifdef WIN32
bool FileLock::lock(bool bSend)
{
    QString event = "SIM.";
    QString s = name();
    event += QString::number(adler32(s.latin1(), s.length()));
    HANDLE hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, event.latin1());
    if (hEvent){
        if (bSend)
            SetEvent(hEvent);
        CloseHandle(hEvent);
        return false;
    }
    hEvent = CreateEventA(NULL, false, false, event.latin1());
    if (hEvent == NULL)
        return false;
    m_thread = new LockThread(hEvent);
    m_thread->start();
#else
bool FileLock::lock(bool)
{
    if (!open(IO_ReadWrite | IO_Truncate)){
        log(L_WARN, "Can't create %s", name().local8Bit().data());
        return false;
    }
    struct flock fl;
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 1;
    if (fcntl(handle(), F_SETLK, &fl) == -1){
        //QFile::remove(name());
        return false;
    }
    m_bLock = true;
#endif
    return true;
}

void HistoryThread::run()
{
    QString str = user_file(".history_file");
    History::save(m_id, str);
    QProcess *m_ex;
    m_ex = new QProcess();
    m_ex->addArgument(m_Viewer);
    m_ex->addArgument(str);
    m_ex->start();
}

#ifndef NO_MOC_INCLUDES
#include "core.moc"
#endif



