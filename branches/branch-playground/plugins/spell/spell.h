/***************************************************************************
                          spell.h  -  description
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

#ifndef _SPELL_H
#define _SPELL_H

#include <qobject.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QEvent>
#include <QByteArray>

#include "cfg.h"
#include "event.h"
#include "misc.h"
#include "plugins.h"
#include "propertyhub.h"

using std::list;

class TextEdit;
class Q3SyntaxHighlighter;
class KDictSpellingHighlighter;
class SpellerBase;
class Speller;

typedef std::map<TextEdit*, Q3SyntaxHighlighter*>	MAP_EDITS;
typedef std::map<SIM::my_string, bool> MAP_BOOL;

class SpellPlugin : virtual public QObject, public SIM::Plugin, public SIM::EventReceiver, public SIM::PropertyHub
{
    Q_OBJECT
public:
    SpellPlugin(unsigned, Buffer*);
    ~SpellPlugin();
    MAP_EDITS m_edits;
    void reset();
    unsigned CmdSpell;
    QStringList suggestions(const QString &word);
    void add(const QString &word);
    MAP_BOOL m_ignore;
signals:
    void misspelling(const QString &word);
    void configChanged();
protected slots:
    void textEditFinished(TextEdit*);
    void check(const QString &word);
protected:
    bool eventFilter(QObject *o, QEvent *e);
    virtual bool processEvent(SIM::Event *e);
    virtual QByteArray getConfig();
    virtual QWidget *createConfigWindow(QWidget *parent);
    void activate();
    void deactivate();
    bool			m_bActive;
    SpellerBase		*m_base;
    list<Speller*>	m_spellers;
};

#endif

