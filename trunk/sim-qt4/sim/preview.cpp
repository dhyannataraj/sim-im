/***************************************************************************
 *                         preview.cpp  -  description
 *                         -------------------
 *                         begin                : Sun Mar 24 2002
 *                         copyright            : (C) 2002 by Vladimir Shutoff
 *                         email                : vovan@shutoff.ru
 ****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "preview.h"
#ifdef USE_KDE
#include <kpreviewwidgetbase.h>
#include <kurl.h>
#else
#include <q3filedialog.h>
#endif

#include <qlabel.h>
#include <qlayout.h>
#include <qimage.h>
#include <qfile.h>
//Added by qt3to4:
#include <QPixmap>
#include <QVBoxLayout>

FilePreview::FilePreview(QWidget *parent)
#ifdef USE_KDE
        : KPreviewWidgetBase(parent)
#else
        : QWidget(parent)
#endif
{
}

FilePreview::~FilePreview()
{
}

#ifdef USE_KDE

void FilePreview::showPreview(const KURL &url)
{
    if (!url.isLocalFile()){
        showPreview(NULL);
        return;
    }
    QString fileName = url.directory(false, false);
    if (!fileName.isEmpty() && (fileName[fileName.length() - 1] != '/'))
        fileName += '/';
    fileName += url.fileName(false);
    showPreview((const char*)(QFile::encodeName(fileName)));
}

void FilePreview::clearPreview()
{
    showPreview(NULL);
}

#else

void FilePreview::previewUrl(const Q3Url &url)
{
    if (!url.isLocalFile()){
        showPreview(NULL);
        return;
    }
    QString fileName = url.toString(false, false);
    showPreview(QFile::encodeName(fileName));
}

#endif

#ifndef USE_KDE

PictPreview::PictPreview(QWidget *parent)
        : FilePreview(parent)
{
    label = new QLabel(this);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setMinimumSize(QSize(70, 70));
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->addWidget(label);
}

void PictPreview::showPreview(const char *file)
{
    if (file == NULL){
        label->setPixmap(QPixmap());
        return;
    }
    QImage img(QFile::decodeName(file));
    if ((img.width() > label->width()) || (img.height() > label->height())){
        bool bOk = false;
        if (img.width() > label->width()){
            int h = img.height() * label->width() / img.width();
            if (h <= label->height()){
                img = img.smoothScale(label->width(), h);
                bOk = true;
            }
        }
        if (!bOk){
            int w = img.width() * label->height() / img.height();
            img = img.smoothScale(w, label->height());
        }
    }
    QPixmap pict;
    pict.fromImage(img);
    label->setPixmap(pict);
}

#endif

