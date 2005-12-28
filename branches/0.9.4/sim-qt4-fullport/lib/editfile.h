/***************************************************************************
                          editfile.h  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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

#ifndef _EDITFILE_H
#define _EDITFILE_H

#include "simapi.h"

#include <QFrame>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDropEvent>

class QHBoxLayout;
class EditFile;
class FilePreview;
class QMenu;

class EXPORT FileLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    FileLineEdit(EditFile *p, const char *name = NULL);
    ~FileLineEdit();
protected:
    virtual void dragEnterEvent(QDragEnterEvent*);
    virtual void dropEvent(QDropEvent*);
};

typedef FilePreview *CreatePreview(QWidget *parent);

class EXPORT EditFile : public QFrame
{
    Q_OBJECT
public:
    EditFile(QWidget *p, const char *name=NULL);
    ~EditFile();
    void setText(const QString&);
    QString text();
    void setFilter(const QString &filter);
    void setDirMode(bool bMode) { bDirMode = bMode; }
    void setStartDir(const QString &dir);
    void setMultiplyMode(bool bMode) { bMultiplyMode = bMode; }
    void setFilePreview(CreatePreview*);
    void setTitle(const QString &title);
    void setReadOnly(bool);
    void setCreate(bool);
signals:
    void textChanged(const QString&);
public slots:
    void showFiles();
    void editTextChanged(const QString&);
protected:
    bool bIsImage;
    bool bDirMode;
    bool bMultiplyMode;
    bool bCreate;
    QString filter;
    QString startDir;
    QString title;
    QHBoxLayout   *lay;
    FileLineEdit  *edtFile;
    CreatePreview *createPreview;
};

class EXPORT EditSound : public EditFile
{
    Q_OBJECT
public:
    EditSound(QWidget *p, const char *name=NULL);
    ~EditSound();
protected slots:
    void play();
};

class EXPORT LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget *parent, const char *name = NULL);
    const char **helpList;
protected slots:
    void menuActivated(int);
protected:
    void mousePressEvent(QMouseEvent *e);
    QMenu *createPopupMenu();
};

class EXPORT MultiLineEdit : public QTextEdit
{
    Q_OBJECT
public:
    MultiLineEdit(QWidget *parent, const char *name = NULL);
    const char **helpList;
protected slots:
    void menuActivated(int);
protected:
    void mousePressEvent(QMouseEvent *e);
    QMenu *createPopupMenu();
};

#endif

