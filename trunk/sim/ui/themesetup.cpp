/***************************************************************************
                           themesetup.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : shutoff@mail.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "themesetup.h"
#include "mainwin.h"
#include "themes.h"
#include "icons.h"
#include "transparent.h"
#include "ballonmsg.h"
#include "splash.h"
#include "editfile.h"
#include "enable.h"
#include "qcolorbutton.h"
#include "log.h"

#include <qlistbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qimage.h>

#ifdef WIN32

typedef struct language
{
    const char *code;
    const char *name;
} language;

static language langs[] =
    {
        { "", I18N_NOOP("System") },
        { "-", I18N_NOOP("English") },
        { "cs", I18N_NOOP("Czech") },
        { "de", I18N_NOOP("German") },
        { "es", I18N_NOOP("Spanish") },
        { "it", I18N_NOOP("Italian") },
        { "nl", I18N_NOOP("Dutch") },
        { "po", I18N_NOOP("Polish") },
        { "ru", I18N_NOOP("Russian") },
        { "tr", I18N_NOOP("Turkish") },
        { "uk", I18N_NOOP("Ukrainian") },
        { "zh_TW", I18N_NOOP("Chinese") },
        { NULL, NULL }
    };

#endif

const char *app_file(const char *f);

ThemeSetup::ThemeSetup(QWidget *parent)
        : ThemeSetupBase(parent)
{
    lstThemes->clear();
    pMain->themes->fillList(lstThemes);
    for (unsigned i = 0; i < lstThemes->count(); i++){
        if (lstThemes->text(i) != pMain->themes->getTheme()) continue;
        lstThemes->setCurrentItem(i);
        break;
    }
    connect(btnAddIcons, SIGNAL(clicked()), this, SLOT(addIcons()));
    connect(lstThemes, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(apply(QListBoxItem*)));
    connect(lstIcons, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(apply(QListBoxItem*)));
    connect(chkSystemColors, SIGNAL(toggled(bool)), this, SLOT(sysColorsToggled(bool)));
    if (TransparentTop::bCanTransparent){
        chkTransparent->setChecked(pMain->UseTransparent);
        sldTransparent->setMinValue(0);
        sldTransparent->setMaxValue(100);
        sldTransparent->setValue(pMain->Transparent);
        sldTransparent->setTickmarks(QSlider::Below);
        sldTransparent->setTickInterval(5);
        connect(chkTransparent, SIGNAL(toggled(bool)), this, SLOT(checkedTransparent(bool)));
        chkTransparentContainer->setChecked(pMain->UseTransparentContainer);
        sldTransparentContainer->setMinValue(0);
        sldTransparentContainer->setMaxValue(100);
        sldTransparentContainer->setValue(pMain->TransparentContainer);
        sldTransparentContainer->setTickmarks(QSlider::Below);
        sldTransparentContainer->setTickInterval(5);
        connect(chkTransparentContainer, SIGNAL(toggled(bool)), this, SLOT(checkedTransparent(bool)));
        checkedTransparent(pMain->UseTransparent);
    }else{
        tabWnd->setCurrentPage(3);
        tabWnd->removePage(tabWnd->currentPage());
        tabWnd->setCurrentPage(0);
    }
    QStrList formats = QImageIO::inputFormats();
    QString format;
    QStrListIterator it(formats);
    char *fmt;
    while ((fmt = ++it) != NULL){
        if (format.length())
#ifdef USE_KDE
            format += " ";
#else
            format += ";";
#endif
        QString f = fmt;
        f = f.lower();
        format += "*." + f;
        if (f == "jpeg")
#ifdef USE_KDE
            format += " *.jpg";
#else
            format += ";*.jpg";
#endif
    }
#ifdef USE_KDE
    edtBg->setFilter(i18n("%1|Graphics") .arg(format));
#else
    edtBg->setFilter(i18n("Graphics(%1)") .arg(format));
#endif
    edtBg->setStartDir(app_file("pict"));
    edtBg->setText(QString::fromLocal8Bit(pMain->BackgroundFile.c_str()));
    cmbPos->clear();
    cmbPos->insertItem("Contact - left");
    cmbPos->insertItem("Contact - scale");
    cmbPos->insertItem("Window - left top");
    cmbPos->insertItem("Window - left bottom");
    cmbPos->insertItem("Window - left center");
    cmbPos->insertItem("Window - scale");
    cmbPos->setCurrentItem(pMain->BackgroundMode);
    spnMargin->setMaxValue(20);
    spnMargin->setValue(pMain->IconMargin);
    int h = lstThemes->itemHeight();
    lstThemes->setMinimumSize(QSize(0, h * 3));
    lstIcons->setMinimumSize(QSize(0, h * 3));
    connect(pMain, SIGNAL(setupInit()), this, SLOT(setupInit()));
#if defined(USE_KDE) || defined(WIN32)
    chkUserWnd->setChecked(pMain->UserWindowInTaskManager);
    chkMainWnd->setChecked(pMain->MainWindowInTaskManager);
#else
    chkUserWnd->hide();
    chkMainWnd->hide();
#endif
#ifdef WIN32
    chkInactive->setChecked(pMain->TransparentIfInactive);
#else
    chkInactive->hide();
#endif
#ifdef WIN32
    int n = 0;
    for (const language *l = langs; l->code; l++, n++){
        cmbLang->insertItem(i18n(l->name));
        if (!strcmp(pSplash->Language.c_str(), l->code))
            cmbLang->setCurrentItem(n);
    }
    bLangChanged = false;
    connect(cmbLang, SIGNAL(activated(int)), this, SLOT(langChanged(int)));
#else
    lblLang->hide();
    cmbLang->hide();
#endif
    chkDock->setChecked(pMain->UseDock);
    chkSplash->setChecked(pSplash->Show);
    chkEmotional->setChecked(pMain->UseEmotional);
    chkUserWndOnTop->setChecked(pMain->UserWndOnTop);
    chkSystemColors->setChecked(pMain->UseSystemColors);
    sysColorsToggled(pMain->UseSystemColors);
    if (pMain->UseSystemColors){
        btnOnlineColor->setColor(colorGroup().text());
        btnOfflineColor->setColor(colorGroup().dark());
    }else{
        btnOnlineColor->setColor(pMain->OnlineColor);
        btnOfflineColor->setColor(pMain->OfflineColor);
    }
    setupInit();
}

const char *app_file(const char *f);

void ThemeSetup::langChanged(int)
{
#ifdef WIN32
    if (bLangChanged) return;
    int n = cmbLang->currentItem();
    for (const language *l = langs; l->code; l++){
        if (n-- > 0) continue;
        break;
    }
    if (l->code && !strcmp(l->code, pSplash->Language.c_str())) return;
    bLangChanged = true;
    BalloonMsg::message(i18n("Need restart SIM for change language"), cmbLang);
#endif
}

void ThemeSetup::setupInit()
{
    QDir icons(QString::fromLocal8Bit(app_file("icons")));
    QStringList lst = icons.entryList("*.dll");
    unsigned i = 1;
    lstIcons->clear();
    lstIcons->insertItem(i18n("Default icons"));
    lstIcons->setCurrentItem(0);
    for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it, i++){
        QString s = *it;
        s.replace(QRegExp(".dll$"), "");
        lstIcons->insertItem(s);
        if (s == QString::fromLocal8Bit(pMain->Icons.c_str()))
            lstIcons->setCurrentItem(i);
    }
};

void ThemeSetup::addIcons()
{
    pMain->goURL("http://sim-icq.sourceforge.net/icons.shtml");
}

void ThemeSetup::checkedTransparent(bool)
{
    sldTransparent->setEnabled(chkTransparent->isChecked());
    lblTransparent->setEnabled(chkTransparent->isChecked());
    sldTransparentContainer->setEnabled(chkTransparentContainer->isChecked());
    lblTransparentContainer->setEnabled(chkTransparentContainer->isChecked());
}

void ThemeSetup::apply(QListBoxItem*)
{
    apply((ICQUser*)NULL);
}

void ThemeSetup::apply(ICQUser*)
{
    pMain->themes->setTheme(lstThemes->currentText());
    QString iconsName = lstIcons->currentItem() ? lstIcons->currentText() : QString("");
    if (QString::fromLocal8Bit(pMain->Icons.c_str()) != iconsName){
        if (iconsName.isEmpty()){
            pMain->Icons = "";
        }else{
            pMain->Icons = iconsName.local8Bit();
        }
        pMain->changeIcons(0);
    }
    bool bColorChanged = false;
    if (chkSystemColors->isChecked()){
        if (!pMain->UseSystemColors) bColorChanged = true;
        pMain->UseSystemColors = true;
    }else{
        if (pMain->UseSystemColors) bColorChanged = true;
        pMain->UseSystemColors = false;
        if (pMain->OnlineColor != btnOnlineColor->color().rgb()){
            bColorChanged = true;
            pMain->OnlineColor = btnOnlineColor->color().rgb();
        }
        if (pMain->OfflineColor != btnOfflineColor->color().rgb()){
            bColorChanged = true;
            pMain->OfflineColor = btnOfflineColor->color().rgb();
        }
    }
    if ((edtBg->text() != QString::fromLocal8Bit(pMain->BackgroundFile.c_str())) ||
            (cmbPos->currentItem() != pMain->BackgroundMode) ||
            (atol(spnMargin->text().latin1()) != pMain->IconMargin) || bColorChanged){
        pMain->BackgroundMode = cmbPos->currentItem();
        set(pMain->BackgroundFile, edtBg->text());
        pMain->IconMargin = atol(spnMargin->text().latin1());
        pMain->changeBackground();
    }
    if (TransparentTop::bCanTransparent){
#ifdef WIN32
        pMain->TransparentIfInactive = chkInactive->isChecked();
#endif
        pMain->UseTransparent = chkTransparent->isChecked();
        pMain->Transparent = sldTransparent->value();
        pMain->UseTransparentContainer = chkTransparentContainer->isChecked();
        pMain->TransparentContainer = sldTransparentContainer->value();
        pMain->changeTransparent();
    }
    pSplash->Show = chkSplash->isChecked();
    pMain->UseEmotional = chkEmotional->isChecked();
    if (chkUserWndOnTop->isChecked() != pMain->UserWndOnTop){
        pMain->UserWndOnTop = chkUserWndOnTop->isChecked();
        pMain->setUserBoxOnTop();
    }
#if defined(USE_KDE) || defined(WIN32)
    bool bChange = false;
    if (pMain->UserWindowInTaskManager != chkUserWnd->isChecked()){
        pMain->UserWindowInTaskManager = chkUserWnd->isChecked();
        bChange = true;
    }
    if (pMain->MainWindowInTaskManager != chkMainWnd->isChecked()){
        pMain->MainWindowInTaskManager = chkMainWnd->isChecked();
        bChange = true;
    }
    if (bChange) pMain->changeWm();
#endif
    if (pMain->UseDock != chkDock->isChecked()){
        pMain->UseDock = chkDock->isChecked();
        pMain->setDock();
    }
#ifdef WIN32
    int n = cmbLang->currentItem();
    for (const language *l = langs; l->code; l++){
        if (n-- > 0) continue;
        pSplash->Language = l->code;
        break;
    }
    pSplash->save();
#endif
}

void ThemeSetup::sysColorsToggled(bool bState)
{
    lblOnline->setEnabled(!bState);
    lblOffline->setEnabled(!bState);
    btnOnlineColor->setEnabled(!bState);
    btnOfflineColor->setEnabled(!bState);
}

#ifndef _WINDOWS
#include "themesetup.moc"
#endif

