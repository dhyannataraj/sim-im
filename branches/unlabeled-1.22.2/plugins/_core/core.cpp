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
#include "exec.h"
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
#include "icons.h"
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

#include <qtimer.h>
#include <qapplication.h>
#include <qwidgetlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qpopupmenu.h>

#include <time.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <list>
using namespace std;

Plugin *createCorePlugin(unsigned base, bool, const char *config)
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
    SIMTranslator(QObject *parent, const QString & filename);
    ~SIMTranslator();
    virtual QTranslatorMessage findMessage(const char* context,
                                           const char *sourceText,
                                           const char* message) const;
    void load ( const QString & filename);
protected:
    string fName;
    loaded_l10nfile domain;
};

SIMTranslator::SIMTranslator(QObject *parent, const QString & filename)
        : QTranslator(parent, "simtranslator")
{
    load(filename);
}

SIMTranslator::~SIMTranslator()
{
    if (domain.data)
        k_nl_unload_domain((struct loaded_domain *)domain.data);
}

void SIMTranslator::load ( const QString & filename)
{
    fName = QFile::encodeName(filename);
    domain.filename = (char*)(fName.c_str());
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
        string s;
        s = "_: ";
        s += message;
        s += "\n";
        s += sourceText;
        text = k_nl_find_msg((struct loaded_l10nfile*)&domain, s.c_str());
    }
    if ((text == NULL) && context && *context && message && *message){
        string s;
        s = "_: ";
        s += context;
        s += "\n";
        s += message;
        text = k_nl_find_msg((struct loaded_l10nfile*)&domain, s.c_str());
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
    bool		SavePasswd;
    bool		NoShow;
    bool		ShowPanel;
    unsigned	ManualStatus;
    long		geometry[4];
    long		toolBarState[7];
    void		*Buttons;
    void		*Menues;
    bool		ShowOnLine;
    unsigned	GroupMode;
} CoreData;
*/
static DataDef coreData[] =
    {
        { "Profile", DATA_STRING, 1, 0 },
        { "SavePasswd", DATA_BOOL, 1, 1 },
        { "NoShow", DATA_BOOL, 1, 0 },
        { "ShowPanel", DATA_BOOL, 1, 1 },
        { "ManualStatus", DATA_ULONG, 1, STATUS_OFFLINE },
        { "Invisible", DATA_BOOL, 1, 0 },
        { "Geometry", DATA_LONG, 4, (unsigned)(-1) },
        { "ToolBar", DATA_LONG, 7, 0 },
        { "Buttons", DATA_STRLIST, 1, 0 },
        { "Menues", DATA_STRLIST, 1, 0 },
        { "ShowOnLine", DATA_BOOL, 1, 0 },
        { "GroupMode", DATA_ULONG, 1, 1 },
        { "UseDblClick", DATA_BOOL, 1, 0 },
        { "UseSysColors", DATA_BOOL, 1, 1 },
        { "ColorOnline", DATA_ULONG, 1, 0 },
        { "ColorOffline", DATA_ULONG, 1, 0 },
        { "ColorAway", DATA_ULONG, 1, 0 },
        { "ColorNA", DATA_ULONG, 1, 0 },
        { "ColorDND", DATA_ULONG, 1, 0 },
        { "ColorGroup", DATA_ULONG, 1, 0 },
        { "GroupSeparator", DATA_BOOL, 1, 1 },
        { "Lang", DATA_STRING, 1, 0 },
        { "ContainerMode", DATA_ULONG, 1, CONTAINER_GROUP },
        { "SendOnEnter", DATA_BOOL, 1, 0 },
        { "ContainerSize", DATA_ULONG, 2, 0 },
        { "ContainerBar", DATA_ULONG, 7, 0 },
        { "ContainerStatusSize", DATA_ULONG, 1, 0 },
        { "Containers", DATA_STRING, 1, 0 },
        { "Container", DATA_STRLIST, 1, 0 },
        { "CopyMessages", DATA_ULONG, 1, 3 },
        { "EditHeight", DATA_ULONG, 1, 100 },
        { "EditBar", DATA_ULONG, 7, 0 },
        { "EditForeground", DATA_ULONG, 1, 0xffffff },
        { "EditBackground", DATA_ULONG, 1, 0x000000 },
        { "EditFont", DATA_STRING, 1, 0 },
        { "OwnColors", DATA_BOOL, 1, 0 },
        { "ColorSend", DATA_ULONG, 1, 0x000000 },
        { "ColorReceive", DATA_ULONG, 1, 0x000000 },
        { "ColorSender", DATA_ULONG, 1, 0x000080 },
        { "ColorReceiver", DATA_ULONG, 1, 0x800000 },
        { "UseSmiles", DATA_BOOL, 1, 1 },
        { "CloseSend", DATA_BOOL, 1, 0 },
        { "HistoryPage", DATA_ULONG, 1, 100 },
        { "HistoryDirection", DATA_BOOL, 1, 0 },
        { "HistorySize", DATA_ULONG, 2, 0 },
        { "HistoryBar", DATA_ULONG, 7, 0 },
        { "HistorySearch", DATA_UTF, 1, 0 },
        { "Unread", DATA_STRING, 1, 0 },
        { "NoShowAutoReply", DATA_STRLIST, 1, 0 },
        { "SortMode", DATA_ULONG, 1, 0x00030201 },
        { "CloseTransfer", DATA_BOOL, 1, 0 },
        { "SystemFonts", DATA_BOOL, 1, 1 },
        { "BaseFont", DATA_STRING, 1, 0 },
        { "MenuFont", DATA_STRING, 1, 0 },
        { NULL, 0, 0, 0 }
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
        { "LogMessage", DATA_BOOL, 1, 1 },
        { "OpenOnReceive", DATA_BOOL, 1, 0 },
        { "OpenOnOnline", DATA_BOOL, 1, 0 },
        { "IncomingPath", DATA_UTF, 1, (unsigned)"Incoming Files" },
        { "AcceptMode", DATA_ULONG, 1, 0 },
        { "OverwriteFiles", DATA_BOOL, 1, 0 },
        { "DeclineMessage", DATA_UTF, 1, 0 },
        { NULL, 0, 0, 0 }
    };

static DataDef smsUserData[] =
    {
        { "SMSTranslit", DATA_BOOL, 1, 0 },
        { "SMSSignatureBefore", DATA_UTF, 1, 0 },
        { "SMSSignatureAfter", DATA_UTF, 1, (unsigned)"\n&MyName;" },
        { NULL, 0, 0, 0 }
    };

static DataDef arUserData[] =
    {
        { "AutoReply", DATA_UTFLIST, 1, 0 },
        { NULL, 0, 0, 0 }
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
        { "OfflineOpen", DATA_BOOL, 1, 1 },
        { "OnlineOpen", DATA_BOOL, 1, 1 },
        { "ShowAlways", DATA_BOOL, 1, 0 },
        { NULL, 0, 0, 0 }
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

typedef struct autoReply
{
    unsigned	status;
    const char	*text;
} autoReply;

static autoReply autoReplies[] =
    {
        { STATUS_AWAY, I18N_NOOP(
              "I am currently away from ICQ.\n"
              "Please leave your message and I will get back to you as soon as I return!\n"
          ) },
        { STATUS_NA, I18N_NOOP(
              "I am out'a here.\n"
              "See you tomorrow!\n"
          ) },
        { STATUS_DND, I18N_NOOP(
              "Please do not disturb me now. Disturb me later.\n"
          ) },
        { STATUS_FFC, I18N_NOOP(
              "We'd love to hear what you have to say. Join our chat.\n"
          ) },
        { 0, NULL }
    };

CorePlugin::CorePlugin(unsigned base, const char *config)
        : Plugin(base), EventReceiver(HighPriority)
{
    m_plugin = this;
    m_saveBaseFont = NULL;
    m_saveMenuFont = NULL;

    load_data(coreData, &data, config);

    user_data_id = getContacts()->registerUserData("core", coreUserData);
    sms_data_id  = getContacts()->registerUserData("sms", smsUserData);
    ar_data_id   = getContacts()->registerUserData("ar", arUserData);
    list_data_id   = getContacts()->registerUserData("list", listUserData);

    m_translator = NULL;
    m_statusWnd  = NULL;
    m_status	 = NULL;
    m_main		 = NULL;
    m_cfg		 = NULL;
    m_search	 = NULL;
    m_view		 = NULL;
    m_manager	 = NULL;
    m_bInit		 = false;
    m_nClients	 = 0;
    m_nClientsMenu = 0;

    loadDir();

    m_tmpl	= new Tmpl(this);
    m_icons = new Icons;
    m_exec	= new ExecManager;
    m_cmds	= new Commands;
    boundTypes();

    Event eToolbar(EventToolbarCreate, (void*)ToolBarMain);
    eToolbar.process();
    Event eToolbarContainer(EventToolbarCreate, (void*)ToolBarContainer);
    eToolbarContainer.process();
    Event eToolbarEdit(EventToolbarCreate, (void*)ToolBarEdit);
    eToolbarEdit.process();
    Event eToolbarHistory(EventToolbarCreate, (void*)BarHistory);
    eToolbarHistory.process();
    Event eToolbarReceived(EventToolbarCreate, (void*)BarReceived);
    eToolbarReceived.process();
    Event eMenu(EventMenuCreate, (void*)MenuMain);
    eMenu.process();
    Event eMenuPhones(EventMenuCreate, (void*)MenuPhones);
    eMenuPhones.process();
    Event eMenuLocation(EventMenuCreate, (void*)MenuLocation);
    eMenuLocation.process();
    Event eMenuPhoneState(EventMenuCreate, (void*)MenuPhoneState);
    eMenuPhoneState.process();

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
    cmd->icon		= NULL;
    cmd->menu_id	= MenuContainerContact;
    cmd->menu_grp	= 0x1000;
    cmd->accel		= NULL;
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
    cmd->bar_id		= ToolBarEdit;
    cmd->bar_grp	= 0x8000;
    cmd->flags		= BTN_PICT;
    eCmd.process();

    cmd->id			= CmdSmile;
    cmd->text		= I18N_NOOP("&Insert smile");
    cmd->icon		= "smile0";
    cmd->bar_grp	= 0x7000;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdSendClose;
    cmd->text		= I18N_NOOP("C&lose after send");
    cmd->icon		= "fileclose";
    cmd->icon_on	= "fileclose";
    cmd->bar_grp	= 0x7010;
    cmd->flags		= COMMAND_DEFAULT;
    if (getCloseSend())
        cmd->flags |= COMMAND_CHECKED;
    eCmd.process();

    cmd->id			= CmdMultiply;
    cmd->text		= I18N_NOOP("&Multiply send");
    cmd->icon		= "1rightarrow";
    cmd->icon_on	= "1leftarrow";
    cmd->bar_grp	= 0xF010;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdCustomInput;
    cmd->text		= I18N_NOOP("Input");
    cmd->icon		= "empty";
    cmd->icon_on	= NULL;
    cmd->bar_grp	= 0x1000;
    cmd->flags		= BTN_CUSTOM;
    eCmd.process();

    cmd->id			= CmdCustomReceive;
    cmd->text		= I18N_NOOP("Receive");
    cmd->icon		= "empty";
    cmd->icon_on	= NULL;
    cmd->bar_grp	= 0x1001;
    cmd->flags		= BTN_CUSTOM;
    eCmd.process();

    cmd->bar_id		= BarReceived;
    eCmd.process();

    cmd->id			= CmdNextMessage;
    cmd->text		= I18N_NOOP("&Next");
    cmd->icon		= "message";
    cmd->bar_grp	= 0x8000;
    cmd->flags		= BTN_PICT;
    eCmd.process();

    cmd->id			= CmdMsgAnswer;
    cmd->text		= I18N_NOOP("&Answer");
    cmd->icon		= "mail_generic";
    cmd->bar_grp	= 0x1000;
    cmd->flags		= BTN_PICT;
    eCmd.process();

    cmd->id			= CmdHistoryDirection;
    cmd->text		= I18N_NOOP("&Direction");
    cmd->icon		= "1uparrow";
    cmd->icon_on	= "1downarrow";
    cmd->bar_id		= BarHistory;
    cmd->bar_grp	= 0x2000;
    cmd->flags		= COMMAND_DEFAULT;
    if (getHistoryDirection())
        cmd->flags = COMMAND_CHECKED;
    eCmd.process();

    cmd->id			= CmdHistoryFind;
    cmd->text		= I18N_NOOP("Search:");
    cmd->icon		= "search";
    cmd->icon_on	= NULL;
    cmd->bar_grp	= 0x3000;
    cmd->flags		= BTN_CUSTOM;
    eCmd.process();

    cmd->id			= CmdHistoryPrev;
    cmd->text		= I18N_NOOP("&Previous page");
    cmd->icon		= "1leftarrow";
    cmd->bar_grp	= 0x5000;
    cmd->flags		= COMMAND_DEFAULT;
    eCmd.process();

    cmd->id			= CmdHistoryNext;
    cmd->text		= I18N_NOOP("&Next page");
    cmd->icon		= "1rightarrow";
    cmd->bar_grp	= 0x5001;
    cmd->flags		= COMMAND_DEFAULT;
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
    cmd->menu_id	= MenuMsgView;
    cmd->menu_grp	= 0x1000;
    cmd->bar_id		= 0;
    cmd->bar_grp	= 0;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdMsgSpecial;
    cmd->text		= "_";
    cmd->icon		= NULL;
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdCopy;
    cmd->text		= I18N_NOOP("&Copy");
    cmd->icon		= "editcopy";
    cmd->menu_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdMsgQuote;
    cmd->text		= I18N_NOOP("&Quote");
    cmd->icon		= NULL;
    cmd->menu_id	= MenuMsgCommand;
    cmd->menu_grp	= 0x1002;
    eCmd.process();

    cmd->id			= CmdMsgForward;
    cmd->text		= I18N_NOOP("&Forward");
    cmd->menu_grp	= 0x1003;
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
    eCmd.process();

    cmd->id			= CmdRedo;
    cmd->text		= I18N_NOOP("&Redo");
    cmd->accel		= "Ctrl+Y";
    cmd->icon		= "redo";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x1001;
    eCmd.process();

    cmd->id			= CmdCut;
    cmd->text		= I18N_NOOP("Cu&t");
    cmd->icon		= "editcut";
    cmd->accel		= "Ctrl+X";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x2000;
    eCmd.process();

    cmd->id			= CmdCopy;
    cmd->text		= I18N_NOOP("&Copy");
    cmd->icon		= "editcopy";
    cmd->accel		= "Ctrl+C";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x2001;
    eCmd.process();

    cmd->id			= CmdPaste;
    cmd->text		= I18N_NOOP("&Paste");
    cmd->icon		= "editpaste";
    cmd->accel		= "Ctrl+V";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x2002;
    eCmd.process();

    cmd->id			= CmdClear;
    cmd->text		= I18N_NOOP("Clear");
    cmd->icon		= NULL;
    cmd->accel		= NULL;
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x3000;
    eCmd.process();

    cmd->id			= CmdSelectAll;
    cmd->text		= I18N_NOOP("Select All");
    cmd->accel		= "Ctrl+A";
    cmd->menu_id	= MenuTextEdit;
    cmd->menu_grp	= 0x3001;
    eCmd.process();

    cmd->id			= user_data_id + 1;
    cmd->text		= I18N_NOOP("&Messages");
    cmd->accel		= NULL;
    cmd->icon		= "message";
    cmd->icon_on	= NULL;
    cmd->param		= (void*)getInterfaceSetup;
    Event ePrefMsg(EventAddPreferences, cmd);
    ePrefMsg.process();

    cmd->id			= sms_data_id + 1;
    cmd->text		= I18N_NOOP("&SMS");
    cmd->icon		= "cell";
    cmd->icon_on	= NULL;
    cmd->param		= (void*)getSMSSetup;
    Event ePrefSMS(EventAddPreferences, cmd);
    ePrefSMS.process();

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
    cmd->text		= I18N_NOOP("&No show groups");
    cmd->icon		= "grp_off";
    cmd->icon_on	= 0;
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

    cmd->id			= CmdGrpCreate;
    cmd->text		= I18N_NOOP("&Create group");
    cmd->icon		= "grp_create";
    cmd->icon_on	= NULL;
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
    cmd->accel		= NULL;
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
    cmd->icon		= NULL;
    cmd->accel		= NULL;
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
    cmd->icon		= NULL;
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
    cmd->icon		= NULL;
    cmd->menu_grp	= 0x8003;
    cmd->accel		= NULL;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id			= CmdInfo;
    cmd->text		= I18N_NOOP("User &info");
    cmd->icon		= "info";
    cmd->menu_grp	= 0x7010;
    cmd->accel		= NULL;
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
    cmd->icon		= NULL;
    cmd->popup_id	= MenuContainer;
    cmd->menu_grp	= 0x8010;
    cmd->accel		= NULL;
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
    cmd->icon		= NULL;
    cmd->accel		= NULL;
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
    cmd->icon		= NULL;
    cmd->menu_id	= MenuLocation;
    cmd->menu_grp   = 0x1000;
    cmd->flags		= COMMAND_CHECK_STATE;
    eCmd.process();
}

void CorePlugin::initData()
{
    if ((getEditBackground() == 0) && (getEditForeground() == 0)){
        QPalette pal = QApplication::palette();
        QColorGroup cg = pal.normal();
        setEditBackground(cg.color(QColorGroup::Base).rgb() & 0xFFFFFF);
        setEditForeground(cg.color(QColorGroup::Text).rgb() & 0xFFFFFF);
    }
    editFont = FontEdit::str2font(getEditFont(), QApplication::font());
    setAutoReplies();
    setFonts();
}

void CorePlugin::setAutoReplies()
{
    ARUserData *data = (ARUserData*)getContacts()->getUserData(ar_data_id);
    for (autoReply *a = autoReplies; a->text; a++){
        const char *t = get_str(data->AutoReply, a->status);
        if ((t == NULL) || (*t == 0))
            set_str(&data->AutoReply, a->status, i18n(a->text).utf8());
    }
}

CorePlugin::~CorePlugin()
{
    destroy();
    delete m_cmds;
    delete m_exec;
    delete m_icons;
    delete m_tmpl;
    if (m_status)
        delete m_status;
    if (m_saveBaseFont)
        delete m_saveBaseFont;
    if (m_saveMenuFont)
        delete m_saveMenuFont;

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
    string s = "po";
    s += "\\";
    for (const char *pp = lang; *pp; pp++)
        s += tolower(*pp);
    s += ".qm";
    s = app_file(s.c_str());
    QFile f(QFile::decodeName(s.c_str()));
    if (!f.exists()) return "";
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
    string lang = getLang();
    if (!strcmp(lang.c_str(), "-"))
        return;
    if (lang.length() == 0){
#ifdef WIN32
        char buff[256];
        int res = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, buff, sizeof(buff));
        if (res){
            lang += tolower(buff[0]);
            lang += tolower(buff[1]);
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
    QString po = poFile(lang.c_str());
    if (po.isEmpty())
        return;
#if !defined(USE_KDE) && (QT_VERSION >= 300)
    m_translator = new SIMTranslator(NULL, po);
#else
    m_translator = new QTranslator(NULL);
    m_translator->load(po);
#endif
    qApp->installTranslator(m_translator);
#if !defined(USE_KDE) || (QT_VERSION < 300)
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
#if !defined(USE_KDE) || (QT_VERSION < 300)
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

void *CorePlugin::processEvent(Event *e)
{
    switch (e->type()){
    case EventARRequest:{
            ARRequest *r = (ARRequest*)(e->param());
            ARUserData *ar;
            const char *tmpl = NULL;
            if (r->contact){
                ar = (ARUserData*)(r->contact->userData.getUserData(ar_data_id, false));
                if (ar)
                    tmpl = get_str(ar->AutoReply, r->status);
                if ((tmpl == NULL) || (*tmpl == 0)){
                    ar = NULL;
                    Group *grp = getContacts()->group(r->contact->getGroup());
                    if (grp)
                        ar = (ARUserData*)(grp->userData.getUserData(ar_data_id, false));
                    if (ar)
                        tmpl = get_str(ar->AutoReply, r->status);
                }
            }
            if ((tmpl == NULL) || (*tmpl == 0)){
                ar = (ARUserData*)(getContacts()->getUserData(ar_data_id));
                tmpl = get_str(ar->AutoReply, r->status);
                if ((tmpl == NULL) || (*tmpl == 0))
                    tmpl = get_str(ar->AutoReply, STATUS_AWAY);
            }
            if (tmpl == NULL)
                tmpl = "";
            QString tstr = QString::fromUtf8(tmpl);
            TemplateExpand t;
            t.contact	= r->contact;
            t.param		= r->param;
            t.receiver	= r->receiver;
            t.tmpl		= tstr;;
            Event eTmpl(EventTemplateExpand, &t);
            eTmpl.process();
            return e->param();
        }
    case EventSaveState:{
            ARUserData *ar = (ARUserData*)getContacts()->getUserData(ar_data_id);
            for (autoReply *a = autoReplies; a->text; a++){
                const char *t = get_str(ar->AutoReply, a->status);
                if (t && !strcmp(t, i18n(a->text).utf8()))
                    set_str(&ar->AutoReply, a->status, NULL);
            }
            e->process(this);
            setAutoReplies();
            return this;
        }
    case EventPluginChanged:{
            pluginInfo *info = (pluginInfo*)(e->param());
            if (info->plugin == this){
                string profile = getProfile();
                free_data(coreData, &data);
                load_data(coreData, &data, info->config);
                if (info->config){
                    free(info->config);
                    info->config = NULL;
                }
                setProfile(profile.c_str());
                removeTranslator();
                installTranslator();
                initData();
            }
            break;
        }
    case EventInit:
        if (!m_bInit && !init(true))
            return (void*)ABORT_LOADING;
        return NULL;
    case EventQuit:
        destroy();
        m_cmds->clear();
        return NULL;
    case EventHomeDir:{
            string *cfg = (string*)(e->param());
            string profile = getProfile();
            if (profile.length()){
#ifdef WIN32
                profile += "\\";
#else
                profile += "/";
#endif
            }
            profile += cfg->c_str();
            *cfg = profile;
            Event eProfile(EventHomeDir, cfg);
            return eProfile.process(this);
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
            unsigned id = (unsigned)(e->param());
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
            messageTypes.add(cmd);
            string name = typeName(cmd->text);
            MAP_TYPES::iterator itt = types.find(name);
            if (itt == types.end()){
                types.insert(MAP_TYPES::value_type(name, cmd->id));
            }else{
                (*itt).second = cmd->id;
            }
            return e->param();
        }
    case EventRemoveMessageType:{
            unsigned id = (unsigned)(e->param());
            for (MAP_TYPES::iterator itt = types.begin(); itt != types.end(); ++itt){
                if ((*itt).second == id){
                    types.erase(itt);
                    break;
                }
            }
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
            Contact *contact = (Contact*)(e->param());
            if (contact->getIgnore())
                clearUnread(contact->id());
            return NULL;
        }
    case EventMessageAcked:{
            Message *msg = (Message*)(e->param());
            unsigned type = msg->type();
            for (;;){
                CommandDef *def = messageTypes.find(type);
                if (def == NULL)
                    break;
                MessageDef *mdef = (MessageDef*)(def->param);
                if (mdef->base_type == 0)
                    break;
                type = mdef->base_type;
            }
            if (type == MessageFile){
                QWidget *w = new FileTransferDlg(static_cast<FileMessage*>(msg));
                raiseWindow(w);
            }
            return NULL;
        }
    case EventMessageReceived:{
            Message *msg = (Message*)(e->param());
            Contact *contact = getContacts()->contact(msg->contact());
            if (contact == NULL)
                return NULL;
            if (msg->type() == MessageStatus){
                CoreUserData *data = (CoreUserData*)(contact->getUserData(CorePlugin::m_plugin->user_data_id));
                if ((data == NULL) || (data->LogStatus == 0))
                    return NULL;
            }else{
                time_t now;
                time(&now);
                contact->setLastActive(now);
                Event e(EventContactStatus, contact);
                e.process();
            }
        }
    case EventSent:{
            Message *msg = (Message*)(e->param());
            CommandDef *def = messageTypes.find(msg->type());
            if (def){
                History::add(msg, typeName(def->text).c_str());
                if ((e->type() == EventMessageReceived) && (msg->type() != MessageStatus)){
                    msg_id m;
                    m.id = msg->id();
                    m.contact = msg->contact();
                    m.client = msg->client();
                    m.type = msg->type();
                    unread.push_back(m);
                    Contact *contact = getContacts()->contact(msg->contact());
                    if (contact && contact->getTemporary()){
                        contact->setTemporary(0);
                        Event e(EventContactChanged, contact);
                        e.process();
                    }
                    if (contact){
                        CoreUserData *data = (CoreUserData*)(contact->getUserData(user_data_id));
                        if (data->OpenOnReceive){
                            Event e(EventOpenMessage, msg);
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
            unsigned contact_id = (unsigned)(e->param());
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
            Message *msg = (Message*)(e->param());
            Contact *contact = getContacts()->contact(msg->contact());
            if (contact == NULL)
                return NULL;
            UserWnd		*userWnd	= NULL;
            Container	*container	= NULL;
            QWidgetList  *list = QApplication::topLevelWidgets();
            QWidgetListIt itw(*list);
            QWidget * w;
            while ((w = itw.current()) != NULL){
                if (w->inherits("Container")){
                    container =  static_cast<Container*>(w);
                    if (getContainerMode() == 0){
                        if (container->isReceived() != (msg->getFlags() & MESSAGE_RECEIVED)){
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
                userWnd = new UserWnd(contact->id(), NULL, msg->getFlags() & MESSAGE_RECEIVED);
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
                    if (container == NULL)
                        container = new Container(1);
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
                    if (container == NULL)
                        container = new Container(id);
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
                    if (getContainerMode() == 0)
                        container->setReceived(msg->getFlags() & MESSAGE_RECEIVED);
                }
                container->addUserWnd(userWnd);
            }else{
                container->raiseUserWnd(userWnd);
            }
            container->setNoSwitch();
            container->show();
            userWnd->setMessage(msg);
            raiseWindow(container);
            return e->param();
        }
    case EventContactOnline:{
            Contact *contact = getContacts()->contact((unsigned)(e->param()));
            if (contact){
                CoreUserData *data = (CoreUserData*)(contact->getUserData(user_data_id));
                if (data->OpenOnOnline){
                    Message msg(MessageGeneric);
                    Event e(EventOpenMessage, &msg);
                    e.process();
                }
            }
            return NULL;
        }
    case EventCheckState:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if (cmd->id == CmdContactClients){
                Contact *contact = getContacts()->contact((unsigned)(cmd->param));
                if (contact == NULL)
                    return NULL;
                if (cmd->menu_id == MenuMessage){
                    unsigned n = contact->clientData.size();
                    if (n <= 1)
                        return NULL;
                    void *data;
                    ClientDataIterator it(contact->clientData);
                    CommandDef *cmds = new CommandDef[n + 2];
                    memset(cmds, 0, sizeof(CommandDef) * (n + 2));
                    n = 1;
                    cmds[0].text = "_";
                    while ((data = ++it) != NULL){
                        unsigned id  = CmdContactClients + n;
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
                        const char *statusIcon = NULL;
                        it.client()->contactInfo(data, status, style, statusIcon);
                        cmds[n].icon = statusIcon;
                        QString t = it.client()->contactName(data);
                        cmds[n].text_wrk = strdup(t.utf8());
                        n++;
                    }
                    cmd->param = cmds;
                    cmd->flags |= COMMAND_RECURSIVE;
                    return e->param();
                }
                unsigned n = cmd->menu_id - CmdContactClients - 1;
                if (n >= contact->clientData.size())
                    return NULL;

                void *data;
                Client *client = NULL;
                ClientDataIterator itc(contact->clientData);
                while ((data = ++itc) != NULL){
                    if (n == 0){
                        client = itc.client();
                        break;
                    }
                    n--;
                }
                if (data == NULL)
                    return NULL;

                Event eMenu(EventGetMenuDef, (void*)MenuMessage);
                CommandsDef *cmdsMsg = (CommandsDef*)(eMenu.process());
                unsigned nCmds = 0;
                {
                    CommandsList it(*cmdsMsg, true);
                    while (++it)
                        nCmds++;
                }

                CommandDef *cmds = new CommandDef[nCmds];
                memset(cmds, 0, sizeof(CommandDef) * nCmds);
                nCmds = 0;

                CommandsList it(*cmdsMsg, true);
                CommandDef *c;
                while ((c = ++it) != NULL){
                    if (!client->canSend(c->id, data))
                        continue;
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
            if (cmd->menu_id == MenuContainer){
                Contact *contact = getContacts()->contact((unsigned)(cmd->param));
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
                    memset(cmds, 0, sizeof(CommandDef) * (nContainers + 1));
                    unsigned n = 0;
                    QWidgetListIt it1(*list);
                    while ((w = it1.current()) != NULL){
                        if (w->inherits("Container")){
                            Container *c = static_cast<Container*>(w);
                            cmds[n] = *cmd;
                            cmds[n].icon = NULL;
                            cmds[n].id = c->getId();
                            cmds[n].flags = COMMAND_DEFAULT;
                            cmds[n].text_wrk = strdup(c->name().utf8());
                            if (c->wnd(contact->id()))
                                cmds[n].flags |= COMMAND_CHECKED;
                            n++;
                        }
                        ++it1;
                    }
                    cmds[n].icon = NULL;
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
                Contact *contact = getContacts()->contact((unsigned)(cmd->param));
                if (contact){
                    void *data;
                    ClientDataIterator it(contact->clientData);
                    while ((data = ++it) != NULL){
                        if (it.client()->canSend(cmd->id, data))
                            return e->param();
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
                    if (p.isEmpty())
                        return NULL;
                    cmd->flags &= ~COMMAND_CHECKED;
                    return e->param();
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
                memset(cmds, 0, sizeof(CommandDef) * n);
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
                    unsigned icon = atol(getToken(item, ',').latin1());
                    cmds[n].id   = CmdLocation + n;
                    cmds[n].text = "_";
                    cmds[n].menu_id  = MenuLocation;
                    cmds[n].text_wrk = strdup(number.utf8());
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
            if (cmd->id == CmdPhones){
                for (unsigned i = 0; i < getContacts()->nClients(); i++){
                    Client *client = getContacts()->getClient(i);
                    if (client->protocol()->description()->flags & PROTOCOL_FOLLOWME)
                        return e->param();
                }
                return NULL;
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
                    def = messageTypes.find(m.type);
                    if (def){
                        MessageDef *mdef = (MessageDef*)(def->param);
                        if (mdef->base_type)
                            m.type = mdef->base_type;
                    }
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
                memset(cmds, 0, sizeof(CommandDef) * (count.size() + 1));
                n = 0;
                for (itc = count.begin(); itc != count.end(); ++itc, n++){
                    cmds[n].id = CmdUnread + (*itc).second.index;
                    def = messageTypes.find((*itc).first.type);
                    if (def == NULL)
                        continue;
                    MessageDef *mdef = (MessageDef*)(def->param);
                    cmds[n].icon = def->icon;
                    QString msg = i18n(mdef->singular, mdef->plural, (*itc).second.count);
                    if (contact_id == 0){
                        Contact *contact = getContacts()->contact((*itc).first.contact);
                        if (contact == NULL)
                            continue;
                        msg = i18n("%1 from %2")
                              .arg(msg)
                              .arg(contact->getName());
                    }
                    cmds[n].text_wrk = strdup(msg.utf8());
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
            for (d = client->protocol()->statusList(); d->text; d++){
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
            if (cmd->menu_id == MenuMessage){
                Message *msg;
                if (cmd->id == CmdGrantAuth){
                    Message *from = (Message*)(cmd->param);
                    msg = new AuthMessage(MessageAuthGranted);
                    const char *client_str = from->client();
                    if (client_str == NULL)
                        client_str = "";
                    msg->setContact(from->contact());
                    msg->setClient(client_str);
                    Contact *contact = getContacts()->contact(msg->contact());
                    if (contact){
                        void *data;
                        ClientDataIterator it(contact->clientData);
                        while ((data = ++it) != NULL){
                            Client *client = it.client();
                            if (*client_str){
                                if ((client->dataName(data) == client_str) && client->send(msg, data))
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
                    msg = new AuthMessage(MessageAuthRefused);
                    msg->setContact(from->contact());
                    msg->setClient(from->client());
                    Event eOpen(EventOpenMessage, msg);
                    eOpen.process();
                    delete msg;
                    return e->param();
                }
                CommandDef *def = messageTypes.find(cmd->id);
                if (def == NULL)
                    return NULL;
                MessageDef *mdef = (MessageDef*)(def->param);
                msg = mdef->create(NULL);
                msg->setContact((unsigned)(cmd->param));
                if (mdef->flags & MESSAGE_SILENT){
                    Contact *contact = getContacts()->contact((unsigned)(cmd->param));
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
                Event eOpen(EventOpenMessage, msg);
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
                        p = i18n("%1 wrote:") .arg(name) + "\n" + p;
                        m->setFlags(MESSAGE_FORWARD);
                    }
                    m->setText(p);
                    Event eOpen(EventOpenMessage, m);
                    eOpen.process();
                    delete m;
                    return e->param();
                }
                return NULL;
            }
            if (cmd->id == CmdSeparate){
                Contact *contact = getContacts()->contact((unsigned)(cmd->param));
                if (contact == NULL)
                    return NULL;
                unsigned n = cmd->menu_id - CmdContactClients - 1;
                if (n >= contact->clientData.size())
                    return NULL;
                Contact *newContact = getContacts()->contact(0, true);
                newContact->setGroup(contact->getGroup());
                newContact->clientData.join(n, contact->clientData);
                contact->setup();
                newContact->setup();
                Event e1(EventContactChanged, contact);
                e1.process();
                Event e2(EventContactChanged, newContact);
                e2.process();
                return e->param();
            }
            if (cmd->id == CmdSendClose){
                setCloseSend(cmd->flags & COMMAND_CHECKED);
                return e->param();
            }
            if (cmd->id == CmdSendSMS){
                Contact *contact = getContacts()->contact(0, true);
                contact->setTemporary(CONTACT_TEMP);
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
                unsigned id = (unsigned)(cmd->param);
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
                    if (data.historySize[0] && data.historySize[1])
                        wnd->resize(data.historySize[0], data.historySize[1]);
                }
                raiseWindow(wnd);
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
                }
                raiseWindow(m_cfg);
                return e->param();
            }
            if (cmd->id == CmdSearch){
                if (m_search == NULL){
                    m_search = new SearchDialog;
                    connect(m_search, SIGNAL(finished()), this, SLOT(dialogFinished()));
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
                for (d = client->protocol()->statusList(); d->text; d++){
                    if (d->id == cmd->id)
                        curStatus = d;
                }
                if (curStatus == NULL)
                    return NULL;
                if ((cmd->id != STATUS_ONLINE) && (cmd->id != STATUS_OFFLINE) &&
                        (client->protocol()->description()->flags & (PROTOCOL_AR | PROTOCOL_AR_USER))){
                    const char *noShow = CorePlugin::m_plugin->getNoShowAutoReply(cmd->id);
                    if ((noShow == NULL) || (*noShow == 0)){
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
            if ((cmd->id >= CmdUnread) && (cmd->id < CmdUnread + unread.size())){
                unsigned n = cmd->id - CmdUnread;
                for (list<msg_id>::iterator it = unread.begin(); it != unread.end(); ++it){
                    if (n-- == 0){
                        Message *msg = History::load((*it).id, (*it).client.c_str(), (*it).contact);
                        if (msg){
                            Event e(EventOpenMessage, msg);
                            e.process();
                            delete msg;
                            break;
                        }
                    }
                }
                return e->param();
            }
            if ((cmd->menu_id > CmdContactClients) && (cmd->menu_id <= CmdContactClients + 0x100)){
                Contact *contact = getContacts()->contact((unsigned)(cmd->param));
                CommandDef *def = messageTypes.find(cmd->id);
                if (def && contact){
                    Client *client = NULL;
                    void *data;
                    unsigned n = cmd->menu_id - CmdContactClients - 1;
                    ClientDataIterator it(contact->clientData);
                    while ((data = ++it) != NULL){
                        if (n == 0){
                            client = it.client();
                            break;
                        }
                        n--;
                    }
                    if (client){
                        MessageDef *mdef = (MessageDef*)(def->param);
                        Message *msg = mdef->create(NULL);
                        msg->setContact((unsigned)(cmd->param));
                        msg->setClient(client->dataName(data).c_str());
                        Event eOpen(EventOpenMessage, msg);
                        eOpen.process();
                        delete msg;
                        return e->param();
                    }
                }
            }
            if (cmd->id == CmdShowPanel){
                setShowPanel(cmd->flags & COMMAND_CHECKED);
                showPanel();
            }
            if (cmd->id == CmdHistoryDirection){
                setHistoryDirection(cmd->flags & COMMAND_CHECKED);
            }
            return NULL;
        }
    }
    if (e->type() == EventGoURL){
        string url = (const char*)(e->param());
        string proto;
        int n = url.find(':');
        if (n < 0)
            return NULL;
        proto = url.substr(0, n);
        if (proto != "sms")
            return NULL;
        url = url.substr(proto.length() + 1);
        while (url[0] == '/')
            url = url.substr(1);
        Contact *contact = getContacts()->contactByPhone(url.c_str());
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
    unsigned id = (unsigned)(cmd->param);
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
    if (cfg == NULL)
        cfg = new UserConfig(contact, group);
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
    string saveProfile = getProfile();
    destroy();
    getContacts()->clearClients();
    Event eUnload(EventPluginsUnload, static_cast<Plugin*>(this));
    eUnload.process();
    getContacts()->clear();
    Event eLoad(EventPluginsLoad, static_cast<Plugin*>(this));
    eLoad.process();
    Event eInfo(EventGetPluginInfo, (void*)"_core");
    pluginInfo *info = (pluginInfo*)(eInfo.process());
    free_data(coreData, &data);
    load_data(coreData, &data, info->config);
    if (info->config){
        free(info->config);
        info->config = NULL;
    }
    setProfile(saveProfile.c_str());
    removeTranslator();
    installTranslator();
    initData();
}

void CorePlugin::selectProfile()
{
    Event e(EventSaveState);
    e.process();
    init(false);
}

bool CorePlugin::init(bool bInit)
{
    m_bInit = bInit;
    bool bLoaded = false;
    bool bRes = true;
    bool bNew = false;
    if (!bInit || (*getProfile() == 0) || !getNoShow() || !getSavePasswd()){
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
    }
    if (*getProfile() == 0){
        hideWindows();
        getContacts()->clearClients();
        NewProtocol pDlg(NULL);
        if (!pDlg.exec()){
            Event eAbort(EventPluginsLoad, (void*)ABORT_LOADING);
            eAbort.process();
            return false;
        }
        Client *client = getContacts()->getClient(0);
        string profile = client->name();
        setProfile(NULL);
        QString profileDir = QFile::decodeName(app_file("").c_str());
        profileDir += profile.c_str();
        for (unsigned i = 1;;i++){
            QDir d(profileDir + "." + QString::number(i));
            if (!d.exists()){
                profile += '.';
                profile += number(i);
                break;
            }
        }

        setProfile(profile.c_str());
        bLoaded = true;
        bRes = false;
        bNew = true;
    }
    if (!bLoaded){
        ClientList clients;
        loadClients(clients);
        clients.addToContacts();
    }
    getContacts()->load();
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *client = getContacts()->getClient(i);
        client->setStatus(client->getManualStatus(), client->getCommonStatus());
    }
    if (m_main)
        return true;

    loadUnread();

    m_main = new MainWindow;
    m_view = new UserView;
    if ((data.geometry[WIDTH] == -1) && (data.geometry[HEIGHT] == -1)){
        data.geometry[HEIGHT] = QApplication::desktop()->height() * 2 / 3;
        data.geometry[WIDTH]  = data.geometry[HEIGHT] / 3;
    }
    if ((data.geometry[LEFT] == -1) && (data.geometry[TOP] == -1)){
        data.geometry[LEFT] = QApplication::desktop()->width() - 25 - data.geometry[WIDTH];
        data.geometry[TOP] = 5;
    }
    restoreGeometry(m_main, data.geometry, true, true);

    if (!bNew){
        string containers = getContainers();
        while (!containers.empty())
            new Container(0, getContainer(strtoul(getToken(containers, ',').c_str(), NULL, 10)));
    }
    clearContainer();
    setContainers(NULL);

    m_bInit = true;
    loadMenu();
    if (!bRes){
        Event eInit(EventInit);
        eInit.process();
        Event eSave(EventSaveState);
        eSave.process();
    }
    return bRes;
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
    string saveProfile = getProfile();
    setProfile(NULL);
    bool bOK = false;
    QString baseName = QFile::decodeName(user_file("").c_str());
    QDir dir(baseName);
    dir.setFilter(QDir::Dirs);
    QStringList list = dir.entryList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it){
        QString entry = *it;
        if (entry[0] == '.') continue;
        QString fname = baseName;
#ifdef WIN32
        fname += "\\";
#else
        fname += "/";
#endif
        fname += entry;
#ifdef WIN32
        fname += "\\";
#else
        fname += "/";
#endif
        fname += CLIENTS_CONF;
        QFile f(fname);
        if (f.exists()){
            m_profiles.push_back((const char*)QFile::encodeName(entry));
            if (QFile::encodeName(entry) == saveProfile.c_str())
                bOK = true;
        }
    }
    if (bOK)
        setProfile(saveProfile.c_str());
}

string CorePlugin::getConfig()
{
    string unread_str;
    for (list<msg_id>::iterator itUnread = unread.begin(); itUnread != unread.end(); ++itUnread){
        msg_id &m = (*itUnread);
        if (!unread_str.empty())
            unread_str += ";";
        unread_str += number(m.contact);
        unread_str += ",";
        unread_str += number(m.id);
        unread_str += ",";
        unread_str += m.client;
    }
    setUnread(unread_str.c_str());

    unsigned editBgColor = getEditBackground();
    unsigned editFgColor = getEditForeground();

    QPalette pal = QApplication::palette();
    QColorGroup cg = pal.normal();
    if (((cg.color(QColorGroup::Base).rgb() & 0xFFFFFF) == getEditBackground()) &&
            ((cg.color(QColorGroup::Text).rgb() & 0xFFFFFF) == getEditForeground())){
        setEditBackground(0);
        setEditForeground(0);
    }

    string ef;
    ef = FontEdit::font2str(editFont, false).latin1();
    string def_ef;
    def_ef = FontEdit::font2str(QApplication::font(), false).latin1();
    setEditFont(ef.c_str());
    if (ef == def_ef)
        setEditFont(NULL);

    clearContainer();
    string containers;

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
            if (!containers.empty())
                containers += ',';
            containers += number(c->getId());
            setContainer(c->getId(), c->getState().c_str());
        }
        ++it;
    }
    delete list;
    setContainers(containers.c_str());
    if (m_main){
        saveGeometry(m_main, data.geometry);
        saveToolbar(m_main->bar, data.toolBarState);
    }
    string cfg = save_data(coreData, &data);
    string saveProfile = getProfile();
    setProfile(NULL);
    string cfgName = user_file("plugins.conf");
    QFile fCFG(QFile::decodeName(cfgName.c_str()));
    if (!fCFG.open(IO_WriteOnly | IO_Truncate)){
        log(L_ERROR, "Can't create %s", cfgName.c_str());
    }else{
        string write = "[_core]\n";
        write += "enable,";
        write += number(m_base);
        write += "\n";
        write += cfg;
        fCFG.writeBlock(write.c_str(), write.length());
    }
    fCFG.close();
    setProfile(saveProfile.c_str());
    cfgName = user_file(CLIENTS_CONF);
    QFile f(QFile::decodeName(cfgName.c_str()));
    if (!f.open(IO_WriteOnly | IO_Truncate)){
        log(L_ERROR, "Can't create %s", cfgName.c_str());
    }else{
        for (unsigned i = 0; i < getContacts()->nClients(); i++){
            Client *client = getContacts()->getClient(i);
            Protocol *protocol = client->protocol();
            pluginInfo *info = NULL;
            for (unsigned n = 0;; n++){
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
            string line = "[";
            line += info->name;
            line += "/";
            line += protocol->description()->text;
            line += "]\n";
            f.writeBlock(line.c_str(), line.length());
            line = client->getConfig();
            if (line.length()){
                f.writeBlock(line.c_str(), line.length());
                f.writeBlock("\n", 1);
            }
        }
    }
#ifndef WIN32
    string dir = user_file("");
    chmod(dir.c_str(),S_IRUSR | S_IWUSR | S_IXUSR);
#endif
    string res = save_data(coreData, &data);
    setEditBackground(editBgColor);
    setEditForeground(editFgColor);
    return res;
}

void CorePlugin::loadUnread()
{
    unread.clear();
    string unread_str = getUnread();
    while (!unread_str.empty()){
        string item = getToken(unread_str, ';');
        unsigned contact = atol(getToken(item, ',').c_str());
        unsigned id = atol(getToken(item, ',').c_str());
        Message *msg = History::load(id, item.c_str(), contact);
        if (msg == NULL)
            continue;
        msg_id m;
        m.id = id;
        m.contact = contact;
        m.client = item.c_str();
        m.type = msg->type();
        unread.push_back(m);
    }
    setUnread(NULL);
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

Message *CorePlugin::createMessage(const char *type, const char *cfg)
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
    string cfgName = user_file(CLIENTS_CONF);
    QFile f(QFile::decodeName(cfgName.c_str()));
    if (!f.open(IO_ReadOnly)){
        log(L_ERROR, "Can't open %s", cfgName.c_str());
        return;
    }
    string client_name;
    string cfg;
    string s;
    while (getLine(f, s)){
        if (s[0] == '['){
            Client *client = loadClient(client_name.c_str(), cfg.c_str());
            if (client)
                clients.push_back(client);
            cfg = "";
            s = s.substr(1);
            client_name = getToken(s, ']');
            continue;
        }
        if (cfg.length())
            cfg += "\n";
        cfg += s;
    }
    Client *client = loadClient(client_name.c_str(), cfg.c_str());
    if (client)
        clients.push_back(client);
}

Client *CorePlugin::loadClient(const char *name, const char *cfg)
{
    if ((name == NULL) || (*name == 0))
        return NULL;
    string clientName = name;
    string pluginName = getToken(clientName, '/');
    if ((pluginName.length() == 0) || (clientName.length() == 0))
        return NULL;
    Event e(EventGetPluginInfo, (void*)pluginName.c_str());
    pluginInfo *info = (pluginInfo*)e.process();
    if (info == NULL){
        log(L_DEBUG, "Plugin %s not found", pluginName.c_str());
        return NULL;
    }
    if (info->info == NULL){
        Event e(EventLoadPlugin, (void*)pluginName.c_str());
        e.process();
    }
    if ((info->info == NULL) || !(info->info->flags & (PLUGIN_PROTOCOL & ~PLUGIN_NOLOAD_DEFAULT))){
        log(L_DEBUG, "Plugin %s no protocol", pluginName.c_str());
        return NULL;
    }
    info->bDisabled = false;
    Event eApply(EventApplyPlugin, (void*)pluginName.c_str());
    eApply.process();
    Protocol *protocol;
    ContactList::ProtocolIterator it;
    while ((protocol = ++it) != NULL){
        if (!strcmp(protocol->description()->text, clientName.c_str()))
            return protocol->createClient(cfg);
    }
    log(L_DEBUG, "Protocol %s not found", clientName.c_str());
    return NULL;
}

void CorePlugin::setCurrentProfile(const char *profile)
{
    if (m_profile){
        free(m_profile);
        m_profile = NULL;
    }
    m_profile = strdup(profile);
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
    QString text	= clientName(client);
    cmd->text_wrk = strdup(text.utf8());
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

void CorePlugin::setFonts()
{
    if (getSystemFonts()){
        if (m_saveBaseFont)
            QApplication::setFont(*m_saveBaseFont, true);
        if (m_saveMenuFont)
            QApplication::setFont(*m_saveMenuFont, true, "QPopupMenu");
    }else{
        setupDefaultFonts();
        QPopupMenu m;
        QApplication::setFont(FontEdit::str2font(getBaseFont(), *m_saveBaseFont), true);
        QApplication::setFont(FontEdit::str2font(getMenuFont(), *m_saveMenuFont), true, "QPopupMenu");
    }
}

void CorePlugin::setupDefaultFonts()
{
    if (m_saveBaseFont == NULL)
        m_saveBaseFont = new QFont(QApplication::font());
    if (m_saveMenuFont == NULL){
        QPopupMenu menu;
        m_saveMenuFont = new QFont(QApplication::font(&menu));
    }
}

string CorePlugin::typeName(const char *name)
{
    string text = name;
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
        unsigned menu_id = CmdClient + m_nClients;
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
            c->icon		= NULL;
            c->menu_id	= MenuConnections;
            c->menu_grp	= 0x1000 + menu_id;
            c->popup_id	= menu_id;
            c->flags	= COMMAND_CHECK_STATE;
            eCmd.process();
            unsigned id = 0x100;
            for (; cmd->text; cmd++){
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
    for (ClientList::iterator it = begin(); it != end(); ++it)
        delete *it;
}

void ClientList::addToContacts()
{
    for (ClientList::iterator it = begin(); it != end(); ++it)
        getContacts()->addClient(*it);
    clear();
}

unsigned CorePlugin::getContainerMode()
{
    return data.ContainerMode;
}

void CorePlugin::setContainerMode(unsigned value)
{
    if (value == data.ContainerMode)
        return;
    data.ContainerMode = value;
    emit modeChanged();
}

QString CorePlugin::clientName(Client *client)
{
    QString res = QString::fromUtf8(client->name().c_str());
    int n = res.find('.');
    if (n > 0)
        res = res.left(n) + " " + res.mid(n + 1);
    return res;
}

#ifdef WIN32

/**
 * DLL's entry point
 **/
int WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    return TRUE;
}

/**
 * This is to prevent the CRT from loading, thus making this a smaller
 * and faster dll.
 **/
extern "C" BOOL __stdcall _DllMainCRTStartup( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain( hinstDLL, fdwReason, lpvReserved );
}

#endif

#ifndef WIN32
#include "core.moc"
#endif



