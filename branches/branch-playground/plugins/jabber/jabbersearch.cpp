/***************************************************************************
                          jabbersearch.cpp  -  description
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

#include <vector>

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3multilineedit.h>
#include <qcombobox.h>
#include <qtimer.h>
#include <qtabwidget.h>
#include <q3widgetstack.h>
#include <qobject.h>
#include <qregexp.h>
#include <qcheckbox.h>
#include <q3groupbox.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3GridLayout>
#include <QPixmap>
#include <Q3VBoxLayout>

#include "ballonmsg.h"
#include "icons.h"
#include "misc.h"

#include "jabberclient.h"
#include "jabbersearch.h"
#include "jabber.h"
#include "jidadvsearch.h"

using namespace std;
using namespace SIM;

class CComboBox : public QComboBox
{
public:
    CComboBox(QWidget *parent, const char *name);
    void addItem(const QString &label, const QString &value);
    QString value();
protected:
    vector<QString> m_values;
};

CComboBox::CComboBox(QWidget *parent, const char *name)
        : QComboBox(parent, name)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

void CComboBox::addItem(const QString &label, const QString &value)
{
    m_values.push_back(value);
    insertItem(label);
}

QString CComboBox::value()
{
    unsigned index = currentItem();
    if (index >= m_values.size())
        return QString::null;
    return m_values[index];
}

const unsigned MAX_ELEMENTS = 8;

JabberSearch::JabberSearch(QWidget *parent, const char *name)
        : QChildWidget(parent, name)
{
}

void JabberSearch::init(QWidget *receiver, JabberClient *client, const QString &jid, const QString &node, const QString &name, bool bRegister)
{
    m_client    = client;
    m_jid       = jid;
    m_node      = node;
    m_name	= name;
    m_receiver	= receiver;
    m_bXData	= false;
    m_bFirst	= true;
    m_bRegister = bRegister;
    m_bDirty = false;
}

struct defFlds
{
    const char  *tag;
    const char  *name;
    bool        bRequired;
};

static defFlds fields[] =
    {
        { "username", I18N_NOOP("Username"), true },
        { "nick", I18N_NOOP("Nick"), false },
        { "email", I18N_NOOP("EMail"), false },
        { "first", I18N_NOOP("First Name"), false },
        { "last", I18N_NOOP("Last Name"), false },
        { "age_min", I18N_NOOP("Age min"), false },
        { "age_max", I18N_NOOP("Age max"), false },
        { "city", I18N_NOOP("City"), false },
        { NULL, NULL, false }
    };

void JabberSearch::addWidget(JabberAgentInfo *data)
{
    QWidget *widget = NULL;
    bool bJoin = false;
    if (!data->Type.str().isEmpty()){
        if (data->Type.str() == "x"){
            m_bXData = true;
            vector<QWidget*>::iterator it;
            for (it = m_widgets.begin(); it != m_widgets.end(); ++it)
                if (*it)
                    delete (*it);
            m_widgets.clear();
            for (it = m_labels.begin(); it != m_labels.end(); ++it)
                if (*it)
                    delete (*it);
            m_labels.clear();
            for (it = m_descs.begin(); it != m_descs.end(); ++it)
                if (*it)
                    delete (*it);
            m_descs.clear();
            m_instruction = QString::null;
        }else if (data->Type.str() == "title"){
            if (!data->Value.str().isEmpty())
                m_title = data->Value.str();
        }else if (data->Type.str() == "text-single"){
            widget = new QLineEdit(this, data->Field.str());
            connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
            connect(widget, SIGNAL(textChanged(const QString&)), m_receiver, SLOT(textChanged(const QString&)));
            if (!data->Value.str().isEmpty())
                static_cast<QLineEdit*>(widget)->setText(data->Value.str());
        }else if (data->Type.str() == "text-private"){
            widget = new QLineEdit(this, data->Field.str());
            static_cast<QLineEdit*>(widget)->setEchoMode(QLineEdit::Password);
            connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
            connect(widget, SIGNAL(textChanged(const QString&)), m_receiver, SLOT(textChanged(const QString&)));
            if (!data->Value.str().isEmpty())
                static_cast<QLineEdit*>(widget)->setText(data->Value.str());
        }else if (data->Type.str() == "text-multi"){
            widget = new Q3MultiLineEdit(this, data->Field.str());
            connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
            if (!data->Value.str().isEmpty())
                static_cast<Q3MultiLineEdit*>(widget)->setText(data->Value.str());
        }else if (data->Type.str() == "boolean" && !data->Label.str().isEmpty()){
            widget = new QCheckBox(data->Label.str(), this, data->Field.str());
            if (!data->Value.str().isEmpty() && !data->Value.str().startsWith("0"))
                static_cast<QCheckBox*>(widget)->setChecked(true);
            data->Label.clear();
            bJoin = true;
        }else if (data->Type.str() == "fixed"){
            if (!data->Value.str().isEmpty()){
                QString text = i18(data->Value.str());
                text = text.replace(QRegExp("  +"), "\n");
                if (m_bFirst){
                    if (!m_label.isEmpty())
                        m_label += '\n';
                    m_label += text;
                }else{
                    QLabel *label = new QLabel(text, this);
                    label->setAlignment(Qt::WordBreak);
                    widget = label;
                    bJoin = true;
                }
            }
        }else if (data->Type.str() == "instructions"){
            if (!data->Value.str().isEmpty()){
                QString text = i18(data->Value.str());
                text = text.replace(QRegExp("  +"), "\n");
                if (!m_instruction.isEmpty())
                    m_instruction += '\n';
                m_instruction += text;
            }
        }else if (data->Type.str() == "list-single"){
            CComboBox *box = new CComboBox(this, data->Field.str());
            int cur = 0;
            int n = 0;
            for (unsigned i = 0; i < data->nOptions.toULong(); i++){
                QString label = get_str(data->OptionLabels, i);
                QString val   = get_str(data->Options, i);
                if(!label.isEmpty() && !val.isEmpty())
				{
                    box->addItem(i18(label), val);
                    if (data->Value.str() == val)
                        cur = n;
                    n++;
                }
            }
            box->setCurrentItem(cur);
            widget = box;
        }else if (data->Type.str() == "key"){
            if (!data->Value.str().isEmpty())
                m_key = data->Value.str();
        }else if (data->Type.str() == "password"){
            widget = new QLineEdit(this, "password");
            static_cast<QLineEdit*>(widget)->setEchoMode(QLineEdit::Password);
            connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
            connect(widget, SIGNAL(textChanged(const QString&)), m_receiver, SLOT(textChanged(const QString&)));
            data->Label.str() = "Password";
        }else if (data->Type.str() == "online"){
            widget = new QCheckBox(this, "online");
            static_cast<QCheckBox*>(widget)->setText(i18n("Online only"));
            bJoin = true;
        }else if (data->Type.str() == "sex"){
            CComboBox *box = new CComboBox(this, data->Field.str());
            box->addItem(QString::null, "0");
            box->addItem(i18n("Male"), "1");
            box->addItem(i18n("Female"), "2");
            data->Label.str() == I18N_NOOP("Gender");
            widget = box;
        }else{
            defFlds *f;
            for (f = fields; f->tag; f++)
                if (data->Type.str() == QString::fromUtf8(f->tag))
                    break;
            if (f->tag){
                widget = new QLineEdit(this, f->tag);
                connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
                connect(widget, SIGNAL(textChanged(const QString&)), m_receiver, SLOT(textChanged(const QString&)));
                if (!data->Value.str().isEmpty())
                    static_cast<QLineEdit*>(widget)->setText(data->Value.str());
                data->Label.str() = QString::fromUtf8(f->name);
                if (f->bRequired && m_bRegister)
                    data->bRequired.asBool() = true;
            }else if (!data->Label.str().isEmpty()){
                widget = new QLineEdit(this, data->Field.str());
                connect(widget, SIGNAL(returnPressed()), m_receiver, SLOT(search()));
                connect(widget, SIGNAL(textChanged(const QString&)), m_receiver, SLOT(textChanged(const QString&)));
                if (!data->Value.str().isEmpty())
                    static_cast<QLineEdit*>(widget)->setText(data->Value.str());
            }
        }
    }else{
        createLayout();
        m_widgets.clear();
        m_labels.clear();
        m_descs.clear();
        m_bDirty = true;
        QTimer::singleShot(0, this, SLOT(setSize()));
        return;
    }
    if (widget){
        m_bFirst = false;
        if (data->bRequired.toBool())
            m_required.push_back(widget);
        QLabel *label = NULL;
        if (!bJoin && !data->Label.str().isEmpty()){
            QString text = i18(data->Label.str());
            if (!text.isEmpty() && (text[(int)(text.length() - 1)] != ':'))
                text += ':';
            label = new QLabel(text, this);
            label->setAlignment(Qt::AlignRight);
        }
        QWidget *help = NULL;
        if (!data->Desc.str().isEmpty())
            help = new HelpButton(data->Desc.str(), this);
        m_labels.push_back(label);
        m_widgets.push_back(widget);
        m_descs.push_back(help);
    }
}

void JabberSearch::setSize()
{
    if (!m_bDirty || (parent() == NULL))
        return;
    m_bDirty = false;
    for (QWidget *p = this; p; p = p->parentWidget()){
        QSize s  = p->sizeHint();
        QSize s1 = QSize(p->width(), p->height());
        if (s.isValid())
            p->setMinimumSize(s);
        p->resize(QMAX(s.width(), s1.width()), QMAX(s.height(), s1.height()));
        if (p->layout())
            p->layout()->invalidate();
        if (p == topLevelWidget())
            break;
    }
    QWidget *t = topLevelWidget();
    QSize s = t->sizeHint();
    t->resize(QMAX(t->width(), s.width()), QMAX(t->height(), s.height()));
    t->adjustSize();
}

#if 0
static const char *any_data[] =
    {
        I18N_NOOP("First (Given)"),
        I18N_NOOP("Last (Family)"),
        I18N_NOOP("Nick (Alias)"),
        I18N_NOOP("Email"),
        I18N_NOOP("Select the speed of the search.  \"Fast\" matches your string to the beginning of the field only (ie. \"b\" would yield Bob,Bill,etc...)  \"Slower\" matches your string anywhere in the field (ie. \"b\" would yield Bob, Bill, Caleb, Robbie, etc...)"),
        I18N_NOOP("Search Speed"),
        I18N_NOOP("Fast / Less accurate"),
        I18N_NOOP("Slower / More extensive")
        I18N_NOOP("Full name")
        I18N_NOOP("First Name")
        I18N_NOOP("Last Name")
        I18N_NOOP("Nickname")
        I18N_NOOP("E-mail")
        I18N_NOOP("Username")
        I18N_NOOP("Password")
        I18N_NOOP("Enter your MSN Messenger account and password. Example: user@hotmail.com.  Nickname is optional.")
        I18N_NOOP("Enter your AIM screenname or ICQ UIN and the password for that account")
        I18N_NOOP("Enter your YAHOO! Messenger Username and Password.")
        I18N_NOOP("Please enter your UIN and password")
        I18N_NOOP("You need a x:data capable client to register.")
        I18N_NOOP("Enter nick you want to register.")
        I18N_NOOP("Complete the form to submit your searchable attributes in the Jabber User Directory")
        I18N_NOOP("Fill in all of the fields to add yourself to the JUD.")
        I18N_NOOP("Fill in a field to search for any matching Jabber User (POSIX regular expressions allowed)")
        I18N_NOOP("Fill in a field to search for any matching Jabber users.")
        I18N_NOOP("To register, please fill out the following form.  Be as accurate as possible to make it easier for people to search for you.")
    };
#endif

QString JabberSearch::i18(const char *text)
{
    if ((text == NULL) || (*text == 0))
        return QString::null;
    QString res = QString::fromUtf8(text);
    for (int i = 0; i < (int)res.length(); i++){
        if (res[i].unicode() >= 0x80)
            return res;
    }
    QString  tstr = i18n(res);
    if (tstr == res)
        return res;
    return tstr;
}

bool JabberSearch::canSearch()
{
    bool bRes = true;

    QObjectList l = queryList("QLineEdit");
    QObject *obj;

    foreach(obj,l)
	{
		QLineEdit *edit = static_cast<QLineEdit*>(obj);
		if (edit->echoMode() == QLineEdit::Password){
			if (edit->text().isEmpty()){
				bRes = false;
				break;
			}
			continue;
		}
		if (edit->text().isEmpty()){
			list<QWidget*>::iterator itw;
			for (itw = m_required.begin(); itw != m_required.end(); ++itw){
				if (*itw == edit)
					break;
			}
			if (itw != m_required.end()){
				bRes = false;
				break;
			}
		}
		if (!edit->text().isEmpty())
			bRes = true;
	}
    return bRes;
}

QString JabberSearch::condition(QWidget *w)
{
    QString res;
    if (m_bXData && (w == NULL))
        res += "x:data";

    if (w == NULL)
        w = this;

    QObjectList l = w->queryList("QLineEdit");
    QObject *obj;
    foreach(obj,l)
	{
        QLineEdit *edit = static_cast<QLineEdit*>(obj);
        if (!edit->text().isEmpty()){
            if (!res.isEmpty())
                res += ';';
            res += edit->name();
            res += '=';
            res += quoteChars(edit->text(), ";");
        }
    }

    l = w->queryList("QComboBox");
    foreach(obj,l)
	{
        CComboBox *box = static_cast<CComboBox*>(obj);
        if (box->currentText().isEmpty()){
            continue;
        }
        if (!res.isEmpty())
            res += ';';
        res += box->name();
        res += '=';
        res += quoteChars(box->value(), ";");
    }

    l = w->queryList("QCheckBox");
    foreach(obj,l)
	{
        QCheckBox *box = static_cast<QCheckBox*>(obj);
        if (!res.isEmpty())
            res += ';';
        res += box->name();
        res += box->isChecked() ? "=1" : "=0";
    }

    l = w->queryList("Q3MultiLineEdit");
    foreach(obj,l)
	{
        Q3MultiLineEdit *edit = static_cast<Q3MultiLineEdit*>(obj);
        if (!edit->text().isEmpty()){
            if (!res.isEmpty())
                res += ';';
            res += edit->name();
            res += '=';
            res += quoteChars(edit->text(), ";");
        }
    }

    if (!m_key.isEmpty() && (w == NULL)){
        if (!res.isEmpty())
            res += ';';
        res += "key=";
        res += quoteChars(m_key, ";");
    }
    return res;
}

void JabberSearch::createLayout()
{
    unsigned start = 0;
    unsigned nCols = 0;
    unsigned nRows = 0;
    Q3VBoxLayout *vlay = new Q3VBoxLayout(this);
    Q3GridLayout *lay = new Q3GridLayout(vlay);
    vlay->setMargin(11);
    lay->setSpacing(6);
    vlay->addStretch();
    if (!m_widgets.empty()){
        nCols = (m_widgets.size() + MAX_ELEMENTS - 1) / MAX_ELEMENTS;
        nRows = (m_widgets.size() + nCols - 1) / nCols;
        start = 0;
        if (!m_label.isEmpty()){
            QLabel *label = new QLabel(m_label, this);
            label->setAlignment(Qt::WordBreak);
            lay->addMultiCellWidget(label, 0, 0, 0, nCols * 3 + 1);
            m_label = QString::null;
            start = 1;
        }
        unsigned row = start;
        unsigned col = 0;
        for (unsigned i = 0; i < m_widgets.size(); i++, row++){
            if (row >= nRows + start){
                row  = 0;
                col += 3;
            }
            if (m_labels[i]){
                static_cast<QLabel*>(m_labels[i])->setAlignment( Qt::AlignVCenter | Qt::AlignRight);
                lay->addWidget(m_labels[i], row, col);
                if (m_descs[i]){
                    lay->addWidget(m_widgets[i], row, col + 1, Qt::AlignVCenter);
                    lay->addWidget(m_descs[i], row, col + 2, Qt::AlignVCenter);
                    m_descs[i]->show();
                }else{
                    lay->addMultiCellWidget(m_widgets[i], row, row, col + 1, col + 2, Qt::AlignVCenter);
                }
                m_labels[i]->show();
            }else{
                if (m_descs[i]){
                    lay->addMultiCellWidget(m_widgets[i], row, row, col, col + 1, Qt::AlignVCenter);
                    lay->addWidget(m_descs[i], row, col + 2, Qt::AlignBottom);
                    m_descs[i]->show();
                }else{
                    lay->addMultiCellWidget(m_widgets[i], row, row, col, col + 2, Qt::AlignVCenter);
                }
            }
            m_widgets[i]->show();
        }
    }
    if (!m_instruction.isEmpty()){
        QLabel *label = new QLabel(m_instruction, this);
        label->setAlignment(Qt::WordBreak);
        lay->addMultiCellWidget(label, nRows + start, nRows + start, 0, nCols * 3 - 1);
        m_instruction = QString::null;
    }
}

HelpButton::HelpButton(const QString &help, QWidget *parent)
        : QPushButton(parent)
{
    const QPixmap p = Pict("help");
    setPixmap(p);
    m_help = help;
    connect(this, SIGNAL(clicked()), this, SLOT(click()));
    setMinimumSize(p.width() + 2, p.height() + 2);
    setMaximumSize(p.width() + 2, p.height() + 2);
}

void HelpButton::click()
{
    BalloonMsg::message(m_help, this);
}

JIDJabberSearch::JIDJabberSearch(QWidget *parent, const char *name)
        : JabberSearch(parent, name)
{
    m_adv = NULL;
}

void JIDJabberSearch::setAdvanced(JIDAdvSearch *adv)
{
    m_adv = adv;
}

static const char *names[] =
    {
        "username",
        "email",
        "nick",
        "first",
        "last",
        "user",
        "fn",
        "given",
        "online",
        NULL
    };

const unsigned MAX_MAIN	= 6;

void JIDJabberSearch::createLayout()
{
    unsigned rowMain = 0;
    Q3GridLayout *lay = new Q3GridLayout(this);
    Q3GridLayout *alay = NULL;
    lay->setSpacing(6);
    unsigned nAdv = 0;
    unsigned nMain = 0;
    if (m_widgets.size() > MAX_MAIN){
        alay = new Q3GridLayout(m_adv->grpSearch);
        alay->setMargin(11);
        alay->setSpacing(6);
        for (unsigned i = 0; i < m_widgets.size(); i++){
            if (nMain > MAX_MAIN){
                nAdv++;
                continue;
            }
            const char **p;
            for (p = names; *p; p++)
                if (!strcmp(*p, m_widgets[i]->name()))
                    break;
            if (*p == NULL){
                nAdv++;
                continue;
            }
            nMain++;
        }
    }
    unsigned nCols = (nAdv + MAX_ELEMENTS - 1) / MAX_ELEMENTS;
    unsigned nRows = nCols ? (nAdv + nCols - 1) / nCols : 0;
    unsigned start = 0;
    unsigned row   = 0;
    unsigned col   = 0;
    nMain = 0;
    for (unsigned i = 0; i < m_widgets.size(); i++){
        bool bMain = false;
        if (alay){
            if (nMain < MAX_MAIN){
                const char **p;
                for (p = names; *p; p++)
                    if (!strcmp(*p, m_widgets[i]->name()))
                        break;
                if (*p){
                    nMain++;
                    bMain = true;
                }
            }
        }else{
            bMain = true;
        }
        if (bMain){
            if (m_labels[i]){
                static_cast<QLabel*>(m_labels[i])->setAlignment(Qt::AlignVCenter);
                lay->addMultiCellWidget(m_labels[i], rowMain, rowMain, 0, 1, Qt::AlignVCenter);
                m_labels[i]->show();
                rowMain++;
            }
            if (m_descs[i]){
                lay->addWidget(m_widgets[i], rowMain, 0, Qt::AlignVCenter);
                lay->addWidget(m_descs[i], rowMain, 1, Qt::AlignVCenter);
                m_descs[i]->show();
            }else{
                lay->addMultiCellWidget(m_widgets[i], rowMain, rowMain, 0, 1, Qt::AlignVCenter);
            }
            m_widgets[i]->show();
            rowMain++;
        }else{
            if (row >= nRows + start){
                row  = 0;
                col += 3;
            }
            m_widgets[i]->reparent(m_adv->grpSearch, QPoint(0, 0));
            if (m_descs[i])
                m_descs[i]->reparent(m_adv->grpSearch, QPoint(0, 0));
            if (m_labels[i]){
                m_labels[i]->reparent(m_adv->grpSearch, QPoint(0, 0));
                static_cast<QLabel*>(m_labels[i])->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
                alay->addWidget(m_labels[i], row, col);
                if (m_descs[i]){
                    alay->addWidget(m_widgets[i], row, col + 1, Qt::AlignVCenter);
                    alay->addWidget(m_descs[i], row, col + 2, Qt::AlignVCenter);
                    m_descs[i]->show();
                }else{
                    alay->addMultiCellWidget(m_widgets[i], row, row, col + 1, col + 2, Qt::AlignVCenter);
                }
                m_labels[i]->show();
            }else{
                if (m_descs[i]){
                    alay->addMultiCellWidget(m_widgets[i], row, row, col, col + 1, Qt::AlignVCenter);
                    alay->addWidget(m_descs[i], row, col + 2, Qt::AlignBottom);
                    m_descs[i]->show();
                }else{
                    alay->addMultiCellWidget(m_widgets[i], row, row, col, col + 2, Qt::AlignVCenter);
                }
            }
            m_widgets[i]->show();
            row++;
        }
    }
    if (alay){
        m_adv->lblTitle->setText(m_title);
        m_adv->lblInstruction->setText(m_instruction);
    }
    m_instruction = QString::null;
}

/*
#ifndef NO_MOC_INCLUDES
#include "jabbersearch.moc"
#endif
*/

