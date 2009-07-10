/***************************************************************************
                          cfgdlg.cpp  -  description
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

#include "icons.h"
#include "cfgdlg.h"
#include "plugincfg.h"
#include "maininfo.h"
#include "arcfg.h"
#include "buffer.h"
#include "core.h"

#include <qpixmap.h>
#include <q3listview.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <q3widgetstack.h>
#include <qobject.h>
#include <q3header.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QByteArray>
#include <QCloseEvent>

namespace ConfigDlg
{

using namespace std;
using namespace SIM;

const unsigned CONFIG_ITEM  = 1;
const unsigned PLUGIN_ITEM  = 2;
const unsigned CLIENT_ITEM  = 3;
const unsigned MAIN_ITEM    = 4;
const unsigned AR_ITEM      = 5;

class ConfigItem : public Q3ListViewItem
{
public:
    ConfigItem(Q3ListViewItem *item, unsigned id);
    ConfigItem(Q3ListView *view, unsigned id);
    ~ConfigItem();
    void show();
    void deleteWidget();
    virtual void apply();
    virtual unsigned type() { return CONFIG_ITEM; }
    unsigned id() { return m_id; }
    static unsigned curIndex;
    bool raisePage(QWidget *w);
    QWidget *widget() { return m_widget; }
    QWidget *m_widget;
protected:
    unsigned m_id;
    static unsigned defId;
    void init(unsigned id);
    virtual QWidget *getWidget(ConfigureDialog *dlg);
};

unsigned ConfigItem::defId = 0x10000;
unsigned ConfigItem::curIndex;

ConfigItem::ConfigItem(Q3ListView *view, unsigned id)
        : Q3ListViewItem(view)
{
    init(id);
}

ConfigItem::ConfigItem(Q3ListViewItem *item, unsigned id)
        : Q3ListViewItem(item)
{
    init(id);
}

ConfigItem::~ConfigItem()
{
    deleteWidget();
}


void ConfigItem::deleteWidget()
{
    if (m_widget){
        delete m_widget;
        m_widget = NULL;
    }
}

void ConfigItem::init(unsigned id)
{
    m_widget = NULL;
    m_id = id;
    QString key = QString::number(++curIndex);
    while (key.length() < 4)
        key = '0' + key;
    setText(1, key);
}

bool ConfigItem::raisePage(QWidget *w)
{
    if (m_widget == w){
        listView()->setCurrentItem(this);
        return true;
    }
    for (Q3ListViewItem *item = firstChild(); item; item = item->nextSibling()){
        if (static_cast<ConfigItem*>(item)->raisePage(w))
            return true;
    }
    return false;
}

void ConfigItem::show()
{
    ConfigureDialog *dlg = static_cast<ConfigureDialog*>(listView()->topLevelWidget());
    if (m_widget == NULL){
        m_widget = getWidget(dlg);
        if (m_widget == NULL)
            return;
        dlg->wnd->addWidget(m_widget, id() ? id() : defId++);
        dlg->wnd->setMinimumSize(dlg->wnd->sizeHint());
        QObject::connect(dlg, SIGNAL(applyChanges()), m_widget, SLOT(apply()));
        QTimer::singleShot(50, dlg, SLOT(repaintCurrent()));
    }
    dlg->showUpdate(type() == CLIENT_ITEM);
    dlg->wnd->raiseWidget(m_widget);
}

void ConfigItem::apply()
{
}

QWidget *ConfigItem::getWidget(ConfigureDialog*)
{
    return NULL;
}

class PluginItem : public ConfigItem
{
public:
    PluginItem(Q3ListViewItem *view, const QString &text, pluginInfo *info, unsigned id);
    pluginInfo *info() { return m_info; }
    virtual void apply();
    virtual unsigned type() { return PLUGIN_ITEM; }
private:
    virtual QWidget *getWidget(ConfigureDialog *dlg);
    pluginInfo *m_info;
};

PluginItem::PluginItem(Q3ListViewItem *item, const QString &text, pluginInfo *info, unsigned id)
        : ConfigItem(item, id)
{
    m_info = info;
    setText(0, text);
    setText(1, text);
}

void PluginItem::apply()
{
    if (m_info->bNoCreate)
        return;
    if (m_info->info && (m_info->info->flags & PLUGIN_NODISABLE))
        return;
    if (m_widget){
        PluginCfg *w = static_cast<PluginCfg*>(m_widget);
        if (w->chkEnable->isChecked() == m_info->bDisabled){
            m_info->bDisabled = !w->chkEnable->isChecked();
            delete m_widget;
            m_widget = NULL;
        }
    }
    EventApplyPlugin e(m_info->name);
    e.process();
}

QWidget *PluginItem::getWidget(ConfigureDialog *dlg)
{
    return new PluginCfg(dlg->wnd, m_info);
}

class ClientItem : public ConfigItem
{
public:
    ClientItem(Q3ListViewItem *item, Client *client, CommandDef *cmd);
    ClientItem(Q3ListView *view, Client *client, CommandDef *cmd);
    Client *client() { return m_client;  }
    virtual unsigned type() { return CLIENT_ITEM; }
private:
    void init();
    virtual QWidget *getWidget(ConfigureDialog *dlg);
    CommandDef *m_cmd;
    Client *m_client;
};

ClientItem::ClientItem(Q3ListViewItem *item, Client *client, CommandDef *cmd)
        : ConfigItem(item, 0)
{
    m_client = client;
    m_cmd	 = cmd;
    init();
}

ClientItem::ClientItem(Q3ListView *view, Client *client, CommandDef *cmd)
        : ConfigItem(view, 0)
{
    m_client = client;
    m_cmd    = cmd;
    init();
}

void ClientItem::init()
{
    if (!m_cmd->text_wrk.isEmpty()){
        setText(0, m_cmd->text_wrk);
        m_cmd->text_wrk = QString::null;
    }else{
        setText(0, i18n(m_cmd->text));
    }
    if (!m_cmd->icon.isEmpty())
        setPixmap(0, Pict(m_cmd->icon));
}

QWidget *ClientItem::getWidget(ConfigureDialog *dlg)
{
    QWidget *res = m_client->configWindow(dlg, m_cmd->id);
    if (res)
        QObject::connect(dlg, SIGNAL(applyChanges(SIM::Client*, void*)), res, SLOT(apply(SIM::Client*, void*)));
    return res;
}

class ARItem : public ConfigItem
{
public:
    ARItem(Q3ListViewItem *view, const CommandDef *d);
    virtual unsigned type() { return AR_ITEM; }
private:
    virtual QWidget *getWidget(ConfigureDialog *dlg);
    unsigned m_status;
};

ARItem::ARItem(Q3ListViewItem *item, const CommandDef *d)
        : ConfigItem(item, 0)
{
    QString icon;

    m_status = d->id;
    setText(0, i18n(d->text));
    switch (d->id){
    case STATUS_ONLINE: 
        icon="SIM_online";
        break;
    case STATUS_AWAY:
        icon="SIM_away";
        break;
    case STATUS_NA:
        icon="SIM_na";
        break;
    case STATUS_DND:
        icon="SIM_dnd";
        break;
	case STATUS_OCCUPIED:
        icon="SIM_occupied";
        break;
    case STATUS_FFC:
        icon="SIM_ffc";
        break;
    case STATUS_OFFLINE:
        icon="SIM_offline";
        break;
    default:
        icon=d->icon;
        break;
    }
    setPixmap(0, Pict(icon));
}

QWidget *ARItem::getWidget(ConfigureDialog *dlg)
{
    return new ARConfig(dlg, m_status, text(0), NULL);
}

class MainInfoItem : public ConfigItem
{
public:
    MainInfoItem(Q3ListView *view, unsigned id);
    unsigned type() { return MAIN_ITEM; }
protected:
    virtual QWidget *getWidget(ConfigureDialog *dlg);
};

MainInfoItem::MainInfoItem(Q3ListView *view, unsigned id)
        : ConfigItem(view, id)
{
    setText(0, i18n("User info"));
    setPixmap(0, Pict("info"));
}

QWidget *MainInfoItem::getWidget(ConfigureDialog *dlg)
{
    return new MainInfo(dlg, NULL);
}

}

using namespace ConfigDlg;

ConfigureDialog::ConfigureDialog() : QDialog(NULL)
{
    setupUi(this);
    m_nUpdates = 0;
    setWindowIcon(Icon("configure"));
    setButtonsPict(this);
    setTitle();
    lstBox->header()->hide();
    QIcon iconSet = Icon("webpress");
    if (!iconSet.pixmap(QSize(16,16), QIcon::Normal).isNull())
        btnUpdate->setIcon(iconSet);
    btnUpdate->hide();
    lstBox->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    fill(0);
    connect(buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
    connect(btnUpdate, SIGNAL(clicked()), this, SLOT(updateInfo()));
    connect(lstBox, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(itemSelected(Q3ListViewItem*)));
    lstBox->setCurrentItem(lstBox->firstChild());
    itemSelected(lstBox->firstChild());
}

ConfigureDialog::~ConfigureDialog()
{
    lstBox->clear();
    for (unsigned long n = 0;; n++){
        EventGetPluginInfo e(n);
        e.process();
        const pluginInfo *info = e.info();
        if (info == NULL)
            break;
        if (info->plugin == NULL)
            continue;
        if (info->bDisabled){
            EventUnloadPlugin eUnload(info->name);
            eUnload.process();
        }
    }
	::saveGeometry(this, CorePlugin::m_plugin->data.CfgGeometry);
}

static unsigned itemWidth(Q3ListViewItem *item, QFontMetrics &fm)
{
    unsigned w = fm.width(item->text(0)) + 64;
    for (Q3ListViewItem *child = item->firstChild(); child ; child = child->nextSibling()){
        w = QMAX(w, itemWidth(child, fm));
    }
    return w;
}

void ConfigureDialog::fill(unsigned id)
{
    lstBox->clear();
    lstBox->setSorting(1);

    ConfigItem *parentItem = new MainInfoItem(lstBox, 0);
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *client = getContacts()->getClient(i);
        CommandDef *cmds = client->configWindows();
        if (cmds){
            parentItem = NULL;
            for (; !cmds->text.isEmpty(); cmds++){
                if (parentItem){
                    new ClientItem(parentItem, client, cmds);
                }else{
                    parentItem = new ClientItem(lstBox, client, cmds);
                    parentItem->setOpen(true);
                }
            }
        }
    }

    unsigned long n;
    parentItem = NULL;
    list<unsigned> st;
    for (n = 0; n < getContacts()->nClients(); n++){
        Protocol *protocol = getContacts()->getClient(n)->protocol();
        if ((protocol->description()->flags & (PROTOCOL_AR | PROTOCOL_AR_USER)) == 0)
            continue;
        if (parentItem == NULL){
            parentItem = new ConfigItem(lstBox, 0);
            parentItem->setText(0, i18n("Autoreply"));
            parentItem->setOpen(true);
        }
        for (const CommandDef *d = protocol->statusList(); !d->text.isEmpty(); d++){
            if (((protocol->description()->flags & PROTOCOL_AR_OFFLINE) == 0) &&
                    ((d->id == STATUS_ONLINE) || (d->id == STATUS_OFFLINE)))
                continue;
            list<unsigned>::iterator it;
            for (it = st.begin(); it != st.end(); ++it)
                if ((*it) == d->id)
                    break;
            if (it != st.end())
                continue;
            st.push_back(d->id);
            new ARItem(parentItem, d);
        }
    }

    parentItem = new ConfigItem(lstBox, 0);
    parentItem->setText(0, i18n("Plugins"));
    parentItem->setPixmap(0, Pict("run"));
    parentItem->setOpen(true);

    for ( n = 0;; n++){
        EventGetPluginInfo e(n);
        e.process();
        pluginInfo *info = e.info();
        if (info == NULL)
            break;
        if (info->info == NULL){
            EventLoadPlugin e(info->name);
            e.process();
        }
        if ((info->info == NULL) || (info->info->title == NULL)) continue;
        QString title = i18n(info->info->title);
        new PluginItem(parentItem, title, info, n);
    }

    QFontMetrics fm(lstBox->font());
    unsigned w = 0;
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        w = QMAX(w, itemWidth(item, fm));
    }
    lstBox->setFixedWidth(w);
    lstBox->setColumnWidth(0, w - 2);

    if (id){
        for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
            if (setCurrentItem(item, id))
                return;
        }
    }
    lstBox->setCurrentItem(lstBox->firstChild());
}

bool ConfigureDialog::setCurrentItem(Q3ListViewItem *parent, unsigned id)
{
    if (static_cast<ConfigItem*>(parent)->id() == id){
        lstBox->setCurrentItem(parent);
        return true;
    }
    for (Q3ListViewItem *item = parent->firstChild(); item; item = item->nextSibling()){
        if (setCurrentItem(item, id))
            return true;
    }
    return false;
}

void ConfigureDialog::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
    emit finished();
}

void ConfigureDialog::itemSelected(Q3ListViewItem *item)
{
    if (item){
        static_cast<ConfigItem*>(item)->show();
        lstBox->setCurrentItem(item);
    }
}

void ConfigureDialog::apply(Q3ListViewItem *item)
{
    static_cast<ConfigItem*>(item)->apply();
    for (item = item->firstChild(); item; item = item->nextSibling())
        apply(item);
}

void ConfigureDialog::reject()
{
    QDialog::reject();
    emit finished();
}

void ConfigureDialog::apply()
{
    bLanguageChanged = false;
    m_bAccept = true;
    emit applyChanges();
    if (!m_bAccept)
        return;
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *client = getContacts()->getClient(i);
        const DataDef *def = client->protocol()->userDataDef();
        if (def == NULL)
            continue;
        size_t size = 0;
        for (const DataDef *d = def; d->name; ++d)
            size += d->n_values;
        Data *data = new Data[size];
        QByteArray cfg = client->getConfig();
        if (cfg.isEmpty()){
            load_data(def, data, NULL);
        }else{
            Buffer config;
            config = "[Title]\n" + cfg;
            config.setWritePos(0);
            config.getSection();
            load_data(def, data, &config);
        }
        emit applyChanges(client, data);
        client->setClientInfo(data);
        free_data(def, data);
        delete[] data;
    }
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        apply(item);
    }
    if (bLanguageChanged){
        unsigned id = 0;
        if (lstBox->currentItem())
            id = static_cast<ConfigItem*>(lstBox->currentItem())->id();
        disconnect(lstBox, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(itemSelected(Q3ListViewItem*)));
        fill(id);
        connect(lstBox, SIGNAL(currentChanged(Q3ListViewItem*)), this, SLOT(itemSelected(Q3ListViewItem*)));
        itemSelected(lstBox->currentItem());
        buttonApply->setText(i18n("&Apply"));
        buttonOk->setText(i18n("&OK"));
        buttonCancel->setText(i18n("&Cancel"));
        setWindowTitle(i18n("Setup"));
    }
    if (lstBox->currentItem())
        static_cast<ConfigItem*>(lstBox->currentItem())->show();
    EventSaveState e;
    e.process();
}

bool ConfigureDialog::processEvent(Event *e)
{
    if (e->type() == eEventLanguageChanged)
        bLanguageChanged = true;
    if (e->type() == eEventPluginChanged){
        EventPluginChanged *p = static_cast<EventPluginChanged*>(e);
        pluginInfo *info = p->info();
        if (info && info->plugin == NULL){
            for (Q3ListViewItem *i = lstBox->firstChild(); i; i = i->nextSibling()){
                ConfigItem *item = static_cast<ConfigItem*>(i);
                if (item->type() != PLUGIN_ITEM)
                    continue;
                if (static_cast<PluginItem*>(item)->info() == info){
                    item->deleteWidget();
                    break;
                }
            }
        }
    }
    if (e->type() == eEventClientsChanged){
        unsigned id = 0;
        if (lstBox->currentItem())
            id = static_cast<ConfigItem*>(lstBox->currentItem())->id();
        fill(id);
    }
    if (e->type() == eEventClientChanged){
        if (m_nUpdates){
            if (--m_nUpdates == 0){
                setTitle();
                btnUpdate->setEnabled(true);
            }
        }
    }
    return false;
}

void ConfigureDialog::setTitle()
{
    QString title = i18n("Configure");
    if (m_nUpdates){
        title += " [";
        title += i18n("Update info");
        title += ']';
    }
    setWindowTitle(title);
}

void ConfigureDialog::accept()
{
    apply();
    if (m_bAccept){
        QDialog::accept();
        emit finished();
    }
}

void ConfigureDialog::showUpdate(bool bShow)
{
    if (bShow){
        btnUpdate->show();
    }else{
        btnUpdate->hide();
    }
}

void ConfigureDialog::updateInfo()
{
    if (m_nUpdates)
        return;
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        m_nUpdates++;
        getContacts()->getClient(i)->updateInfo(NULL, NULL);
    }
    btnUpdate->setEnabled(!m_nUpdates);
    setTitle();
}

void ConfigureDialog::raisePage(Client *client)
{
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        if (static_cast<ConfigItem*>(item)->type() != CLIENT_ITEM)
            continue;
        if (static_cast<ClientItem*>(item)->client() == client){
            lstBox->setCurrentItem(item);
            lstBox->ensureItemVisible(item);
            return;
        }
    }
}

void ConfigureDialog::raisePage(QWidget *widget)
{
    if (!m_bAccept)
        return;
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        if (static_cast<ConfigItem*>(item)->raisePage(widget)){
            m_bAccept = false;
            break;
        }
    }
}

void ConfigureDialog::raisePhoneBook()
{
    lstBox->setCurrentItem(lstBox->firstChild());
    QWidget *w = static_cast<ConfigItem*>(lstBox->currentItem())->widget();
    if (w == NULL)
        return;
    QList<QTabWidget*> l = topLevelWidget()->findChildren<QTabWidget*>();
	if(l.isEmpty())
		return;
    QTabWidget *tab = l.first();
    if(tab == NULL)
        return;
    tab->setCurrentIndex(2);
}

void ConfigureDialog::repaintCurrent()
{
    QWidget *active = wnd->visibleWidget();
    if (active == NULL)
        return;
    active->repaint();
    Q3ListViewItem *item = findItem(active);
    if (item)
        lstBox->setCurrentItem(item);
    lstBox->repaint();
}

Q3ListViewItem *ConfigureDialog::findItem(QWidget *w)
{
    for (Q3ListViewItem *item = lstBox->firstChild(); item; item = item->nextSibling()){
        Q3ListViewItem *res = findItem(w, item);
        if (res)
            return res;
    }
    return NULL;
}

Q3ListViewItem *ConfigureDialog::findItem(QWidget *w, Q3ListViewItem *parent)
{
    if (static_cast<ConfigItem*>(parent)->m_widget == w)
        return parent;
    for (Q3ListViewItem *item = parent->firstChild(); item; item = item->nextSibling()){
        Q3ListViewItem *res = findItem(w, item);
        if (res)
            return res;
    }
    return NULL;
}

