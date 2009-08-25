/***************************************************************************
                          shortcutcfg.cpp  -  description
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

#include "shortcutcfg.h"
#include "shortcuts.h"
#include "qkeybutton.h"

#include "mousecfg.h"
#include "core.h"
#include "core_consts.h"
#include "cmddef.h"

#include <q3listview.h>
#include <q3accel.h>

#include <QResizeEvent>
#include <QLabel>
#include <QRegExp>
#include <QPushButton>
#include <QCheckBox>
#include <QTabWidget>

using namespace SIM;

ShortcutsConfig::ShortcutsConfig(QWidget *parent, ShortcutsPlugin *plugin) : QWidget(parent)
{
	setupUi(this);
    m_plugin = plugin;
    lstKeys->setSorting(0);
    loadMenu(MenuMain, true);
    loadMenu(MenuGroup, false);
    loadMenu(MenuContact, false);
    loadMenu(MenuStatus, true);
    adjustColumns();
    selectionChanged();
    connect(lstKeys, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(edtKey, SIGNAL(changed()), this, SLOT(keyChanged()));
    connect(btnClear, SIGNAL(clicked()), this, SLOT(keyClear()));
    connect(chkGlobal, SIGNAL(toggled(bool)), this, SLOT(globalChanged(bool)));
    for (QObject *p = parent; p != NULL; p = p->parent()){
        if (!p->inherits("QTabWidget"))
            continue;
        QTabWidget *tab = static_cast<QTabWidget*>(p);
        mouse_cfg = new MouseConfig(tab, plugin);
        tab->addTab(mouse_cfg, i18n("Mouse"));
        break;
    }
}

ShortcutsConfig::~ShortcutsConfig()
{
}

void ShortcutsConfig::loadMenu(unsigned long id, bool bCanGlobal)
{
    EventMenuGetDef eMenu(id);
    eMenu.process();
    CommandsDef *def = eMenu.defs();
    if (def)
	{
        CommandsList list(*def, true);
        CommandDef *s;
        while ((s = ++list) != NULL){
            if ((s->id == 0) || s->popup_id || (s->flags & COMMAND_TITLE))
                continue;
            QString title = i18n(s->text);
            if (title == "_")
                continue;
            title = title.remove('&');
            QString accel;
            int key = 0;
            const QString cfg_accel = m_plugin->property("Key").toMap().value(QString::number(s->id)).toString();
            if (!cfg_accel.isEmpty())
                key = Q3Accel::stringToKey(cfg_accel);
            if ((key == 0) && !s->accel.isEmpty())
                key = Q3Accel::stringToKey(i18n(s->accel));
            if (key)
                accel = Q3Accel::keyToString(key);
            QString global;
            bool bGlobal = m_plugin->getOldGlobal(s);
            const QString cfg_global = m_plugin->property("Global").toMap().value(QString::number(s->id)).toString();
            if (!cfg_global.isEmpty())
                bGlobal = !bGlobal;
            if (bGlobal)
                global = i18n("Global");
            Q3ListViewItem *item;
            for (item = lstKeys->firstChild(); item; item = item->nextSibling()){
                if (item->text(3).toUInt() == s->id)
                    break;
            }
            if (item == NULL)
                new Q3ListViewItem(lstKeys,
                                  title, accel, global,
                                  QString::number(s->id), bCanGlobal ? "1" : "");
        }
    }
}

void ShortcutsConfig::apply()
{
    mouse_cfg->apply();
    saveMenu(MenuMain);
    saveMenu(MenuGroup);
    saveMenu(MenuContact);
    saveMenu(MenuStatus);
    m_plugin->releaseKeys();
    m_plugin->applyKeys();
}

void ShortcutsConfig::saveMenu(unsigned long id)
{
    EventMenuGetDef eMenu(id);
    eMenu.process();
    CommandsDef *def = eMenu.defs();
    if (def){
        CommandsList list(*def, true);
        CommandDef *s;
        while ((s = ++list) != NULL){
            if ((s->id == 0) || s->popup_id)
                continue;
            for (Q3ListViewItem *item = lstKeys->firstChild(); item; item = item->nextSibling()){
                if (item->text(3).toUInt() != s->id) continue;
                int key = Q3Accel::stringToKey(item->text(1));
                const QString cfg_key = m_plugin->getOldKey(s);
                if (key == Q3Accel::stringToKey(cfg_key)){
					QVariantMap map;
					map = m_plugin->property("Key").toMap();
					map.remove(QString::number(s->id));
					m_plugin->setProperty("Key", map);
                }else{
                    QString t = item->text(1);
                    if (t.isEmpty())
                        t = "-";
					QVariantMap map;
					map = m_plugin->property("Key").toMap();
					map.insert(QString::number(s->id), t);
					m_plugin->setProperty("Key", map);
                }
                bool bGlobal = !item->text(2).isEmpty();
                bool bCfgGlobal = m_plugin->getOldGlobal(s);
                if (item->text(1).isEmpty()){
                    bGlobal = false;
                    bCfgGlobal = false;
                }
                if (bGlobal == bCfgGlobal){
					QVariantMap map;
					map = m_plugin->property("Global").toMap();
					map.remove(QString::number(s->id));
					m_plugin->setProperty("Global", map);
                }else{
					QVariantMap map;
					map = m_plugin->property("Global").toMap();
					map.insert(QString::number(s->id), bGlobal ? "1" : "-1");
					m_plugin->setProperty("Global", map);
                }
            }
        }
    }
}

void ShortcutsConfig::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    adjustColumns();
}

void ShortcutsConfig::adjustColumns()
{
    QScrollBar *bar = lstKeys->verticalScrollBar();
    int wScroll = 0;
    if (bar && bar->isVisible())
        wScroll = bar->width();
    lstKeys->setColumnWidth(0, lstKeys->width() -
                            lstKeys->columnWidth(2) - lstKeys->columnWidth(1) - 4 - wScroll);
}

void ShortcutsConfig::selectionChanged()
{
    Q3ListViewItem *item = lstKeys->currentItem();
    if (item == NULL){
        lblKey->setText(QString::null);
        edtKey->setEnabled(false);
        btnClear->setEnabled(false);
        chkGlobal->setEnabled(false);
        return;
    }
    lblKey->setText(item->text(0));
    edtKey->setEnabled(true);
    btnClear->setEnabled(true);
    edtKey->setText(item->text(1));
    if (!item->text(1).isEmpty() && !item->text(4).isEmpty()){
        chkGlobal->setEnabled(true);
        chkGlobal->setChecked(!item->text(2).isEmpty());
    }else{
        chkGlobal->setEnabled(false);
        chkGlobal->setChecked(false);
    }
}

void ShortcutsConfig::keyClear()
{
    Q3ListViewItem *item = lstKeys->currentItem();
    if (item == NULL)
        return;
    item->setText(1, QString::null);
    edtKey->setText(QString::null);
    edtKey->clearFocus();
}

void ShortcutsConfig::keyChanged()
{
    Q3ListViewItem *item = lstKeys->currentItem();
    if (item == NULL)
        return;
    QString key = edtKey->text();
    if (key.isEmpty() || item->text(4).isEmpty()){
        chkGlobal->setChecked(false);
        chkGlobal->setEnabled(false);
    }else{
        chkGlobal->setEnabled(true);
    }
    item->setText(1, key);
    edtKey->clearFocus();
}

void ShortcutsConfig::globalChanged(bool)
{
    Q3ListViewItem *item = lstKeys->currentItem();
    if ((item == NULL) || item->text(4).isEmpty())
        return;
    item->setText(2, chkGlobal->isChecked() ? i18n("Global") : QString::null);
}

