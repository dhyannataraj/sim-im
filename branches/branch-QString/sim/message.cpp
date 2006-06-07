/***************************************************************************
                          message.cpp  -  description
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

#include "simapi.h"

#include <time.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>

#include <vector>

namespace SIM
{

using namespace std;

static DataDef	messageData[] =
    {
        { "Text", DATA_UTF, 1, 0 },
        { "ServerText", DATA_STRING, 1, 0 },
        { "Flags", DATA_ULONG, 1, 0 },
        // Use impossible RGB values as defaults, to signify there's no color set.
        { "Background", DATA_ULONG, 1, (const char*)0xFFFFFFFF },
        { "Foreground", DATA_ULONG, 1, (const char*)0xFFFFFFFF },
        { "Time", DATA_ULONG, 1, 0 },
        { "Font", DATA_STRING, 1, 0 },
        { "", DATA_STRING, 1, 0 },			// Error
        { "", DATA_ULONG, 1, 0 },			// RetryCode
        { "Resource", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

Message::Message(unsigned type, ConfigBuffer *cfg)
{
    m_type = type;
    m_id = 0;
    m_contact = 0;
    load_data(messageData, &data, cfg);
}

Message::~Message()
{
    free_data(messageData, &data);
}

QString Message::getPlainText()
{
    QString res = ((getFlags() & MESSAGE_RICHTEXT) == 0) ? getText() : unquoteText(getText());
    if ((getFlags() & MESSAGE_TRANSLIT) == 0)
        return res;
    return toTranslit(res);
}

QString Message::getRichText()
{
    QString res = ((getFlags() & MESSAGE_RICHTEXT) != 0) ? getText() : QString("<p>") + quoteString(getText()) + "</p>";
    if ((getFlags() & MESSAGE_TRANSLIT) == 0)
        return res;
    return toTranslit(res);
}

QString Message::presentation()
{
    QString res = getRichText();
    // Historically we added some formatting here.
    // Nowadays, formatting is done by XSL.
    return res;
}

void Message::setClient(const QString &client)
{
    m_client = client;
}

QString Message::save()
{
    if (getTime() == 0){
        time_t now;
        time(&now);
        setTime(now);
    }
    unsigned saveFlags = getFlags();
    setFlags(getFlags() & MESSAGE_SAVEMASK);
    QString res = save_data(messageData, &data);
    setFlags(saveFlags);
    return res;
}

QString Message::getText() const
{
    QString res = data.Text.str();
    if (!res.isEmpty())
        return res;
    res = data.ServerText.str();
    if (!res.isEmpty())
        return getContacts()->toUnicode(getContacts()->contact(m_contact), res);
    return res;
}

void Message::setText(const QString &text)
{
    data.Text.setStr(text);
}

static DataDef messageSMSData[] =
    {
        { "Phone", DATA_UTF, 1, 0 },
        { "Network", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

SMSMessage::SMSMessage(ConfigBuffer *cfg)
        : Message(MessageSMS, cfg)
{
    load_data(messageSMSData, &data, cfg);
}

SMSMessage::~SMSMessage()
{
    free_data(messageSMSData, &data);
}

QString SMSMessage::save()
{
    QString s = Message::save();
    QString s1 = save_data(messageSMSData, &data);
    if (!s1.isEmpty()){
        if (!s.isEmpty())
            s += '\n';
        s += s1;
    }
    return s;
}

QString SMSMessage::presentation()
{
    QString phone = quoteString(getPhone());
    QString net   = quoteString(getNetwork());
    if (!net.isEmpty())
        net = QString(" (") + net + ")";
    QString res = QString("<p><a href=\"sms:%1\"><img src=\"icon:cell\">%2%3</a></p>")
                  .arg(phone)
                  .arg(phone)
                  .arg(net);
    res += getRichText();
    return res;
}

static DataDef messageUrlData[] =
    {
        { "Url", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

UrlMessage::UrlMessage(unsigned type, ConfigBuffer *cfg)
        : Message(type, cfg)
{
    load_data(messageUrlData, &data, cfg);
}

UrlMessage::~UrlMessage()
{
    free_data(messageUrlData, &data);
}

QString UrlMessage::save()
{
    QString s = Message::save();
    QString s1 = save_data(messageUrlData, &data);
    if (!s1.isEmpty()){
        if (!s.isEmpty())
            s += '\n';
        s += s1;
    }
    return s;
}

QString UrlMessage::presentation()
{
    QString url = quoteString(getUrl());
    QString res = "<p><a href=\"";
    res += url;
    res += "\">";
    res += url;
    res += "</a></p><p>";
    res += getRichText();
    res += "</p>";
    return res;
}

static DataDef messageContactsData[] =
    {
        { "Contacts", DATA_UTF, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

ContactsMessage::ContactsMessage(unsigned type, ConfigBuffer *cfg)
        : Message(type, cfg)
{
    load_data(messageContactsData, &data, cfg);
}

ContactsMessage::~ContactsMessage()
{
    free_data(messageContactsData, &data);
}

QString ContactsMessage::save()
{
    QString s = Message::save();
    QString s1 = save_data(messageContactsData, &data);
    if (!s1.isEmpty()){
        if (!s.isEmpty())
            s += '\n';
        s += s1;
    }
    return s;
}

QString ContactsMessage::presentation()
{
    QString res;
    QString contacts = getContacts();
    while (contacts.length()){
        QString contact = getToken(contacts, ';');
        QString url = getToken(contact, ',');
        contact = quoteString(contact);
        res += QString("<p><a href=\"%1\">%2</a></p>")
               .arg(url)
               .arg(contact);
    }
    return res;
}

typedef struct fileItem
{
    QString		name;
    unsigned	size;
} fileItem;

class FileMessageIteratorPrivate : public vector<fileItem>
{
public:
    FileMessageIteratorPrivate(const FileMessage &msg);
    vector<fileItem>::iterator it;
    unsigned m_size;
    unsigned m_dirs;
    void add_file(const QString&, bool bFirst);
    void add(const QString&, unsigned size);
    QString save();
};

FileMessageIteratorPrivate::FileMessageIteratorPrivate(const FileMessage &msg)
{
    m_size = 0;
    m_dirs = 0;
    QString files = ((FileMessage&)msg).getFile();
    while (!files.isEmpty()){
        QString item = getToken(files, ';', false);
        QString name = getToken(item, ',');
        if (item.isEmpty()){
            add_file(name, true);
        }else{
            add(name, item.toUInt());
        }
    }
    it = begin();
    if (it != end())
        m_size = it[0].size;
}

void FileMessageIteratorPrivate::add_file(const QString &str, bool bFirst)
{
    QString fn = str;
    QFileInfo f(str);
    if (!f.exists())
        return;
    if (!f.isDir()){
        add(fn, f.size());
        return;
    }
    if (!bFirst){
        add(fn + "/", 0);
        m_dirs++;
    }
    QDir d(str);
    QStringList l = d.entryList();
    for (QStringList::Iterator it = l.begin(); it != l.end(); ++it){
        QString f = *it;
        if ((f == ".") || (f == ".."))
            continue;
        QString p = fn;
        p += "/";
        p += f;
        add_file(p, false);
    }
}

void FileMessageIteratorPrivate::add(const QString &str, unsigned size)
{
    fileItem f;
    f.name = str;
    f.size = size;
    push_back(f);
}

QString FileMessageIteratorPrivate::save()
{
    QString res;
    for (iterator it = begin(); it != end(); ++it){
        fileItem &f = *it;
        if (!res.isEmpty())
            res += ";";
        res += f.name;
        res += ",";
        res += QString::number(f.size);
    }
    return res;
}

FileMessage::Iterator::Iterator(const FileMessage &m)
{
    p = new FileMessageIteratorPrivate(m);
}

FileMessage::Iterator::~Iterator()
{
    delete p;
}

unsigned FileMessage::Iterator::dirs()
{
    return p->m_dirs;
}

const QString *FileMessage::Iterator::operator[](unsigned n)
{
    if (n >= p->size())
        return NULL;
    p->m_size = (*p)[n].size;
    return &(*p)[n].name;
}

const QString *FileMessage::Iterator::operator++()
{
    if (p->it == p->end())
        return NULL;
    const QString *res = &(*(p->it)).name;
    p->m_size = (*(p->it)).size;
    ++(p->it);
    return res;
}

unsigned FileMessage::Iterator::size()
{
    return p->m_size;
}

void FileMessage::Iterator::reset()
{
    p->it = p->begin();
}

unsigned FileMessage::Iterator::count()
{
    return p->size();
}

static DataDef messageFileData[] =
    {
        { "File", DATA_UTF, 1, 0 },
        { "Description", DATA_UTF, 1, 0 },
        { "Size", DATA_ULONG, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

FileMessage::FileMessage(unsigned type, ConfigBuffer *cfg)
        : Message(type, cfg)
{
    load_data(messageFileData, &data, cfg);
    m_transfer = NULL;
}

FileMessage::~FileMessage()
{
    free_data(messageFileData, &data);
    if (m_transfer)
        delete m_transfer;
}

unsigned FileMessage::getSize()
{
    if (data.Size.asULong())
        return data.Size.asULong();
    Iterator it(*this);
    const QString *name;
    while ((name = ++it) != NULL){
        data.Size.asULong() += it.size();
    }
    return data.Size.asULong();
}

void FileMessage::addFile(const QString &file, unsigned size)
{
    Iterator it(*this);
    it.p->add(file, size);
    setFile(it.p->save());
    if (m_transfer){
        m_transfer->m_nFile++;
        m_transfer->m_fileSize = size;
        m_transfer->m_bytes = 0;
        if (m_transfer->m_notify)
            m_transfer->m_notify->process();
    }
}

void FileMessage::setSize(unsigned size)
{
    data.Size.asULong() = size;
}

QString FileMessage::getDescription()
{
    if (!data.Description.str().isEmpty())
        return data.Description.str();
    Iterator it(*this);
    if (it.count() <= 1){
        const QString *name = ++it;
        if (name == NULL)
            return NULL;
        QString shortName = *name;
        shortName = shortName.replace(QRegExp("\\\\"), "/");
        int n = shortName.findRev("/");
        if (n >= 0)
            shortName = shortName.mid(n + 1);
        return QDir::convertSeparators(shortName);
    }
    QString res;
    if (it.dirs()){
        if (it.dirs() == 1){
            res = "1 directory";
        }else{
            res = QString("%1 directories") .arg(it.dirs());
        }
    }
    int nFiles = it.count() - it.dirs();
    if (nFiles){
        if (it.dirs())
            res += ", ";
        if (nFiles == 1){
            res += "1 file";
        }else{
            res += QString("%1 files") .arg(nFiles);
        }
    }
    return res;
}

bool FileMessage::setDescription(const QString &str)
{
    return data.Description.setStr(str);
}

QString FileMessage::save()
{
    QString s = Message::save();
    QString s1 = save_data(messageFileData, &data);
    if (!s1.isEmpty()){
        if (!s.isEmpty())
            s += '\n';
        s += s1;
    }
    return s;
}

QString FileMessage::presentation()
{
    QString res = getDescription();
    unsigned size = getSize();

    if (size){
        res += " ";

        if (size >= 1024 * 1024){
            res += i18n("%1 Mbytes") .arg(size / (1024 * 1024));
        }else if (size >= 1024){
            res += i18n("%1 kbytes") .arg(size / 1024);
        }else{
            res += i18n("%1 bytes") .arg(size);
        }

    }
    QString text = getRichText();
    if (text.length()){
        res += "<br>";
        res += text;
    }
    return res;
}

FileTransfer::FileTransfer(FileMessage *msg)
{
    m_file		 = 0;
    m_msg		 = msg;
    m_notify	 = NULL;
    m_nFile		 = NO_FILE;
    m_bytes		 = 0;
    m_totalBytes = 0;
    m_fileSize	 = 0;
    if (msg){
        FileMessage::Iterator it(*msg);
        m_nFiles	 = it.count();
        m_totalSize	 = msg->getSize();
    }else{
        m_nFiles	 = 0;
        m_totalSize  = 0;
    }
    m_speed		 = 100;
    m_state		 = Unknown;
    m_overwrite  = Ask;
    m_transferBytes = 0;
    m_sendTime   = 0;
    m_sendSize   = 0;
    m_transfer   = 0;
    if (msg){
        if (msg->m_transfer)
            delete msg->m_transfer;
        msg->m_transfer = this;
    }
}

FileTransfer::~FileTransfer()
{
    setNotify(NULL);
    if (m_msg)
        m_msg->m_transfer = NULL;
    if (m_file)
        delete m_file;
}

bool FileTransfer::openFile()
{
    if (m_file){
        delete m_file;
        m_file = NULL;
    }
    m_bDir = false;
    if (++m_nFile >= m_nFiles){
        m_state = Done;
        if (m_notify)
            m_notify->process();
        return false;
    }
    FileMessage::Iterator it(*m_msg);
    QString fn = *it[m_nFile];
    if (fn.isEmpty() || (fn[(int)(fn.length() - 1)] == '/')){
        m_bytes    = 0;
        m_fileSize = 0;
        m_bDir     = true;
        fn = fn.left(fn.length() - 1);
        if (m_base.isEmpty() || (fn.left(m_base.length()) != m_base)){
            int n = fn.findRev("/");
            if (n >= 0)
                m_base = fn.left(n + 1);
        }
        m_name = fn.mid(m_base.length());
        return true;
    }
    if (m_base.isEmpty()){
        int n = fn.findRev("/");
        if (n >= 0)
            m_base = fn.left(n + 1);
    }
    m_bDir = false;
    m_name = fn.mid(m_base.length());
    m_file = new QFile(fn);
    if (!m_file->open(IO_ReadOnly)){
        m_msg->setError(i18n("Can't open %1") .arg(fn));
        setError();
        return false;
    }
    m_bytes    = 0;
    m_fileSize = m_file->size();
    return true;
}

void FileTransfer::setError()
{
    m_state = Error;
    if (m_notify)
        m_notify->process();
}

void FileTransfer::setSpeed(unsigned speed)
{
    m_speed = speed;
}

void FileTransfer::setNotify(FileTransferNotify *notify)
{
    if (m_notify)
        delete m_notify;
    m_notify = notify;
}

QString AuthMessage::presentation()
{
    return "";
}

static DataDef messageStatusData[] =
    {
        { "Status", DATA_ULONG, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

StatusMessage::StatusMessage(ConfigBuffer *cfg)
        : Message(MessageStatus, cfg)
{
    load_data(messageStatusData, &data, cfg);
}

QString StatusMessage::save()
{
    QString s = Message::save();
    QString s1 = save_data(messageStatusData, &data);
    if (!s1.isEmpty()){
        if (!s.isEmpty())
            s += '\n';
        s += s1;
    }
    return s;
}

QString StatusMessage::presentation()
{
    return "";
}

}

