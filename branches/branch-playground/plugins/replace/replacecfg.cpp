/***************************************************************************
                          replacecfg.cpp  -  description
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

#include "misc.h"

#include "replacecfg.h"
#include "replace.h"
#include "listview.h"
#include "intedit.h"

#include <q3frame.h>
#include <qlayout.h>
#include <q3header.h>
#include <QResizeEvent>
#include <QEvent>
#include <QKeyEvent>

ReplaceCfg::ReplaceCfg(QWidget *parent, ReplacePlugin *plugin) : QWidget(parent)
{
	setupUi(this);
    m_count = 0;
    m_plugin = plugin;
    lstKeys->addColumn(i18n("You type"));
    lstKeys->addColumn(i18n("You send"));
    lstKeys->setExpandingColumn(1);
    lstKeys->setSorting(2);
    for (unsigned i = 1; i <= m_plugin->getKeys(); i++){
        QString key = m_plugin->getKey(i);
        QString value = m_plugin->getValue(i);
        if (key.isEmpty())
            continue;
        new Q3ListViewItem(lstKeys, key, value, QString::number(m_count++));
    }
    new Q3ListViewItem(lstKeys, "", "", QString::number(m_count++));
    lstKeys->adjustColumn();
    m_edit = new IntLineEdit(lstKeys->viewport());
    m_edit->installEventFilter(this);
    lstKeys->setFocusProxy(m_edit);
    m_col = 0;
    lstKeys->setCurrentItem(lstKeys->firstChild());
    m_editItem = NULL;
    m_editCol  = (unsigned)(-1);
    m_bDelete = false;
    setEdit();
    connect(lstKeys, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(lstKeys->header(), SIGNAL(sizeChange(int,int,int)), this, SLOT(sizeChange(int,int,int)));
    connect(lstKeys, SIGNAL(mouseButtonPressed(int, Q3ListViewItem*, const QPoint&, int)), this, SLOT(mouseButtonPressed(int, Q3ListViewItem*, const QPoint&, int)));
}

ReplaceCfg::~ReplaceCfg()
{
}

void ReplaceCfg::apply()
{
    if (m_editItem)
        m_editItem->setText(m_editCol, m_edit->text());
    unsigned n = 0;
    for (Q3ListViewItem *item = lstKeys->firstChild(); item; item = item->nextSibling()){
        if (item->text(0).isEmpty())
            continue;
        n++;
        m_plugin->setKey(n, item->text(0));
        m_plugin->setValue(n, item->text(1));
    }
    m_plugin->setKeys(n);
}

void ReplaceCfg::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    lstKeys->adjustColumn();
}

bool ReplaceCfg::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress){
        QKeyEvent *ke = (QKeyEvent*)e;
        if ((ke->key() == Qt::Key_Right) && (m_col == 0)){
            if (!m_edit->hasMarkedText() && (m_edit->cursorPosition() == (int)m_edit->text().length())){
                m_col = 1;
                setEdit();
                return true;
            }
        }
        if ((ke->key() == Qt::Key_Left) && (m_col == 1)){
            if (!m_edit->hasMarkedText() && (m_edit->cursorPosition() == 0)){
                m_col = 0;
                setEdit();
                return true;
            }
        }
        if ((ke->key() == Qt::Key_Enter) || (ke->key() == Qt::Key_Return)){
            QString text = m_edit->text();
            flush();
            if ((m_col == 0) && !text.isEmpty())
                m_col = 1;
            setEdit();
            return true;
        }
        if (ke->key() == Qt::Key_Escape){
            m_edit->setText(m_editItem->text(m_col));
            m_edit->setSelection(0, m_edit->text().length());
            return true;
        }
    }
    return QWidget::eventFilter(o, e);
}

void ReplaceCfg::flush()
{
    if (m_editItem == NULL)
        return;
    if (m_edit->text().isEmpty()){
        if ((m_editCol == 0) && !m_editItem->text(0).isEmpty()){
            m_bDelete = true;
            delete m_editItem;
            m_editItem = NULL;
            m_bDelete = false;
        }
        return;
    }
    if ((m_editCol == 0) && m_editItem->text(0).isEmpty())
        new Q3ListViewItem(lstKeys, "", "", QString::number(m_count++));
    m_editItem->setText(m_editCol, m_edit->text());
}

void ReplaceCfg::setEdit()
{
    Q3ListViewItem *item = lstKeys->currentItem();
    if (item == NULL){
        m_edit->hide();
    }else{
        if ((m_editItem != item) || (m_col != m_editCol)){
            QString text = item->text(m_col);
            flush();
            m_edit->setText(text);
            m_edit->setSelection(0, m_edit->text().length());
            m_editCol = m_col;
            m_editItem = item;
        }
        QRect rc = lstKeys->itemRect(item);
        if (m_col){
            rc.setLeft(lstKeys->columnWidth(0));
            rc.setWidth(lstKeys->columnWidth(1));
        }else{
            rc.setWidth(lstKeys->columnWidth(0));
        }
        m_edit->move(rc.left(), rc.top());
        m_edit->resize(rc.width(), rc.height());
        m_edit->show();
    }
}

void ReplaceCfg::selectionChanged()
{
    if (m_bDelete)
        return;
    setEdit();
}

void ReplaceCfg::sizeChange(int,int,int)
{
    setEdit();
}

void ReplaceCfg::mouseButtonPressed(int, Q3ListViewItem *item, const QPoint&, int col)
{
    if (item){
        m_col = col;
        setEdit();
    }
}

