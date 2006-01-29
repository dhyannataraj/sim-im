/***************************************************************************
                          filter.cpp  -  description
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

#include "filter.h"
#include "filtercfg.h"
#include "simapi.h"
#include "ballonmsg.h"
#include "core.h"
#include "msgedit.h"
#include "msgview.h"
#include "userwnd.h"

#include <qregexp.h>

using namespace SIM;

Plugin *createFilterPlugin(unsigned base, bool, Buffer *cfg)
{
    Plugin *plugin = new FilterPlugin(base, cfg);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Filter"),
        I18N_NOOP("Plugin provides message filter"),
        VERSION,
        createFilterPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

/*
typedef struct FilterData
{
	unsigned FromList;
} FilterData;
*/
static DataDef filterData[] =
    {
        { "FromList", DATA_BOOL, 1, 0 },
        { "AuthFromList", DATA_BOOL, 1, 0 },
        { NULL, 0, 0, 0 }
    };

static DataDef filterUserData[] =
    {
        { "SpamList", DATA_UTF, 1, 0 },
        { NULL, 0, 0, 0 }
    };

static FilterPlugin *filterPlugin = NULL;

static QWidget *getFilterConfig(QWidget *parent, void *data)
{
    return new FilterConfig(parent, (FilterUserData*)data, filterPlugin, false);
}

FilterPlugin::FilterPlugin(unsigned base, Buffer *cfg)
        : Plugin(base), EventReceiver(HighPriority - 1)
{
    filterPlugin = this;

    load_data(filterData, &data, cfg);
    user_data_id = getContacts()->registerUserData(info.title, filterUserData);

    CmdIgnoreList	= registerType();
    CmdIgnore		= registerType();
    CmdIgnoreText	= registerType();

    Command cmd;
    cmd->id          = CmdIgnoreList;
    cmd->text        = I18N_NOOP("Ignore list");
    cmd->menu_id     = MenuContactGroup;
    cmd->menu_grp    = 0x8080;
    cmd->flags		 = COMMAND_CHECK_STATE;

    Event eCmd(EventCommandCreate, cmd);
    eCmd.process();

    cmd->id          = CmdIgnore;
    cmd->text        = I18N_NOOP("Ignore user");
    cmd->icon		 = "ignorelist";
    cmd->menu_id     = 0;
    cmd->menu_grp    = 0;
    cmd->bar_id		 = ToolBarContainer;
    cmd->bar_grp	 = 0x7001;
    cmd->flags		 = COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->id          = CmdIgnoreText;
    cmd->text        = I18N_NOOP("Ignore this phrase");
    cmd->icon		 = NULL;
    cmd->menu_id     = MenuTextEdit;
    cmd->menu_grp    = 0x7000;
    cmd->bar_id		 = 0;
    cmd->bar_grp	 = 0;
    cmd->flags		 = COMMAND_CHECK_STATE;
    eCmd.process();

    cmd->menu_id     = MenuMsgView;
    eCmd.process();

    cmd->id			 = user_data_id + 1;
    cmd->text		 = I18N_NOOP("&Filter");
    cmd->icon		 = "filter";
    cmd->menu_id	 = 0;
    cmd->menu_grp	 = 0;
    cmd->param		 = (void*)getFilterConfig;
    Event ePref(EventAddPreferences, cmd);
    ePref.process();
}

FilterPlugin::~FilterPlugin()
{
    free_data(filterData, &data);

    Event ePref(EventRemovePreferences, (void*)user_data_id);
    ePref.process();

    Event eCmd(EventCommandRemove, (void*)CmdIgnoreList);
    eCmd.process();

    getContacts()->unregisterUserData(user_data_id);
}

std::string FilterPlugin::getConfig()
{
    return save_data(filterData, &data);
}

void *FilterPlugin::processEvent(Event *e)
{
    if (e->type() == EventContactChanged){
        Contact *contact = (Contact*)(e->param());
        if (contact->getGroup()){
            Command cmd;
            cmd->id		= CmdIgnore;
            cmd->flags	= BTN_HIDE;
            cmd->param  = (void*)(contact->id());
            Event eShow(EventCommandShow, cmd);
            eShow.process();
        }
        return NULL;
    }
    if (e->type() == EventMessageReceived){
        Message *msg = (Message*)(e->param());
        if (!msg || (msg->type() == MessageStatus))
            return NULL;
        Contact *contact = getContacts()->contact(msg->contact());
        FilterUserData *data = NULL;
        // check if we accept only from users on the list
        if (
            ((contact == NULL) || contact->getFlags() & CONTACT_TEMPORARY) &&
	    (
	        getFromList() ||
		( getAuthFromList() && msg->type() <= MessageContacts)
            )
	) {
            delete msg;
            delete contact;
            return msg;
        }
        if (!contact)
            return NULL;
        // check if the user is a ignored user
        if (contact->getIgnore()){
            delete msg;
            return msg;
        }

        // get filter-data
        data = (FilterUserData*)(contact->getUserData(user_data_id));
        if (data && data->SpamList.ptr && *data->SpamList.ptr){
            if (checkSpam(msg->getPlainText(), QString::fromUtf8(data->SpamList.ptr))){
                delete msg;
                return msg;
            }
        }
        return NULL;
    }
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdIgnore){
            cmd->flags &= ~BTN_HIDE;
            Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
            if (contact && contact->getGroup())
                cmd->flags |= BTN_HIDE;
            return e->param();
        }
        if (cmd->id == CmdIgnoreText){
            cmd->flags &= ~COMMAND_CHECKED;
            if (cmd->menu_id == MenuMsgView){
                MsgViewBase *edit = (MsgViewBase*)(cmd->param);
                if (edit->hasSelectedText())
                    return e->param();
            }else if (cmd->menu_id == MenuTextEdit){
                TextEdit *edit = ((MsgEdit*)(cmd->param))->m_edit;
                if (edit->hasSelectedText())
                    return e->param();
            }
            return NULL;
        }
        if (cmd->menu_id == MenuContactGroup){
            if (cmd->id == CmdIgnoreList){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                cmd->flags &= COMMAND_CHECKED;
                if (contact->getIgnore())
                    cmd->flags |= COMMAND_CHECKED;
                return e->param();
            }
        }
    }
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->id == CmdIgnore){
            Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
            if (contact){
                QString text = i18n("Add %1 to ignore list?") .arg(contact->getName());
                Command cmd;
                cmd->id		= CmdIgnore;
                cmd->param	= (void*)(contact->id());
                Event e(EventCommandWidget, cmd);
                QWidget *w = (QWidget*)(e.process());
                BalloonMsg::ask((void*)(contact->id()), text, w, SLOT(addToIgnore(void*)), NULL, NULL, this);
            }
            return e->param();
        }
        if (cmd->id == CmdIgnoreText){
            QString text;
            unsigned id = 0;
            if (cmd->menu_id == MenuMsgView){
                MsgViewBase *view = (MsgViewBase*)(cmd->param);
                if (view->hasSelectedText()){
                    text = view->selectedText();
#if (COMPAT_QT_VERSION < 0x030000) || (COMPAT_QT_VERSION >= 0x030100)
                    text = unquoteText(text);
#endif
                    id = view->m_id;
                }
            }else if (cmd->menu_id == MenuTextEdit){
                MsgEdit *medit = (MsgEdit*)(cmd->param);
                TextEdit *edit = medit->m_edit;
                if (edit->hasSelectedText()){
                    text = edit->selectedText();
#if (COMPAT_QT_VERSION < 0x030000) || (COMPAT_QT_VERSION >= 0x030100)
                    if (edit->textFormat() == QTextEdit::RichText)
                        text = unquoteText(text);
#endif
                    id = medit->m_userWnd->id();
                }
            }
            FilterUserData *data = NULL;
            Contact *contact = getContacts()->contact(id);
            if (contact){
                data = (FilterUserData*)(contact->getUserData(user_data_id));
            }else{
                data = (FilterUserData*)(getContacts()->getUserData(user_data_id));
            }
            QString s;
            s = QString::fromUtf8(data->SpamList.ptr);
            while (!text.isEmpty()){
                QString line = getToken(text, '\n');
                line = line.replace(QRegExp("\r"), "");
                if (line.isEmpty())
                    continue;
                bool bSpace = false;
                for (int i = 0; i < (int)(line.length()); i++)
                    if (line[i] == ' '){
                        bSpace = true;
                        break;
                    }
                if (bSpace)
                    line = QString("\"") + line + "\"";
                if (!s.isEmpty())
                    s += " ";
                s += line;
            }
            set_str(&data->SpamList.ptr, s.utf8());
            return NULL;
        }
        if (cmd->menu_id == MenuContactGroup){
            if (cmd->id == CmdIgnoreList){
                Contact *contact = getContacts()->contact((unsigned long)(cmd->param));
                if (contact == NULL)
                    return NULL;
                contact->setIgnore((cmd->flags & COMMAND_CHECKED) == 0);
                Event eContact(EventContactChanged, contact);
                eContact.process();
                return e->param();
            }
        }
    }
    return NULL;
}

QWidget *FilterPlugin::createConfigWindow(QWidget *parent)
{
    FilterUserData *data = (FilterUserData*)(getContacts()->getUserData(user_data_id));
    return new FilterConfig(parent, data, this, true);
}

static bool match(const QString &text, const QString &pat)
{
    int i;
    for (i = 0; (i < (int)(text.length())) && (i < (int)(pat.length())); i++){
        QChar c = pat[i];
        if (c == '?')
            continue;
        if (c == '*'){
            int n;
            for (n = i; n < (int)(pat.length()); n++)
                if (pat[n] != '*')
                    break;
            QString p = pat.mid(n);
            if (p.isEmpty())
                return true;
            for (n = i; n < (int)(text.length()); n++){
                QString t = text.mid(n);
                if (match(text, p))
                    return true;
            }
            return false;
        }
        if (text[i] != c)
            return false;
    }
    return (i == (int)(text.length())) && (i == (int)(pat.length()));
}

bool FilterPlugin::checkSpam(const QString &text, const QString &_filter)
{
    QString filter = _filter;
    QStringList wordsText;
    getWords(text, wordsText, false);
    bool bQuota = false;
    while (!filter.isEmpty()){
        QString filterPart = getToken(filter, '\"');
        QStringList wordsFilter;
        getWords(filterPart, wordsFilter, true);
        if (wordsFilter.count()){
            if (bQuota){
                for (QStringList::Iterator it = wordsText.begin(); it != wordsText.end(); ++it){
                    if (!match(*it, wordsFilter[0]))
                        continue;
                    QStringList::Iterator it1 = it;
                    QStringList::Iterator itFilter = wordsFilter.begin();
                    for (; (it1 != wordsText.end()) && (itFilter != wordsFilter.end()); ++it1, ++itFilter){
                        if (!match(*it1, *itFilter))
                            break;
                    }
                    if (itFilter == wordsFilter.end())
                        return true;
                }
            }else{
                for (QStringList::Iterator it = wordsText.begin(); it != wordsText.end(); ++it){
                    for (QStringList::Iterator itFilter = wordsFilter.begin(); itFilter != wordsFilter.end(); ++itFilter){
                        if (match(*it, *itFilter))
                            return true;
                    }
                }
            }
        }
        bQuota = !bQuota;
    }
    return false;
}

void FilterPlugin::getWords(const QString &text, QStringList &words, bool bPattern)
{
    QString word;
    for (int i = 0; i < (int)(text.length()); i++){
        QChar c = text[i];
        if (c.isLetterOrNumber()){
            word += c;
            continue;
        }
        if (bPattern && ((c == '?') || (c == '*'))){
            word += c;
            continue;
        }
        if (word.isEmpty())
            continue;
        words.append(word);
        word = "";
    }
    if (!word.isEmpty())
        words.append(word);
}

void FilterPlugin::addToIgnore(void *p)
{
    Contact *contact = getContacts()->contact((unsigned long)p);
    if (contact && !contact->getIgnore()){
        contact->setIgnore(true);
        Event e(EventContactChanged, contact);
        e.process();
    }
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
#include "filter.moc"
#endif

