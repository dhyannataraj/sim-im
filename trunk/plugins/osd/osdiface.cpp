/***************************************************************************
                          osdiface.cpp  -  description
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

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include "fontedit.h"
#include "misc.h"
#include "qcolorbutton.h"

#include "osdiface.h"
#include "osd.h"

using namespace SIM;

OSDIface::OSDIface(QWidget *parent, void *d, OSDPlugin *plugin)
        : OSDIfaceBase(parent)
{
    m_plugin = plugin;
    OSDUserData *data = (OSDUserData*)d;
#ifndef WIN32
	chkFading->setChecked(false);
	chkFading->hide();
#endif
    cmbPos->insertItem(i18n("Left-bottom"));
    cmbPos->insertItem(i18n("Left-top"));
    cmbPos->insertItem(i18n("Right-bottom"));
    cmbPos->insertItem(i18n("Right-top"));
    cmbPos->insertItem(i18n("Center-bottom"));
    cmbPos->insertItem(i18n("Center-top"));
    cmbPos->insertItem(i18n("Center"));
    cmbPos->setCurrentItem(data->Position.toULong());
    spnOffs->setMinValue(0);
    spnOffs->setMaxValue(500);
    spnOffs->setValue(data->Offset.toULong());
    spnTimeout->setMinValue(1);
    spnTimeout->setMaxValue(60);
    spnTimeout->setValue(data->Timeout.toULong());
    btnColor->setColor(data->Color.toULong());
    if (data->Font.str().isEmpty()){
        edtFont->setFont(FontEdit::font2str(plugin->getBaseFont(font()), false));
    }else{
        edtFont->setFont(data->Font.str());
    }
    chkShadow->setChecked(data->Shadow.toBool());
	chkFading->setChecked(data->Fading.toBool());
    if (data->Background.toBool()){
        chkBackground->setChecked(true);
        btnBgColor->setColor(data->BgColor.toULong());
    }else{
        chkBackground->setChecked(false);
    }
    bgToggled(data->Background.toBool());
    connect(chkBackground, SIGNAL(toggled(bool)), this, SLOT(bgToggled(bool)));
    unsigned nScreens = screens();
    if (nScreens <= 1){
        lblScreen->hide();
        cmbScreen->hide();
    }else{
        for (unsigned i = 0; i < nScreens; i++)
            cmbScreen->insertItem(QString::number(i));
        unsigned curScreen = data->Screen.toULong();
        if (curScreen >= nScreens)
            curScreen = 0;
        cmbScreen->setCurrentItem(curScreen);
    }
}

void OSDIface::bgToggled(bool bState)
{
    if (bState){
        btnBgColor->setEnabled(true);
        return;
    }
    btnBgColor->setColor(colorGroup().base());
    btnBgColor->setEnabled(false);
}

void OSDIface::apply(void *d)
{
    OSDUserData *data = (OSDUserData*)d;
    data->Position.asULong() = cmbPos->currentItem();
    data->Offset.asULong()   = spnOffs->text().toULong();
    data->Timeout.asULong()  = spnTimeout->text().toULong();
    data->Color.asULong()    = btnColor->color().rgb();
    QString f = edtFont->getFont();
    QString base = FontEdit::font2str(m_plugin->getBaseFont(font()), false);
    if (f == base)
        f = "";
    data->Font.str() = f;
    data->Shadow.asBool() = chkShadow->isChecked();
	data->Fading.asBool() = chkFading->isChecked();
    data->Background.asBool() = chkBackground->isChecked();
    if (data->Background.toBool()){
        data->BgColor.asULong() = btnBgColor->color().rgb();
    }else{
        data->BgColor.asULong() = 0;
    }
    unsigned nScreens = screens();
    if (nScreens <= 1){
        data->Screen.asULong() = 0;
    }else{
        data->Screen.asULong() = cmbScreen->currentItem();
    }
}

#ifndef NO_MOC_INCLUDES
#include "osdiface.moc"
#endif

