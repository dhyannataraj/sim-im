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

#include <QDate>
#include <QWidget>

class QDateEdit;
class QPushButton;

class EXPORT DatePicker : public QWidget
{
    Q_OBJECT
public:
    DatePicker(QWidget *parent);
    ~DatePicker();

    QDate getDate() const;
public Q_SLOTS:
    void setDate(const QDate&);
Q_SIGNALS:
    void changed();
protected Q_SLOTS:
    void showPopup();
protected:
    void setEnabled(bool);
    QDateEdit	*m_edit;
    QPushButton	*m_button;
};

#endif

