/***************************************************************************
                          editfile.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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

#include "editfile.h"
#include "icons.h"
#include "mainwin.h"

#include <qlineedit.h>
#include <qpushbutton.h>
#include <qiconset.h>
#include <qlayout.h>
#if USE_KDE
#include <kfiledialog.h>
#define QFileDialog	KFileDialog
#else
#include <qfiledialog.h>
#endif

EditFile::EditFile(QWidget *p, const char *name)
        : QFrame(p, name)
{
    bDirMode = false;
    lay = new QHBoxLayout(this);
    edtFile = new QLineEdit(this);
    lay->addWidget(edtFile);
    lay->addSpacing(3);
    QPushButton *btnOpen = new QPushButton(this);
    lay->addWidget(btnOpen);
    btnOpen->setPixmap(Pict("fileopen"));
    connect(btnOpen, SIGNAL(clicked()), this, SLOT(showFiles()));
    connect(edtFile, SIGNAL(textChanged(const QString&)), this, SLOT(editTextChanged(const QString&)));
}

void EditFile::editTextChanged(const QString &str)
{
    emit textChanged(str);
}

void EditFile::setText(const QString &t)
{
    edtFile->setText(t);
}

QString EditFile::text()
{
    return edtFile->text();
}

void EditFile::showFiles()
{
    QString s = edtFile->text();
#if WIN32
    s.replace(QRegExp("\\\\"), "/");
#endif
    if (bDirMode){
        s = QFileDialog::getExistingDirectory(s, this,
                                              i18n("Directory for incoming files"));
    }else if (bSaveMode){
        s = QFileDialog::getSaveFileName(s, filter, this);
    }else{
        s = QFileDialog::getOpenFileName(s, filter, this);
    }
#if WIN32
    s.replace(QRegExp("/"), "\\");
#endif
    if (s.length()) edtFile->setText(s);
}

EditSound::EditSound(QWidget *p, const char *name)
        : EditFile(p, name)
{
    QPushButton *btnPlay = new QPushButton(this);
    lay->addSpacing(3);
    lay->addWidget(btnPlay);
    btnPlay->setPixmap(Pict("1rightarrow"));
    connect(btnPlay, SIGNAL(clicked()), this, SLOT(play()));
    filter = "Sounds (*.wav)";
}

void EditSound::play()
{
    pMain->playSound(edtFile->text().local8Bit());
}

#ifndef _WINDOWS
#include "editfile.moc"
#endif

