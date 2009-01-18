/***************************************************************************
                          datepicker.h  -  description
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

#ifndef _DATEPICKER_H
#define _DATEPICKER_H

#include "simapi.h"

#include <q3frame.h>
#include <qlabel.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>

class QLineEdit;
class QPushButton;
class QSpinBox;

class PickerLabel;

class EXPORT DatePicker : public Q3Frame
{
    Q_OBJECT
public:
    DatePicker(QWidget *parent, const char *name = NULL);
    ~DatePicker();
    void setDate(QDate);
    QDate getDate();
    void setText(const QString&);
    QString text();
signals:
    void changed();
protected slots:
    void showPopup();
    void textChanged(const QString&);
protected:
    void setEnabled(bool);
    void paintEvent(QPaintEvent*);
    QLineEdit	*m_edit;
    QPushButton	*m_button;
};

class PickerPopup : public Q3Frame
{
    Q_OBJECT
public:
    PickerPopup(DatePicker *parent);
    ~PickerPopup();
protected slots:
    void monthChanged(int);
    void yearChanged(int);
    void dayClick(PickerLabel*);
protected:
    void fill();
    QSpinBox	*m_monthBox;
    QSpinBox	*m_yearBox;
    QLabel		**m_labels;
    DatePicker	*m_picker;
};

class PickerLabel : public QLabel
{
    Q_OBJECT
public:
    PickerLabel(QWidget *parent);
signals:
    void clicked(PickerLabel*);
protected:
    void mouseReleaseEvent(QMouseEvent*);
};

#endif

