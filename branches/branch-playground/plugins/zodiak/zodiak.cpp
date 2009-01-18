/***************************************************************************
                          zodiak.cpp  -  description
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

#include <qapplication.h>
#include <qlayout.h>
#include <q3frame.h>
#include <qwidget.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qpainter.h>
//Added by qt3to4:
#include <Q3BoxLayout>
#include <QPaintEvent>
#include <Q3GridLayout>
#include <QChildEvent>
#include <QPixmap>
#include <QLabel>
#include <QEvent>

#include "datepicker.h"
#include "misc.h"

#include "zodiak.h"

#include "xpm/1.xpm"
#include "xpm/2.xpm"
#include "xpm/3.xpm"
#include "xpm/4.xpm"
#include "xpm/5.xpm"
#include "xpm/6.xpm"
#include "xpm/7.xpm"
#include "xpm/8.xpm"
#include "xpm/9.xpm"
#include "xpm/10.xpm"
#include "xpm/11.xpm"
#include "xpm/12.xpm"

using namespace SIM;

Plugin *createZodiakPlugin(unsigned base, bool, Buffer*)
{
    Plugin *plugin = new ZodiakPlugin(base);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("Zodiak"),
        I18N_NOOP("Plugin provides show zodiak pictures for date edit"),
        VERSION,
        createZodiakPlugin,
        PLUGIN_NOLOAD_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

ZodiakPlugin::ZodiakPlugin(unsigned base)
        : Plugin(base)
{
	qApp->installEventFilter(this);
	QWidgetList list = QApplication::topLevelWidgets();
	for(QWidgetList::iterator it = list.begin(); it != list.end(); ++it)
	{
		QWidget* w = *it;
		QObjectList l = w->queryList("DatePicker");
		for(QObjectList::iterator it1 = l.begin(); it1 != l.end(); ++it1)
		{
			QObject* obj = *it1;
			createLabel(static_cast<DatePicker*>(obj));
		}
	}
}

ZodiakPlugin::~ZodiakPlugin()
{
    // The labels we created would be destroyed on their own, since their parent object
    // (the DatePicker) is destroyed. We just clean the list so it won't contain stale Pickers.
    m_pickers.clear();
}

void ZodiakPlugin::createLabel(DatePicker *picker)
{
    Picker p;
    p.picker = picker;
    p.label  = new ZodiakWnd(picker);
    m_pickers.append(p);
    if (p.picker->layout())
        static_cast<Q3BoxLayout*>(p.picker->layout())->addWidget(p.label);
    p.label->show();
}

bool ZodiakPlugin::processEvent(Event *e)
{
    if (e->type() == eEventQuit)
        m_pickers.clear();
    return false;
}

bool ZodiakPlugin::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::ChildAdded){
        QChildEvent *ce = (QChildEvent*)e;
        if (ce->child()->inherits("DatePicker")){
            DatePicker *picker = (DatePicker*)(ce->child());
            Q3ValueListIterator<Picker> it;
            for (it = m_pickers.begin(); it != m_pickers.end(); ++it){
                if ((*it).picker == picker)
                    break;
            }
            if (it == m_pickers.end())
                createLabel(picker);
        }
    }
    if (e->type() == QEvent::ChildRemoved){
        QChildEvent *ce = (QChildEvent*)e;
        if (ce->child()->inherits("DatePicker")){
            DatePicker *picker = (DatePicker*)(ce->child());
            for (Q3ValueListIterator<Picker> it = m_pickers.begin(); it != m_pickers.end(); ++it){
                if ((*it).picker == picker){
                    m_pickers.remove(it);
                    break;
                }
            }
        }
    }
    return QObject::eventFilter(o, e);
}

ZodiakWnd::ZodiakWnd(DatePicker *parent)
        : Q3Frame(parent)
{
    m_picker = parent;
    setLineWidth(0);
    Q3GridLayout *lay = new Q3GridLayout(this, 2, 2);
    lay->setSpacing(2);
    lay->setMargin(4);
    m_picture = new QLabel(this);
    m_picture->setFixedSize(52, 52);
    m_picture->setFrameShadow(Sunken);
    m_picture->setLineWidth(1);
    lay->addMultiCellWidget(m_picture, 0, 1, 0, 0);
    m_name = new QLabel(this);
    QFont f(font());
    f.setBold(true);
    m_name->setFont(f);
    m_name->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    lay->addWidget(m_name, 0, 1);
    m_button = new QPushButton(this);
    m_button->setText(i18n("View horoscope"));
    lay->addWidget(m_button, 1, 1);
    changed();
    connect(parent, SIGNAL(changed()), this, SLOT(changed()));
    connect(m_button, SIGNAL(clicked()), this, SLOT(view()));
}

void ZodiakWnd::paintEvent(QPaintEvent *e)
{
    if (parentWidget() && parentWidget()->parentWidget() && parentWidget()->parentWidget()->backgroundPixmap()){
        QPoint pos = mapToParent(QPoint(0, 0));
        pos = parentWidget()->mapToParent(pos);
        QPainter p(this);
        p.drawTiledPixmap(0, 0, width(), height(), *parentWidget()->parentWidget()->backgroundPixmap(), pos.x(), pos.y());
        return;
    }
    Q3Frame::paintEvent(e);
}

static const char *signes[] =
    {
        I18N_NOOP("Aries"),			// 21.03. - 20.04.
        I18N_NOOP("Taurus"),		// 21.04. - 20.05.
        I18N_NOOP("Gemini"),		// 21.05. - 21.06.
        I18N_NOOP("Cancer"),		// 22.06. - 22.07.
        I18N_NOOP("Leo"),			// 23.07. - 23.08.
        I18N_NOOP("Virgo"),			// 24.08. - 23.09.
        I18N_NOOP("Libra"),			// 24.09. - 23.10.
        I18N_NOOP("Scorpio"),		// 24.10. - 22.11.
        I18N_NOOP("Saqittarius"),	// 23.11. - 21.12.
        I18N_NOOP("Capricorn"),		// 22.12. - 20.01.
        I18N_NOOP("Aquarius"),		// 21.01. - 19.02.
        I18N_NOOP("Pisces")			// 20.02. - 20.03.
    };

static const char **xpms[] =
    {
        xpm_1,
        xpm_2,
        xpm_3,
        xpm_4,
        xpm_5,
        xpm_6,
        xpm_7,
        xpm_8,
        xpm_9,
        xpm_10,
        xpm_11,
        xpm_12
    };

void ZodiakWnd::changed()
{
    int day = m_picker->getDate().day();
    int month = m_picker->getDate().month();
    int year = m_picker->getDate().year();
    if (day && month && year){
        int n = getSign(day, month);
        m_picture->setPixmap(QPixmap(xpms[n]));
        m_name->setText(i18n(signes[n]));
        m_button->show();
    }else{
        m_picture->setPixmap(QPixmap());
        m_name->setText(QString::null);
        m_button->hide();
    }
}

void ZodiakWnd::view()
{
    int day = m_picker->getDate().day();
    int month = m_picker->getDate().month();
    int year = m_picker->getDate().year();
    if (day && month && year){
        int n = getSign(day, month);
        QString s = QString("http://horoscopes.swirve.com/scope.cgi?Sign=%1").arg(signes[n]);
        EventGoURL e(s);
        e.process();
    }
}

static int bound[] =
    {
        21, 21, 21, 22, 23, 24, 24, 24, 23, 22, 21, 20
    };

int ZodiakWnd::getSign(int day, int month)
{
    month -= 3;
    if (month < 0)
        month += 12;
    if (day >= bound[month])
        return month;
    month--;
    if (month < 0)
        month += 12;
    return month;
}

