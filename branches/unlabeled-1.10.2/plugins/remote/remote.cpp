/***************************************************************************
                          remote.cpp  -  description
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

#include "remote.h"
#include "remotecfg.h"
#include "simapi.h"
#include "stl.h"

#include "core.h"

#include <qapplication.h>
#include <qwidgetlist.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qfile.h>
#include <time.h>

Plugin *createRemotePlugin(unsigned base, bool, const char *config)
{
    Plugin *plugin = new RemotePlugin(base, config);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Remote control"),
        I18N_NOOP("Plugin provides remote control"),
        VERSION,
        createRemotePlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

static DataDef remoteData[] =
    {
#ifdef WIN32
        { "Path", DATA_STRING, 1, "auto:" },
#else
{ "Path", DATA_STRING, 1, "/tmp/sim.%user%" },
#endif
        { NULL, 0, 0, 0 }
    };

#ifdef WIN32

#include <windows.h>

static RemotePlugin *remote = NULL;

class IPC
{
public:
    IPC();
    ~IPC();
    string prefix();
    void    process();
protected:
    unsigned *s;
    HANDLE	hMem;
    HANDLE	hMutex;
    HANDLE	hEventIn;
    HANDLE	hEventOut;
    bool    bExit;
    static  DWORD __stdcall IPCThread(LPVOID lpParameter);
    friend class IPCLock;
};

class IPCLock
{
public:
    IPCLock(IPC *ipc);
    ~IPCLock();
protected:
    IPC *m_ipc;
};

IPC::IPC()
{
    s = NULL;
    string name = prefix() + "mem";
    hMem = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, N_SLOTS * sizeof(unsigned), name.c_str());
    if (hMem)
        s = (unsigned*)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (s)
        memset(s, 0, N_SLOTS * sizeof(unsigned));
    name = prefix() + "mutex";
    hMutex = CreateMutexA(NULL, FALSE, name.c_str());
    name = prefix() + "in";
    hEventIn = CreateEventA(NULL, TRUE, FALSE, name.c_str());
    name = prefix() + "out";
    hEventOut = CreateEventA(NULL, TRUE, FALSE, name.c_str());
    bExit = false;
}

IPC::~IPC()
{
    bExit = true;
    SetEvent(hEventIn);
    if (s)
        UnmapViewOfFile(s);
    if (hMem)
        CloseHandle(hMem);
    if (hMutex)
        CloseHandle(hMutex);
    if (hEventIn)
        CloseHandle(hEventIn);
    if (hEventOut)
        CloseHandle(hEventOut);
}

DWORD __stdcall IPC::IPCThread(LPVOID lpParameter)
{
    IPC *ipc = (IPC*)lpParameter;
    for (;;){
        ResetEvent(ipc->hEventIn);
        WaitForSingleObject(ipc->hEventIn, INFINITE);
        if (ipc->bExit)
            break;
        QTimer::singleShot(0, remote, SLOT(command()));
    }
    return 0;
}

void IPC::process()
{
    IPCLock(this);
    for (unsigned i = 0; i < N_SLOTS; i++){
        if (s[i] != SLOT_IN)
            continue;
        QString in;
        QString out;
        string name = prefix();
        name += number(i);
        HANDLE hMem = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
        if (hMem == NULL){
            s[i] = SLOT_NONE;
            PulseEvent(hEventOut);
            continue;
        }
        unsigned short *mem = (unsigned short*)MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        unsigned short *p;
        for (p = mem; *p; p++)
            in += QChar(*p);

        bool bError = false;
        bool bRes = remote->command(in, out, bError);
        p = mem;
        if (!bError){
            if (bRes){
                *(p++) = QChar('>').unicode();
            }else{
                *(p++) = QChar('?').unicode();
            }
            for (int n = 0; n < (int)(out.length()); n++)
                *(p++) = out[n].unicode();
        }
        *(p++) = 0;
        UnmapViewOfFile(mem);
        CloseHandle(hMem);
        s[i] = SLOT_OUT;
        PulseEvent(hEventOut);
    }
}

#ifndef SM_REMOTECONTROL
#define SM_REMOTECONTROL	0x2001
#endif
#ifndef SM_REMOTESESSION
#define SM_REMOTESESSION	0x1000
#endif

string IPC::prefix()
{
    string res;
    if (GetSystemMetrics(SM_REMOTECONTROL) || GetSystemMetrics(SM_REMOTESESSION))
        res = "Global/";
    res += SIM_SHARED;
    return res;
}

IPCLock::IPCLock(IPC *ipc)
{
    m_ipc = ipc;
    WaitForSingleObject(m_ipc->hMutex, INFINITE);
}

IPCLock::~IPCLock()
{
    ReleaseMutex(m_ipc->hMutex);
}

#endif

RemotePlugin::RemotePlugin(unsigned base, const char *config)
        : Plugin(base)
{
    load_data(remoteData, &data, config);
    Event ePlugin(EventGetPluginInfo, (void*)"_core");
    pluginInfo *info = (pluginInfo*)(ePlugin.process());
    core = static_cast<CorePlugin*>(info->plugin);
    bind();
#ifdef WIN32
    remote = this;
    ipc = new IPC;
#endif
}

RemotePlugin::~RemotePlugin()
{
#ifdef WIN32
    delete ipc;
#endif
    while (!m_sockets.empty())
        delete m_sockets.front();
    free_data(remoteData, &data);
}

string RemotePlugin::getConfig()
{
    return save_data(remoteData, &data);
}

QWidget *RemotePlugin::createConfigWindow(QWidget *parent)
{
    return new RemoteConfig(parent, this);
}

void *RemotePlugin::processEvent(Event*)
{
    return NULL;
}

static char TCP[] = "tcp:";

void RemotePlugin::bind()
{
    const char *path = getPath();
    if ((strlen(path) > strlen(TCP)) && !memcmp(path, TCP, strlen(TCP))){
        unsigned short port = (unsigned short)atol(path + strlen(TCP));
        ServerSocketNotify::bind(port, port, NULL);
#ifndef WIN32
    }else{
        ServerSocketNotify::bind(path);
#endif
    }
}

bool RemotePlugin::accept(Socket *s, unsigned long)
{
    log(L_DEBUG, "Accept remote control");
    new ControlSocket(this, s);
    return false;
}

void RemotePlugin::bind_ready(unsigned short)
{
}

bool RemotePlugin::error(const char *err)
{
    if (*err)
        log(L_DEBUG, "Remote: %s", err);
    return true;
}

void RemotePlugin::command()
{
#ifdef WIN32
    ipc->process();
#endif
}

const unsigned CMD_STATUS		= 0;
const unsigned CMD_INVISIBLE	= 1;
const unsigned CMD_MAINWND		= 2;
const unsigned CMD_SEARCHWND	= 3;
const unsigned CMD_QUIT			= 4;
const unsigned CMD_CLOSE		= 5;
const unsigned CMD_HELP			= 6;
const unsigned CMD_ADD			= 7;
const unsigned CMD_DELETE		= 8;
const unsigned CMD_OPEN			= 9;
const unsigned CMD_FILE			= 10;
const unsigned CMD_CONTACTS		= 11;
const unsigned CMD_SENDFILE		= 12;

typedef struct cmdDef
{
    const char *cmd;
    const char *shortDescr;
    const char *longDescr;
    unsigned minArgs;
    unsigned maxArgs;
} cmdDef;

static cmdDef cmds[] =
    {
        { "STATUS", "set status", "STATUS [status]", 0, 1 },
        { "INVISIBLE", "set invisible mode", "INVISIBLE [on|off]", 0, 1 },
        { "MAINWINDOW", "show/hide main window", "MAINWINDOW [on|off|toggle]", 0, 1 },
        { "SEARCHWINDOW", "show/hide search window", "SEARCHWINDOW [on|off]", 0, 1 },
        { "QUIT", "quit SIM", "QUIT", 0, 0 },
        { "CLOSE", "close session", "CLOSE", 0, 0 },
        { "HELP", "command help information", "HELP [<cmd>]", 0, 1 },
        { "ADD", "add contact", "ADD <protocol> <address> [<nick>] [<group>]", 2, 4 },
        { "DELETE", "delete contact", "DELETE [<address> | <nick>]", 1, 1 },
        { "OPEN", "open contact", "ADD <protocol> <address> [<nick>] [<group>]", 2, 4 },
        { "FILE", "process UIN file", "FILE <file>", 1, 1 },
        { "CONTACTS", "print contact list", "CONTACTS [<message_type>]", 0, 1 },
        { "SENDFILE", "send file", "SENDFILE <file> <contact>", 2, 2 },
        { NULL, NULL, NULL, 0, 0 }
    };

#if 0
{ "MESSAGE", "send message", "MESSAGE <UIN|Name> <message>", 2, 2 },
{ "SMS", "send SMS", "SMS <phone> <message>", 2, 2 },
{ "DOCK", "show/hide dock", "DOCK [on|off]", 0, 1 },
{ "NOTIFY", "set notify mode", "NOTIFY [on|off]", 0, 1 },
{ "ICON", "get icon in xpm format", "ICON nIcon", 1, 1 },
{ "OPEN", "open unread message", "OPEN", 0, 0 },
{ "POPUP", "show popup", "POPUP x y", 2, 2 },
#endif

static bool isOn(const QString &s)
{
    return (s == "1") || (s == "on") || (s == "ON");
}

static bool cmpStatus(const char *s1, const char *s2)
{
    QString ss1 = s1;
    QString ss2 = s2;
    ss1 = ss1.replace(QRegExp("\\&"), "");
    ss2 = ss2.replace(QRegExp("\\&"), "");
    return ss1.lower() == ss2.lower();
}

static QWidget *findWidget(const char *className)
{
    QWidgetList  *list = QApplication::topLevelWidgets();
    QWidgetListIt it(*list);
    QWidget *w;
    while ((w = it.current()) != NULL){
        if (w->inherits(className))
            break;
        ++it;
    }
    return w;
}

bool RemotePlugin::command(const QString &in, QString &out, bool &bError)
{
    QString cmd;
    vector<QString> args;
    int i = 0;
    for (; i < (int)(in.length()); i++)
        if (in[i] != ' ')
            break;
    for (; i < (int)(in.length()); i++){
        if (in[i] == ' ')
            break;
        cmd += in[i];
    }
    for (; i < (int)(in.length()); ){
        for (; i < (int)(in.length()); i++)
            if (in[i] != ' ')
                break;
        if (i >= (int)(in.length()))
            break;
        QString arg;
        if ((in[i] == '\'') || (in[i] == '\"')){
            QChar c = in[i];
            for (i++; i < (int)(in.length()); i++){
                if (in[i] == c){
                    i++;
                    break;
                }
                arg += in[i];
            }
        }else{
            for (; i < (int)(in.length()); i++){
                if (in[i] == '\\'){
                    i++;
                    if (i >= (int)(in.length()))
                        break;
                    arg += in[i];
                    continue;
                }
                if (in[i] == ' ')
                    break;
                arg += in[i];
            }
        }
        args.push_back(arg);
    }
    unsigned nCmd = 0;
    const cmdDef *c;
    for (c = cmds; c->cmd; c++, nCmd++)
        if (cmd == c->cmd)
            break;
    if (c->cmd == NULL){
        out = "Unknown command ";
        out += cmd;
        return false;
    }
    if ((args.size() < c->minArgs) || (args.size() > c->maxArgs)){
        out = "Bad arguments number. Try help ";
        out += cmd;
        return false;
    }
    QWidget *w;
    unsigned n;
    switch (nCmd){
    case CMD_SENDFILE:{
            FileMessage *msg = new FileMessage;
            msg->setContact(args[1].toUInt());
            msg->setFile(args[0]);
            Event e(EventOpenMessage, &msg);
            e.process();
            delete msg;
            return true;
        }
    case CMD_CONTACTS:{
            unsigned type = 0;
            if (args.size())
                type = args[0].toUInt();
            ContactList::ContactIterator it;
            Contact *contact;
            while ((contact = ++it) != NULL){
                if (type){
                    Command cmd;
                    cmd->id      = type;
                    cmd->menu_id = MenuMessage;
                    cmd->param   = (void*)(contact->id());
                    Event e(EventCheckState, cmd);
                    if (!e.process())
                        continue;
                }
                if (!out.isEmpty())
                    out += "\n";
                out += QString::number(contact->id());
                out += " ";
                out += contact->getName();
            }
            return true;
        }
    case CMD_FILE:{
            QFile f(args[0]);
            if (!f.open(IO_ReadOnly)){
                out = "Can't open ";
                out += args[0];
                return false;
            }
            string line;
            bool bOpen = false;
            unsigned uin = 0;
            while (getLine(f, line)){
                if (line == "[ICQ Message User]")
                    bOpen = true;
                if (line.substr(0, 4) == "UIN=")
                    uin = atol(line.substr(4).c_str());
            }
            if (uin == 0){
                out = "Bad file ";
                out += args[0];
                return false;
            }
            string uin_str = number(uin);
            addContact ac;
            ac.proto = "ICQ";
            ac.addr  = uin_str.c_str();
            ac.nick  = NULL;
            ac.group = 0;
            Event e(EventAddContact, &ac);
            Contact *contact = (Contact*)(e.process());
            if (contact == NULL){
                out = "Can't add user";
                return false;
            }
            if (bOpen){
                Message *m = new Message(MessageGeneric);
                m->setContact(contact->id());
                Event e(EventOpenMessage, &m);
                e.process();
                delete m;
            }
            return true;
        }
    case CMD_STATUS:
        if (args.size()){
            unsigned status = STATUS_UNKNOWN;
            for (n = 0; n < getContacts()->nClients(); n++){
                Client *client = getContacts()->getClient(n);
                for (const CommandDef *d = client->protocol()->statusList(); d->text; d++){
                    if (cmpStatus(d->text, args[0].latin1())){
                        status = d->id;
                        break;
                    }
                }
                if (status != STATUS_UNKNOWN)
                    break;
            }
            if (status == STATUS_UNKNOWN){
                out = "Unknown status ";
                out += args[0];
                return false;
            }
            for (n = 0; n < getContacts()->nClients(); n++){
                Client *client = getContacts()->getClient(n);
                if (client->getCommonStatus())
                    client->setStatus(status, true);
            }
            if (core->getManualStatus() == status)
                return true;
            core->data.ManualStatus.value  = status;
            time_t now;
            time(&now);
            core->data.StatusTime.value = now;
            Event e(EventClientStatus);
            e.process();
            return true;
        }
        for (n = 0; n < getContacts()->nClients(); n++){
            Client *client = getContacts()->getClient(n);
            if (client->getCommonStatus()){
                const CommandDef *d = NULL;
                for (d = client->protocol()->statusList(); d->text; d++){
                    if (d->id == core->getManualStatus())
                        break;
                }
                if (d){
                    out = "STATUS ";
                    for (const char *p = d->text; *p; p++){
                        if (*p == '&')
                            continue;
                        out += *p;
                    }
                    break;
                }
            }
        }
        return true;

    case CMD_INVISIBLE:
        if (args.size()){
            bool bInvisible = isOn(args[0]);
            if (core->getInvisible() != bInvisible){
                core->setInvisible(bInvisible);
                for (unsigned i = 0; i < getContacts()->nClients(); i++)
                    getContacts()->getClient(i)->setInvisible(bInvisible);
            }
        }else{
            out  = "INVISIBLE ";
            out += core->getInvisible() ? "on" : "off";
        }
        return true;
    case CMD_MAINWND:
        w = findWidget("MainWindow");
        if (args.size()){
            if (args[0].lower() == "toggle"){
                if (w){
                    if (w->isVisible()){
                        w->hide();
                    }else{
                        w->show();
                    }
                }
            }else if (isOn(args[0])){
                if (w)
                    raiseWindow(w);
            }else{
                if (w)
                    w->hide();
            }
        }else{
            out += "MAINWINDOW ";
            out += (w ? "on" : "off");
        }
        return true;
    case CMD_SEARCHWND:
        w = findWidget("SearchDialog");
        if (args.size()){
            if (isOn(args[0])){
                if (w){
                    raiseWindow(w);
                }else{
                    Command cc;
                    cc->id = CmdSearch;
                    Event e(EventCommandExec, cc);
                    e.process();
                }
            }else{
                if (w)
                    w->close();
            }
        }else{
            out = "SEARCHWINDOW ";
            out += (w ? "on" : "off");
        }
        return true;
    case CMD_QUIT:{
            Command cc;
            cc->id = CmdQuit;
            Event e(EventCommandExec, cc);
            e.process();
            break;
        }
    case CMD_CLOSE:
        bError = true;
        return false;
    case CMD_OPEN:
    case CMD_ADD:{
            Group *grp = NULL;
            bool  bNewGrp = false;
            if (args.size() > 3){
                ContactList::GroupIterator it;
                while ((grp = ++it) != NULL){
                    if (grp->getName() == args[3])
                        break;
                }
                if (grp == NULL){
                    grp = getContacts()->group(0, true);
                    grp->setName(args[3]);
                    bNewGrp = true;
                }
            }
            string proto;
            proto = args[0].utf8();
            string addr;
            addr  = args[1].utf8();
            string nick;
            addContact ac;
            ac.proto = proto.c_str();
            ac.addr  = addr.c_str();
            if (args.size() > 2){
                nick = args[2].utf8();
                ac.nick = nick.c_str();
            }else{
                ac.nick = NULL;
            }
            ac.group = 0;
            if (grp)
                ac.group = grp->id();
            Event e(EventAddContact, &ac);
            Contact *contact = (Contact*)(e.process());
            if (contact){
                if (bNewGrp){
                    Event e(EventGroupChanged, grp);
                    e.process();
                }
                if (nCmd == CMD_OPEN){
                    Message *m = new Message(MessageGeneric);
                    m->setContact(contact->id());
                    Event e(EventOpenMessage, &m);
                    e.process();
                    delete m;
                }
                return true;
            }
            if (bNewGrp)
                delete grp;
            out += "Can't create ";
            out += args[1];
            return false;
        }
    case CMD_DELETE:{
            ContactList::ContactIterator it;
            Contact *contact;
            while ((contact = ++it) != NULL){
                if (contact->getName() == args[0]){
                    delete contact;
                    return true;
                }
            }
            string s;
            s = args[0].utf8();
            Event e(EventDeleteContact, (void*)(s.c_str()));
            if (e.process())
                return true;
            out = "Contact ";
            out += args[0];
            out += " not found";
            return false;
        }
    case CMD_HELP:
        if (args.size() == 0){
            for (c = cmds; c->cmd; c++){
                out += c->cmd;
                out += "\t";
                out += c->shortDescr;
                out += "\n";
            }
        }else{
            for (c = cmds; c->cmd; c++)
                if (args[0] == c->cmd)
                    break;
            if (c->cmd == NULL){
                out = "Unknown command ";
                out += args[0];
                return false;
            }
            out = c->cmd;
            out += "\t";
            out += c->shortDescr;
            out += "\n";
            out += c->longDescr;
        }
        return true;
    }
    return false;
}

static char CRLF[] = "\r\n>";

ControlSocket::ControlSocket(RemotePlugin *plugin, Socket *socket)
{
    m_plugin = plugin;
    m_plugin->m_sockets.push_back(this);
    m_socket = new ClientSocket(this);
    m_socket->setSocket(socket);
    m_socket->setRaw(true);
    m_socket->readBuffer.init(0);
    m_socket->readBuffer.packetStart();
    write(CRLF);
}

ControlSocket::~ControlSocket()
{
    for (list<ControlSocket*>::iterator it = m_plugin->m_sockets.begin(); it != m_plugin->m_sockets.end(); ++it){
        if ((*it) == this){
            m_plugin->m_sockets.erase(it);
            break;
        }
    }
    delete m_socket;
}

void ControlSocket::write(const char *msg)
{
    log(L_DEBUG, "Remote write %s", msg);
    m_socket->writeBuffer.packetStart();
    m_socket->writeBuffer.pack(msg, strlen(msg));
    m_socket->write();
}

bool ControlSocket::error_state(const char *err, unsigned)
{
    if (err && *err)
        log(L_WARN, "ControlSocket error %s", err);
    return true;
}

void ControlSocket::connect_ready()
{
}

void ControlSocket::packet_ready()
{
    string line;
    if (!m_socket->readBuffer.scan("\n", line))
        return;
    if (line.empty())
        return;
    if (line[(int)line.size() - 1] == '\r')
        line = line.substr(0, line.size() - 1);
    log(L_DEBUG, "Remote read: %s", line.c_str());
    QString out;
    bool bError = false;
    bool bRes = m_plugin->command(QString::fromLocal8Bit(line.c_str()), out, bError);
    if (bError){
        m_socket->error_state("");
        return;
    }
    if (!bRes)
        write("? ");
    string s;
    s = out.local8Bit();
    string res;
    for (const char *p = s.c_str(); *p; p++){
        if (*p == '\r')
            continue;
        if (*p == '\n')
            res += '\r';
        res += *p;
    }
    write(res.c_str());
    write(CRLF);
}

#ifdef WIN32

#include <windows.h>

/**
 * DLL's entry point
 **/
int WINAPI DllMain(HINSTANCE, DWORD, LPVOID)
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
#include "remote.moc"
#endif

