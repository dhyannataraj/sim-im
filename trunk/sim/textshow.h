/***************************************************************************
                          textshow.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
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

#ifndef _TEXTSHOW_H
#define _TEXTSHOW_H

#include "sim_export.h"
#include "event.h"

#include <qglobal.h>
#include <qmainwindow.h>
#include <qtoolbutton.h>
#include <qlabel.h>

#include <qtextedit.h>
#ifdef USE_KDE
#include <ktextedit.h>
#define QTextEdit KTextEdit
#endif

class CToolBar;
class QTextDrag;

const unsigned TextCmdBase	= 0x00030000;
const unsigned CmdBgColor	= TextCmdBase;
const unsigned CmdFgColor	= TextCmdBase + 1;
const unsigned CmdBold		= TextCmdBase + 2;
const unsigned CmdItalic	= TextCmdBase + 3;
const unsigned CmdUnderline	= TextCmdBase + 4;
const unsigned CmdFont		= TextCmdBase + 5;

class EXPORT TextShow : public QTextEdit
{
    Q_OBJECT
public:
    TextShow (QWidget *parent, const char *name=NULL);
    ~TextShow();
    virtual void setSource(const QString &url);
    const QColor &background() const;
    const QColor &foreground() const;
    void setForeground(const QColor&);
    void setBackground(const QColor&);
signals:
    void finished();
protected:
    void emitLinkClicked(const QString&);
    bool linksEnabled() const { return true; }
};

class EXPORT TextEdit : public TextShow, public SIM::EventReceiver
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent, const char *name = NULL);
    ~TextEdit();
    void setCtrlMode(bool);
    void setTextFormat(QTextEdit::TextFormat);
    const QColor &foreground() const;
    const QColor &defForeground() const;
    void setForeground(const QColor&, bool bDef);
    void setParam(void*);
    void setFont(const QFont&);
    bool isEmpty();
    QPoint m_popupPos;
signals:
    void ctrlEnterPressed();
    void lostFocus();
    void emptyChanged(bool);
    void colorsChanged();
    void fontSelected(const QFont &font);
    void finished(TextEdit*);
protected slots:
    void slotClicked(int parag, int index);
    void slotTextChanged();
    void slotColorChanged(const QColor &c);
    void bgColorChanged(QColor c);
    void fgColorChanged(QColor c);
    void fontChanged(const QFont &f);
protected:
    virtual bool processEvent(SIM::Event *e);
    virtual void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    QPopupMenu *createPopupMenu(const QPoint& pos);
    void *m_param;
    bool m_bBold;
    bool m_bItalic;
    bool m_bUnderline;
    bool m_bChanged;
    QColor curFG;
    QColor defFG;
    bool m_bCtrlMode;
    bool m_bEmpty;
    bool m_bSelected;
    bool m_bNoSelected;
    bool m_bInClick;
};

class QToolBar;

class EXPORT ColorLabel : public QLabel
{
    Q_OBJECT
public:
    ColorLabel(QWidget *parent, QColor c, int id, const QString&);
signals:
    void selected(int);
protected:
    void mouseReleaseEvent(QMouseEvent*);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    unsigned m_id;
};

class EXPORT ColorPopup : public QFrame
{
    Q_OBJECT
public:
    ColorPopup(QWidget *parent, QColor c);
signals:
    void colorChanged(QColor color);
protected slots:
    void colorSelected(int);
protected:
    QColor m_color;
};

class EXPORT RichTextEdit : public QMainWindow
{
    Q_OBJECT
public:
    RichTextEdit(QWidget *parent, const char *name = NULL);
    void setText(const QString&);
    QString text();
    void setTextFormat(QTextEdit::TextFormat);
    QTextEdit::TextFormat textFormat();
    void setReadOnly(bool bState);
    void showBar();
protected:
    TextEdit	*m_edit;
    CToolBar	*m_bar;
};

#endif
