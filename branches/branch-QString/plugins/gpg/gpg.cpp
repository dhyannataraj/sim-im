/***************************************************************************
                          gpg.cpp  -  description
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

#include "gpg.h"
#include "gpgcfg.h"
#include "gpguser.h"
#include "core.h"
#include "msgedit.h"
#include "textshow.h"
#include "userwnd.h"
#include "passphrase.h"
#include "ballonmsg.h"

#include <qtimer.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qstring.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

using namespace SIM;

#ifndef WIN32
static QString GPGpath;
#endif

Plugin *createGpgPlugin(unsigned base, bool, ConfigBuffer *cfg)
{
#ifndef WIN32
    if (GPGpath.isEmpty())
        return NULL;
#endif
    Plugin *plugin = new GpgPlugin(base, cfg);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("GPG"),
        I18N_NOOP("Plugin adds GnuPG encryption/decryption support for messages"),
        VERSION,
        createGpgPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
#ifndef WIN32
    QString path;
    const char *p = getenv("PATH");
    if (p)
        path = QFile::decodeName(p);
    while (!path.isEmpty()){
        QString p = getToken(path, ':');
        p += "/gpg";
        QFile f(p);
        QFileInfo fi(f);
        if (fi.isExecutable()){
            GPGpath = p;
            break;
        }
    }
    if (GPGpath.isEmpty())
        info.description = I18N_NOOP("Plugin adds GnuPG encryption/decryption support for messages\n"
                                     "GPG not found in PATH");
#endif
    return &info;
}

static DataDef gpgData[] =
    {
        { "GPG", DATA_STRING, 1, 0 },
        { "Home", DATA_STRING, 1, "keys/" },
        { "GenKey", DATA_STRING, 1, "--gen-key --batch" },
        { "PublicList", DATA_STRING, 1, "--with-colon --list-public-keys" },
        { "SecretList", DATA_STRING, 1, "--with-colon --list-secret-keys" },
        { "Import", DATA_STRING, 1, "--import %keyfile%" },
        { "Export", DATA_STRING, 1, "--batch --yes --armor --comment \"\" --no-version --export %userid%" },
        { "Encrypt", DATA_STRING, 1, "--batch --yes --armor --comment \"\" --no-version --recipient %userid% --trusted-key %userid% --output %cipherfile% --encrypt %plainfile%" },
        { "Decrypt", DATA_STRING, 1, "--yes --passphrase-fd 0 --output \"%plainfile%\" --decrypt %cipherfile%" },
        { "Key", DATA_STRING, 1, 0 },
        { "Passphrases", DATA_UTFLIST, 1, 0 },
        { "Keys", DATA_STRLIST, 1, 0 },
        { "NPassphrases", DATA_ULONG, 1, 0 },
        { "", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static DataDef gpgUserData[] =
    {
        { "Key", DATA_STRING, 1, 0 },
        { "Use", DATA_BOOL, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

GpgPlugin *GpgPlugin::plugin = NULL;

GpgPlugin::GpgPlugin(unsigned base, ConfigBuffer *cfg)
        : Plugin(base), EventReceiver(HighestPriority - 0x100)
{
    load_data(gpgData, &data, cfg);
    m_bMessage = false;
    m_passphraseDlg = NULL;
    user_data_id = getContacts()->registerUserData(info.title, gpgUserData);
    reset();
    plugin = this;
}

GpgPlugin::~GpgPlugin()
{
    if (m_passphraseDlg)
        delete m_passphraseDlg;
    unregisterMessage();
    free_data(gpgData, &data);
    QValueList<DecryptMsg>::iterator it;
    for (it = m_decrypt.begin(); it != m_decrypt.end(); ++it){
        delete (*it).msg;
        delete (*it).process;
    }
    for (it = m_import.begin(); it != m_import.end(); ++it){
        delete (*it).msg;
        delete (*it).process;
    }
    for (it = m_public.begin(); it != m_public.end(); ++it)
        delete (*it).process;
    for (it = m_wait.begin(); it != m_wait.end(); ++it)
        delete (*it).msg;
    getContacts()->unregisterUserData(user_data_id);
}

QString GpgPlugin::GPG()
{
#ifdef WIN32
    return getGPG();
#else
    return GPGpath;
#endif
}

void GpgPlugin::clear()
{
    QValueList<DecryptMsg>::iterator it;
    for (it = m_decrypt.begin(); it != m_decrypt.end();){
        if ((*it).msg){
            ++it;
            continue;
        }
        delete (*it).process;
        QFile::remove((*it).infile);
        QFile::remove((*it).outfile);
        m_decrypt.erase(it);
        it = m_decrypt.begin();
    }
    for (it = m_import.begin(); it != m_import.end(); ){
        if ((*it).msg){
            ++it;
            continue;
        }
        delete (*it).process;
        QFile::remove((*it).infile);
        QFile::remove((*it).outfile);
        m_import.erase(it);
        it = m_import.begin();
    }
    for (it = m_public.begin(); it != m_public.end(); ){
        if ((*it).contact){
            ++it;
            continue;
        }
        delete (*it).process;
        QFile::remove((*it).infile);
        QFile::remove((*it).outfile);
        m_public.erase(it);
        it = m_public.begin();
    }
}

void GpgPlugin::decryptReady()
{
    int res = 0;
        for (QValueList<DecryptMsg>::iterator it = m_decrypt.begin(); it != m_decrypt.end(); ++it){
                QProcess *p = (*it).process;
        if (p && !p->isRunning()){
            Message *msg = (*it).msg;
            (*it).msg = NULL;
            QTimer::singleShot(0, this, SLOT(clear()));
            if (p->normalExit() && p->exitStatus() == 0){
                QFile f((*it).outfile);
                if (f.open(IO_ReadOnly)){
                    QByteArray ba = f.readAll();
                    msg->setText(QString::fromUtf8(ba));
                    msg->setFlags(msg->getFlags() | MESSAGE_SECURE);
                }else{
                    QString s;
                    s = (*it).outfile;
                    log(L_WARN, "Can't open output decrypt file %s", s.local8Bit().data());
                    res = -1;
                }
                if (!(*it).key.isEmpty()){
                    unsigned i = 1;
                    for (i = 1; i <= getnPassphrases(); i++){
                        if ((*it).key == getKeys(i))
                            break;
                    }
                    if (i > getnPassphrases()){
                        setnPassphrases(i);
                        setKeys(i, (*it).key);
                    }
                    setPassphrases(i, (*it).passphrase);
                    for (;;){
                        QValueList<DecryptMsg>::iterator itw;
                        bool bDecode = false;
                        for (itw = m_wait.begin(); itw != m_wait.end(); ++itw){
                            if ((*itw).key == (*it).key){
                                decode((*itw).msg, (*it).passphrase.utf8(), (*it).key);
                                m_wait.erase(itw);
                                bDecode = true;
                                break;
                            }
                        }
                        if (!bDecode)
                            break;
                    }
                    if (m_passphraseDlg && ((*it).key == m_passphraseDlg->m_key)){
                        delete m_passphraseDlg;
                        m_passphraseDlg = NULL;
                        askPassphrase();
                    }
                }
            }else{
                QCString key;
                QCString res;
                QString passphrase;
                QCString str(p->readStderr());
                key = getToken(str, '\n');
                if (str.contains("bad passphrase")){
                    int n = key.find("ID ");
                    if (n > 0)
                        key = key.mid(n + 3);
                    key = getToken(key, ' ');
                    key = getToken(key, ',');
                    if (m_passphraseDlg && ((*it).key == m_passphraseDlg->m_key)){
                        DecryptMsg m;
                        m.msg    = msg;
                        m.key    = key;
                        m_wait.push_back(m);
                        m_passphraseDlg->error();
                        return;
                    }
                    if ((*it).passphrase.isEmpty()){
                        for (unsigned i = 1; i <= getnPassphrases(); i++){
                            if (QString::fromLocal8Bit(key) == getKeys(i)){
                                passphrase = getPassphrases(i);
                                break;
                            }
                        }
                    }
                    if ((*it).passphrase.isEmpty() && !passphrase.isEmpty()){
                        if (decode(msg, passphrase, key))
                            return;
                    }else{
                        DecryptMsg m;
                        m.msg    = msg;
                        m.key    = key;
                        m_wait.push_back(m);
                        (*it).msg = NULL;
                        QTimer::singleShot(0, this, SLOT(clear()));
                        askPassphrase();
                        return;
                    }
                }else{
                    if (m_passphraseDlg && ((*it).key == m_passphraseDlg->m_key)){
                        delete m_passphraseDlg;
                        m_passphraseDlg = NULL;
                        askPassphrase();
                    }
                }
            }
            Event e(EventMessageReceived, msg);
            if ((res == 0) && processEvent(&e))
                return;
            if (!e.process(this))
                delete msg;
            return;
        }
    }
    log(L_WARN, "No decrypt exec");
}

void GpgPlugin::importReady()
{
    for (QValueList<DecryptMsg>::iterator it = m_import.begin(); it != m_import.end(); ++it){
        QProcess *p = (*it).process;
        if (p && !p->isRunning()){
            if (p->normalExit() && p->exitStatus() == 0){
                Message *msg = new Message(MessageGPGKey);
                QByteArray ba = p->readStderr();
                QString err = QString::fromLocal8Bit(ba.data(), ba.size());
                QRegExp r1("[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]:");
                QRegExp r2("\".*\"");
                int len;
                int pos = r1.match(err, 0, &len);
                if (pos >= 0){
                    QString key_name;
                    key_name  = err.mid(pos + 1, len - 2);
                    QString text = key_name;
                    text += " ";
                    pos = r2.match(err, 0, &len);
                    text += err.mid(pos + 1, len - 2);
                    msg->setText(text);
                    msg->setContact((*it).msg->contact());
                    msg->setClient((*it).msg->client());
                    msg->setFlags((*it).msg->getFlags());
                    delete (*it).msg;
                    (*it).msg = msg;

                    QString home = user_file(GpgPlugin::plugin->getHome());
                    if (home.endsWith("\\") || home.endsWith("/"))
                        home = home.left(home.length() - 1);

                    QStringList sl;
                    sl += GPG();
                    sl += "--no-tty";
                    sl += "--homedir";
                    sl += home;
                    sl += QStringList::split(' ', getPublicList());

                    QProcess *proc = new QProcess(sl, this);

                    DecryptMsg dm;
                    dm.process = proc;
                    dm.contact = msg->contact();
                    dm.outfile = key_name;
                    m_public.push_back(dm);
                    connect(dm.process, SIGNAL(processExited()), this, SLOT(publicReady()));
                    dm.process->start();
                }
            }
            Event e(EventMessageReceived, (*it).msg);
            if (!e.process(this))
                delete (*it).msg;
            (*it).msg = NULL;
            QTimer::singleShot(0, this, SLOT(clear()));
            return;
        }
    }
    log(L_WARN, "No decrypt exec");
}

QString GpgPlugin::getConfig()
{
    QStringList keys;
    QStringList passphrases;
    unsigned i;
    for (i = 1; i <= getnPassphrases(); i++){
        keys.append(getKeys(i));
        passphrases.append(getPassphrases(i));
    }
    if (!getSavePassphrase()){
        clearKeys();
        clearPassphrases();
    }
    QString res = save_data(gpgData, &data);
    for (i = 0; i < getnPassphrases(); i++){
        setKeys(i + 1, keys[i].latin1());
        setPassphrases(i + 1, passphrases[i]);
    }
    return res;
}

void *GpgPlugin::processEvent(Event *e)
{
    switch (e->type()){
    case EventCheckState:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if (cmd->menu_id == MenuMessage){
                if (cmd->id == MessageGPGKey){
                    cmd->flags &= ~COMMAND_CHECKED;
                    CommandDef c = *cmd;
                    c.id = MessageGeneric;
                    Event eCheck(EventCheckState, &c);
                    return eCheck.process();
                }
                if (cmd->id == MessageGPGUse){
                    cmd->flags &= ~COMMAND_CHECKED;
                    Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                    if (contact == NULL)
                        return NULL;
                    GpgUserData *data = (GpgUserData*)(contact->userData.getUserData(user_data_id, false));
                    if (!data || data->Key.str().isEmpty())
                        return NULL;
                    if (data->Use.asBool())
                        cmd->flags |= COMMAND_CHECKED;
                    return e->param();
                }
            }
            return NULL;
        }
    case EventCommandExec:{
            CommandDef *cmd = (CommandDef*)(e->param());
            if ((cmd->menu_id == MenuMessage) && (cmd->id == MessageGPGUse)){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                GpgUserData *data = (GpgUserData*)(contact->userData.getUserData(user_data_id, false));
                if (data && !data->Key.str().isEmpty())
                    data->Use.asBool() = (cmd->flags & COMMAND_CHECKED) != 0;
                return e->param();
            }
            return NULL;
        }
    case EventCheckSend:{
            CheckSend *cs = (CheckSend*)(e->param());
            if ((cs->id == MessageGPGKey) && cs->client->canSend(MessageGeneric, cs->data))
                return e->param();
            return NULL;
        }
    case EventMessageSent:{
            Message *msg = (Message*)(e->param());
            for (QValueList<KeyMsg>::iterator it = m_sendKeys.begin(); it != m_sendKeys.end(); ++it){
                if ((*it).msg == msg){
                    if (msg->getError().isEmpty()){
                        Message m(MessageGPGKey);
                        m.setText((*it).key);
                        m.setClient(msg->client());
                        m.setContact(msg->contact());
                        Event e(EventSent, &m);
                        e.process();
                    }
                    m_sendKeys.erase(it);
                    break;
                }
            }
            return NULL;
        }
    case EventMessageSend:{
            Message *msg = (Message*)(e->param());
            if (msg->type() == MessageGeneric){
                Contact *contact = getContacts()->contact(msg->contact());
                if (contact){
                    GpgUserData *data = (GpgUserData*)(contact->userData.getUserData(user_data_id, false));
                    if (data && !data->Key.str().isEmpty() && data->Use.asBool()){
                        msg->setFlags(msg->getFlags() | MESSAGE_SECURE);
                        if (msg->getFlags() & MESSAGE_RICHTEXT){
                            QString text = msg->getPlainText();
                            msg->setText(text);
                            msg->setFlags(msg->getFlags() & ~MESSAGE_RICHTEXT);
                        }
                    }
                }
            }
            return NULL;
        }
    case EventSend:{
            messageSend *ms = (messageSend*)(e->param());
            if ((ms->msg->type() == MessageGeneric) &&
                    (ms->msg->getFlags() & MESSAGE_SECURE)){
                Contact *contact = getContacts()->contact(ms->msg->contact());
                if (contact){
                    GpgUserData *data = (GpgUserData*)(contact->userData.getUserData(user_data_id, false));
                    if (data && !data->Key.str().isEmpty() && data->Use.asBool()){
                        QString output = user_file("m.");
                        output += QString::number((unsigned long)ms->msg);
                        QString input = output + ".in";
                        QFile in(input);
                        if (!in.open(IO_WriteOnly | IO_Truncate)){
                            log(L_WARN, "Can't create %s", (const char *)input.local8Bit());
                            return NULL;
                        }
                        QCString *cstr = ms->text;
                        in.writeBlock(cstr->data(), cstr->length());
                        in.close();
                        QString home = user_file(GpgPlugin::plugin->getHome());
                        if (home.endsWith("\\") || home.endsWith("/"))
                            home = home.left(home.length() - 1);

                        QStringList sl;
                        sl += GPG();
                        sl += "--no-tty";
                        sl += "--homedir";
                        sl += home;
                        sl += QStringList::split(' ', getEncrypt());
                        sl = sl.gres(QRegExp("\\%plainfile\\%"), input);
                        sl = sl.gres(QRegExp("\\%cipherfile\\%"), output);
                        sl = sl.gres(QRegExp("\\%userid\\%"), data->Key.str());

                        QProcess proc(sl, this);

                        if(!proc.launch("\n"))
                            return ms->msg;

                        // FIXME: not soo good...
                        while(proc.isRunning())
                            qApp->processEvents();

                        if (!proc.normalExit() || proc.exitStatus() != 0){
                            ms->msg->setError(I18N_NOOP("Encrypt failed"));
                            QFile::remove(input);
                            QFile::remove(output);
                            return ms->msg;
                        }
                        QFile::remove(input);
                        QFile out(output);
                        if (!out.open(IO_ReadOnly)){
                            QFile::remove(output);
                            ms->msg->setError(I18N_NOOP("Encrypt failed"));
                            return ms->msg;
                        }
                        *ms->text = out.readAll();
                        out.close();
                        QFile::remove(output);
                        return NULL;
                    }
                }
            }
            return NULL;
        }
    case EventMessageReceived:{
            Message *msg = (Message*)(e->param());
            if ((msg->baseType() == MessageGeneric) && m_bMessage){
                QString text = msg->getPlainText();
                const char SIGN_MSG[] = "-----BEGIN PGP MESSAGE-----";
                const char SIGN_KEY[] = "-----BEGIN PGP PUBLIC KEY BLOCK-----";
                if (text.startsWith(SIGN_MSG)){
                    if (decode(msg, "", ""))
                        return msg;
                    return NULL;
                }
                if (text.startsWith(SIGN_KEY)){
                    QString input = user_file("m.");
                    input  += QString::number((unsigned long)msg);
                    input += ".in";
                    QFile in(input);
                    if (!in.open(IO_WriteOnly | IO_Truncate)){
                        log(L_WARN, "Can't create %s", input.local8Bit().data());
                        return NULL;
                    }
                    QByteArray ba = text.utf8();
                    in.writeBlock(ba);
                    in.close();
                    QString home = user_file(GpgPlugin::plugin->getHome());
                    if (home.endsWith("\\") || home.endsWith("/"))
                        home = home.left(home.length() - 1);

                    QStringList sl;
                    sl += GPG();
                    sl += "--no-tty";
                    sl += "--homedir";
                    sl += home;
                    sl += QStringList::split(' ', getImport());
                    sl = sl.gres(QRegExp("\\%keyfile\\%"), input);

                    QProcess *proc = new QProcess(sl, this);

                    DecryptMsg dm;
                    dm.process = proc;
                    dm.msg     = msg;
                    dm.infile  = input;
                    m_import.push_back(dm);
                    connect(dm.process, SIGNAL(processExited()), this, SLOT(importReady()));
                    dm.process->launch("\n");
                    return msg;
                }
            }
            return NULL;
        }
    }
    return NULL;
}

static unsigned decode_index = 0;

bool GpgPlugin::decode(Message *msg, const QString &aPassphrase, const QString &key)
{
    QString output = user_file("md.");
    output += QString::number(decode_index++);
    QString input = output + ".in";
    QFile in(input);
    if (!in.open(IO_WriteOnly | IO_Truncate)){
        log(L_WARN, "Can't create %s", input.local8Bit().data());
        return false;
    }
    QByteArray ba = msg->getPlainText().utf8();
    in.writeBlock(ba);
    in.close();
    QString home = user_file(GpgPlugin::plugin->getHome());
    if (home.endsWith("\\") || home.endsWith("/"))
        home = home.left(home.length() - 1);

    QStringList sl;
    sl += GPG();
    sl += "--no-tty";
    sl += "--homedir";
    sl += home;
    sl += QStringList::split(' ', getDecrypt());
    sl = sl.gres(QRegExp("\\%plainfile\\%"), output);
    sl = sl.gres(QRegExp("\\%cipherfile\\%"), input);

    QProcess *proc = new QProcess(sl, this);

    DecryptMsg dm;
    dm.process = proc;
    dm.msg     = msg;
    dm.infile  = input;
    dm.outfile = output;
    dm.passphrase = aPassphrase;
    dm.key        = key;
    m_decrypt.push_back(dm);

    connect(dm.process, SIGNAL(processExited()), this, SLOT(decryptReady()));
        dm.process->launch (aPassphrase + "\n");
    return true;
}

void GpgPlugin::publicReady()
{
    for (QValueList<DecryptMsg>::iterator it = m_public.begin(); it != m_public.end(); ++it){
                QProcess *p = (*it).process;
        if (p && !p->isRunning()){
            if (p->normalExit() && p->exitStatus() == 0){
                QCString str(p->readStdout());
                for (;;){
                    QCString line;
                    line = getToken(str, '\n');
                    if(line.isEmpty())
                            break;
                    QCString type = getToken(line, ':');
                    if (type == "pub"){
                        getToken(line, ':');
                        getToken(line, ':');
                        getToken(line, ':');
                        QCString sign = getToken(line, ':');
                        QString name = (*it).outfile;
                        int pos = sign.length() - name.length();
                        if (pos < 0)
                            pos = 0;
                        if (sign.mid(pos) == name.latin1()){
                            Contact *contact = getContacts()->contact((*it).contact);
                            if (contact){
                                GpgUserData *data = (GpgUserData*)(contact->userData.getUserData(user_data_id, true));
                                data->Key.str() = sign;
                            }
                            break;
                        }
                    }
                }
            }
            (*it).contact = 0;
            break;
        }
    }
}

void GpgPlugin::passphraseApply(const QString &passphrase)
{
    for (QValueList<DecryptMsg>::iterator it = m_wait.begin(); it != m_wait.end(); ++it){
        if ((*it).key == m_passphraseDlg->m_key){
            Message *msg = (*it).msg;
            m_wait.erase(it);
            decode(msg, passphrase, m_passphraseDlg->m_key);
            return;
        }
    }
    delete m_passphraseDlg;
    m_passphraseDlg = NULL;
    askPassphrase();
}

QWidget *GpgPlugin::createConfigWindow(QWidget *parent)
{
    return new GpgCfg(parent, this);
}

void GpgPlugin::reset()
{
    if (!GPG().isEmpty() && !getHome().isEmpty() && !getKey().isEmpty()){
#ifdef HAVE_CHMOD
        chmod(user_file(getHome()), 0700);
#endif
        registerMessage();
    }else{
        unregisterMessage();
    }
}

#if 0
i18n("%n GPG key", "%n GPG keys", 1);
#endif

static Message *createGPGKey(ConfigBuffer *cfg)
{
    return new Message(MessageGPGKey, cfg);
}

static QObject *generateGPGKey(MsgEdit *p, Message *msg)
{
    return new MsgGPGKey(p, msg);
}

static MessageDef defGPGKey =
    {
        NULL,
        NULL,
        MESSAGE_INFO | MESSAGE_SYSTEM,
        "%n GPG key",
        "%n GPG keys",
        createGPGKey,
        generateGPGKey,
        NULL
    };

static MessageDef defGPGUse =
    {
        NULL,
        NULL,
        MESSAGE_SILENT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };

static QWidget *getGpgSetup(QWidget *parent, void *data)
{
    return new GpgUser(parent, (GpgUserData*)data);
}

void GpgPlugin::registerMessage()
{
    if (m_bMessage)
        return;
    m_bMessage = true;
    Command cmd;
    cmd->id             = MessageGPGKey;
    cmd->text           = I18N_NOOP("GPG key");
    cmd->icon           = "encrypted";
    cmd->param          = &defGPGKey;
    cmd->menu_grp       = 0x4081;
    Event eMsg(EventCreateMessageType, cmd);
    eMsg.process();

    cmd->id             = MessageGPGUse;
    cmd->text           = I18N_NOOP("Use GPG encryption");
    cmd->icon           = "";
    cmd->param          = &defGPGUse;
    cmd->menu_grp       = 0x4080;
    eMsg.process();

    cmd->id      = user_data_id + 1;
    cmd->text    = I18N_NOOP("&GPG key");
    cmd->icon    = "encrypted";
    cmd->param   = (void*)getGpgSetup;
    Event e(EventAddPreferences, cmd);
    e.process();
}

void GpgPlugin::unregisterMessage()
{
    if (!m_bMessage)
        return;
    m_bMessage = false;
    Event e(EventRemoveMessageType, (void*)MessageGPGKey);
    e.process();
    Event eUse(EventRemoveMessageType, (void*)MessageGPGUse);
    eUse.process();
    Event eUser(EventRemovePreferences, (void*)user_data_id);
    eUser.process();
}

void GpgPlugin::askPassphrase()
{
    if (m_passphraseDlg || m_wait.empty())
        return;
    m_passphraseDlg = new PassphraseDlg(this, m_wait.front().key);
    connect(m_passphraseDlg, SIGNAL(finished()), this, SLOT(passphraseFinished()));
    connect(m_passphraseDlg, SIGNAL(apply(const QString&)), this, SLOT(passphraseApply(const QString&)));
    raiseWindow(m_passphraseDlg);
}

void GpgPlugin::passphraseFinished()
{
    if (m_passphraseDlg){
        for (QValueList<DecryptMsg>::iterator it = m_wait.begin(); it != m_wait.end();){
            if ((*it).key != m_passphraseDlg->m_key){
                ++it;
                continue;
            }
            Event e(EventMessageReceived, (*it).msg);
            if (!e.process(this))
                delete (*it).msg;
            m_wait.erase(it);
            it = m_wait.begin();
        }
    }
    m_passphraseDlg = NULL;
    askPassphrase();
}

MsgGPGKey::MsgGPGKey(MsgEdit *parent, Message *msg)
        : QObject(parent)
{
    m_client = msg->client();
    m_edit   = parent;
    m_edit->m_edit->setText("");
    m_edit->m_edit->setReadOnly(true);
    m_edit->m_edit->setTextFormat(PlainText);
    m_edit->m_edit->setParam(m_edit);

    Command cmd;
    cmd->id    = CmdSend;
    cmd->flags = COMMAND_DISABLED;
    cmd->param = m_edit;
    Event e(EventCommandDisabled, cmd);
    e.process();

    QString gpg  = GpgPlugin::plugin->GPG();
    QString home = user_file(GpgPlugin::plugin->getHome());
    m_key = GpgPlugin::plugin->getKey();
    if (home.endsWith("\\") || home.endsWith("/"))
        home = home.left(home.length() - 1);

    QStringList sl;
    sl += GpgPlugin::plugin->GPG();
    sl += "--no-tty";
    sl += "--homedir";
    sl += home;
    sl += QStringList::split(' ', GpgPlugin::plugin->getExport());
    sl = sl.gres(QRegExp("\\%userid\\%"), m_key);

    m_process = new QProcess(sl, this);

    connect(m_process, SIGNAL(processExited()), this, SLOT(exportReady()));
    if (!m_process->start())
        exportReady();
}

MsgGPGKey::~MsgGPGKey()
{
    delete m_process;
}

void MsgGPGKey::init()
{
    m_edit->m_edit->setFocus();
}

void MsgGPGKey::exportReady()
{
    if (m_process->normalExit() && m_process->exitStatus() == 0) {
        QByteArray ba1 = m_process->readStdout();
        m_edit->m_edit->setText(QString::fromLocal8Bit(ba1.data(), ba1.size()));
        if(ba1.isEmpty()) {
            QByteArray ba2 = m_process->readStderr();
            QString errStr;
            if(!ba2.isEmpty())
                errStr = " (" + QString::fromLocal8Bit( ba2.data(), ba2.size() ) + ")";
            BalloonMsg::message(i18n("Can't read gpg key") + errStr, m_edit->m_edit);
        }
    }

    Command cmd;
    cmd->id    = CmdSend;
    cmd->flags = 0;
    cmd->param = m_edit;
    Event e(EventCommandDisabled, cmd);
    e.process();

    delete m_process;
    m_process = 0;
}

void *MsgGPGKey::processEvent(Event *e)
{
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->param == m_edit){
            unsigned id = cmd->bar_grp;
            if ((id >= MIN_INPUT_BAR_ID) && (id < MAX_INPUT_BAR_ID)){
                cmd->flags |= BTN_HIDE;
                return e->param();
            }
            switch (cmd->id){
            case CmdSend:
            case CmdSendClose:
                e->process(this);
                cmd->flags &= ~BTN_HIDE;
                return e->param();
            case CmdTranslit:
            case CmdSmile:
            case CmdNextMessage:
            case CmdMsgAnswer:
                e->process(this);
                cmd->flags |= BTN_HIDE;
                return e->param();
            }
        }
    }
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if ((cmd->id == CmdSend) && (cmd->param == m_edit)){
            QString msgText = m_edit->m_edit->text();
            if (!msgText.isEmpty()){
                Message *msg = new Message;
                msg->setText(msgText);
                msg->setContact(m_edit->m_userWnd->id());
                msg->setClient(m_client);
                msg->setFlags(MESSAGE_NOHISTORY);
                KeyMsg km;
                km.key = m_key;
                km.msg = msg;
                GpgPlugin::plugin->m_sendKeys.push_back(km);
                MsgSend s;
                s.edit = m_edit;
                s.msg  = msg;
                Event e(EventRealSendMessage, &s);
                e.process();
            }
            return e->param();
        }
    }
    return NULL;
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


#endif

#ifndef _MSC_VER
#include "gpg.moc"
#endif

