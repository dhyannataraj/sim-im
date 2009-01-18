/***************************************************************************
                          spellcfg.cpp  -  description
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

#ifdef WIN32
#include "spellfind.h"
#endif

#include <qlabel.h>
#include <qpushbutton.h>
#include <q3header.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qstyle.h>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>

#include "editfile.h"
#include "linklabel.h"
#include "listview.h"
#include "log.h"

#include "spellcfg.h"
#include "spell.h"
#include "speller.h"

using namespace SIM;

const unsigned COL_NAME		= 0;
const unsigned COL_CHECK	= 1;
const unsigned COL_CHECKED	= 2;

SpellConfig::SpellConfig(QWidget *parent, SpellPlugin *plugin) : QWidget(parent)
{
	setupUi(this);
    m_plugin = plugin;
#ifdef WIN32
    edtPath->setText(m_plugin->getPath());
    edtPath->setFilter(i18n("ASpell(aspell.exe)"));
    lnkAspell->setUrl("http://aspell.net/win32/");
    lnkAspell->setText(i18n("Download ASpell"));
    m_find = NULL;
#else
    lblPath->hide();
    edtPath->hide();
#endif
    connect(edtPath, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(btnFind, SIGNAL(clicked()), this, SLOT(find()));
    connect(lstLang, SIGNAL(clickItem(Q3ListViewItem*)), this, SLOT(langClicked(Q3ListViewItem*)));
    lstLang->addColumn("");
    lstLang->addColumn("");
    lstLang->header()->hide();
    lstLang->setExpandingColumn(0);
    lstLang->adjustColumn();
    textChanged(edtPath->text());
}

SpellConfig::~SpellConfig()
{
#ifdef WIN32
    delete m_find;
#endif
}

void SpellConfig::apply()
{
#ifdef WIN32
    m_plugin->setPath(edtPath->text());
#endif
    QString lang;
    for (Q3ListViewItem *item = lstLang->firstChild(); item; item = item->nextSibling()){
        if (item->text(COL_CHECKED).isEmpty())
            continue;
        if (!lang.isEmpty())
            lang += ';';
        lang += item->text(COL_NAME);
    }
    m_plugin->setLang(lang);
    m_plugin->reset();
}

void SpellConfig::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    lstLang->adjustColumn();
}

#ifdef WIN32
void SpellConfig::textChanged(const QString &str)
#else
void SpellConfig::textChanged(const QString&)
#endif
{
    QString langs;
#ifdef WIN32
    if (str.isEmpty()){
        lnkAspell->show();
        btnFind->show();
    }else{
#endif
        lnkAspell->hide();
        btnFind->hide();
#ifdef WIN32
        SpellerBase base(str);
#else
        SpellerBase base(QString::null);
#endif
        SpellerConfig cfg(base);
        langs = cfg.getLangs();
#ifdef WIN32
    }
#endif
    lstLang->clear();
    if (langs.isEmpty()){
        lblLang->setEnabled(false);
        lstLang->setEnabled(false);
    }else{
        lblLang->setEnabled(true);
        lstLang->setEnabled(true);
        while (!langs.isEmpty()){
            QString l = SIM::getToken(langs, ';');
            bool bCheck = false;
            QString ll = m_plugin->getLang();
            while (!ll.isEmpty()){
                QString lc = SIM::getToken(ll, ';');
                if (l == lc){
                    bCheck = true;
                    break;
                }
            }
            Q3ListViewItem *item = new Q3ListViewItem(lstLang, l, "", bCheck ? "1" : "");
            setCheck(item);
        }
    }
}

void SpellConfig::find()
{
#ifdef WIN32
    if (m_find == NULL){
        m_find = new SpellFind(edtPath);
        connect(m_find, SIGNAL(finished()), this, SLOT(findFinished()));
    }
    raiseWindow(m_find);
#endif
}

void SpellConfig::findFinished()
{
#ifdef WIN32
    m_find = NULL;
#endif
}

void SpellConfig::langClicked(Q3ListViewItem *item)
{
    if(!item)
        return;
    SIM::log(SIM::L_DEBUG, "langClicked");
    if (item->text(COL_CHECKED).isEmpty()){
        item->setText(COL_CHECKED, "1");
    }else{
        item->setText(COL_CHECKED, "");
    }
    setCheck(item);
}

#define CHECK_OFF       QStyle::State_Off
#define CHECK_ON        QStyle::State_On
#define CHECK_NOCHANGE  QStyle::State_NoChange

void SpellConfig::setCheck(Q3ListViewItem *item)
{
    int state = item->text(COL_CHECKED).isEmpty() ? CHECK_OFF : CHECK_ON;
    QColorGroup cg = palette().active();
    int w = style()->pixelMetric(QStyle::PM_IndicatorWidth);
    int h = style()->pixelMetric(QStyle::PM_IndicatorHeight);
    QPixmap pixInd(w, h);
    QPainter pInd(&pixInd);
    pInd.setBrush(cg.background());
    QRect rc(0, 0, w, h);
    pInd.eraseRect(rc);
	log(L_DEBUG, "SpellConfig::setCheck FIXME!!!");
    //style().drawPrimitive(QStyle::PE_Indicator, &pInd, rc, cg, state);
    pInd.end();
    item->setPixmap(COL_CHECK, pixInd);
}

