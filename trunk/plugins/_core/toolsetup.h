/***************************************************************************
                          toolsetup.h  -  description
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

#ifndef _TOOLSETUP_H
#define _TOOLSETUP_H

#include <vector>

#include "toolsetupbase.h"

class Commands;

class ToolBarSetup : public ToolBarSetupBase
{
    Q_OBJECT
public:
    ToolBarSetup(Commands*, SIM::CommandsDef*);
    ~ToolBarSetup();
protected slots:
    void selectionChanged();
    void addClick();
    void removeClick();
    void upClick();
    void downClick();
    void applyClick();
    void okClick();
protected:
    SIM::CommandsDef *m_def;
    Commands	*m_bars;
    std::vector<unsigned> active;
    bool bDirty;
    void addButton(QListBox *lst, unsigned id);
    void setButtons();
    friend class Commands;
};

#endif

