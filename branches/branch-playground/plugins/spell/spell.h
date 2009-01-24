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
#include <Q3CString>

#include "cfg.h"
#include "event.h"
#include "misc.h"
#include "plugins.h"
#include "xevent.h"

using std::list;

struct SpellData
{
#ifdef WIN32
    SIM::Data	Path;
#endif
    SIM::Data	Lang;
};

class TextEdit;
class Q3SyntaxHighlighter;
class KDictSpellingHighlighter;
class SpellerBase;
class Speller;

typedef std::map<TextEdit*, Q3SyntaxHighlighter*>	MAP_EDITS;
typedef std::map<SIM::my_string, bool> MAP_BOOL;

class SpellPlugin : public QObject, public SIM::Plugin, public SIM::EventReceiver
{
    Q_OBJECT
public:
    SpellPlugin(unsigned, Buffer*);
    ~SpellPlugin();
#ifdef WIN32
    PROP_STR(Path);
#endif
    PROP_STR(Lang);
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
    virtual Q3CString getConfig();
    virtual QWidget *createConfigWindow(QWidget *parent);
    void activate();
    void deactivate();
    bool			m_bActive;
    SpellerBase		*m_base;
    list<Speller*>	m_spellers;
    SpellData data;
};

#endif

