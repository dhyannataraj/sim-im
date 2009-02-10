/***************************************************************************
                          statuswnd.cpp  -  description
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

#include "icons.h"
#include "statuswnd.h"
#include "core.h"
#include "ballonmsg.h"
#include "toolbtn.h"
#include "socket.h"
#include "log.h"

#include <q3popupmenu.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <QFrame>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qimage.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>

using namespace std;
using namespace SIM;

StatusLabel::StatusLabel(QWidget *parent, Client *client, unsigned id)
        : QLabel(parent)
{
    m_client = client;
    m_bBlink = false;
    m_id = id;
    m_timer = NULL;
    setPict();
}

void StatusLabel::setPict()
{
    QString icon;
    const char *text;
    if (m_client->getState() == Client::Connecting){
        if (getSocketFactory()->isActive()){
            if (m_timer == NULL){
                m_timer = new QTimer(this);
                connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
                m_timer->start(1000);
                m_bBlink = false;
            }
            Protocol *protocol = m_client->protocol();
            text = I18N_NOOP("Connecting");
            unsigned status;
            if (m_bBlink){
                icon = "online";
                status = m_client->getManualStatus();
            }else{
                icon = "offline";
                status = STATUS_OFFLINE;
            }
            if (protocol){
                for (const CommandDef *cmd = protocol->statusList(); !cmd->text.isEmpty(); cmd++){
                    if (cmd->id == status){
                        icon = cmd->icon;
                        break;
                    }
                }
            }
        }else{
            if (m_timer){
                delete m_timer;
                m_timer = NULL;
            }

            Protocol *protocol = m_client->protocol();
            const CommandDef *cmd = protocol->description();
            icon = cmd->icon;
            int n = icon.find('_');
            if (n > 0)
                icon = icon.left(n);
            icon += "_inactive";
            text = I18N_NOOP("Inactive");
        }
    }
	else
	{
        if (m_timer)
		{
            delete m_timer;
            m_timer = NULL;
        }
        if (m_client->getState() == Client::Error){
            icon = "error";
            text = I18N_NOOP("Error");
        }else{
            Protocol *protocol = m_client->protocol();
            const CommandDef *cmd = protocol->description();
            icon = cmd->icon;
            text = cmd->text;
            for (cmd = protocol->statusList(); !cmd->text.isEmpty(); cmd++){
                if (cmd->id == m_client->getStatus()){
                    icon = cmd->icon;
                    text = cmd->text;
                    break;
                }
            }
        }
    }
    QPixmap p = Pict(icon);
    setPixmap(p);
    QString tip = CorePlugin::m_plugin->clientName(m_client);
    tip += '\n';
    tip += i18n(text);
    QToolTip::add(this, tip);
    resize(p.width(), p.height());
    setFixedSize(p.width(), p.height());
}

void StatusLabel::timeout()
{
    m_bBlink = !m_bBlink;
    setPict();
}

void StatusLabel::mousePressEvent(QMouseEvent *me)
{
	if(me->button() == Qt::RightButton)
	{
		EventMenuProcess eMenu(m_id, (void *)winId());
		eMenu.process();
		Q3PopupMenu *popup = eMenu.menu();
		if(popup)
		{
			QPoint pos = CToolButton::popupPos(this, popup);
			popup->popup(pos);
		}
	}
}

StatusFrame::StatusFrame(QWidget *parent) : QFrame(parent), EventReceiver(LowPriority + 1)
{
	log(L_DEBUG, "StatusFrame::StatusFrame()");
    setFrameStyle(NoFrame);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_frame = new QFrame(this);
    m_frame->show();
    m_lay = new QHBoxLayout(m_frame);
    m_lay->setMargin(1);
    m_lay->setSpacing(2);
    m_lay->addStretch();
    addClients();
}

void StatusFrame::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::RightButton){
        Command cmd;
        cmd->id = MenuConnections;
        EventMenuGet e(cmd);
        e.process();
        Q3PopupMenu *popup = e.menu();
        if (popup)
            popup->popup(me->globalPos());
    }
}

bool StatusFrame::processEvent(Event *e)
{
    switch (e->type()){
    case eEventSocketActive:
		{
			QObjectList list = queryList("StatusLabel");
			for (int i = 0; i < list.size(); ++i) 
				if (StatusLabel *lbl = dynamic_cast<StatusLabel *>(list.at(i)))
					lbl->setPict();
			break;
		}
    case eEventCheckCommandState:
		{
			EventCheckCommandState *ecs = static_cast<EventCheckCommandState*>(e);
			CommandDef *cmd = ecs->cmd();
			if ((cmd->menu_id == MenuStatusWnd) && (cmd->id == CmdStatusWnd)){
				unsigned n = 0;
				{
					QObjectList list = queryList("StatusLabel");
					for (int i = 0; i < list.size(); ++i) 
						if (StatusLabel *lbl = dynamic_cast<StatusLabel *>(list.at(i)))
							if (lbl->x() + lbl->width() > width())
								n++;

				}
				CommandDef *cmds = new CommandDef[n + 1];
				n = 0;
				QObjectList list = queryList("StatusLabel");
				for (int i = 0; i < list.size(); ++i)
				{
					if (StatusLabel *lbl = dynamic_cast<StatusLabel *>(list.at(i))) 
					{
						if (lbl->x() + lbl->width() > width())
						{
							cmds[n].id = 1;
							cmds[n].text = "_";
							cmds[n].text_wrk = CorePlugin::m_plugin->clientName(lbl->m_client);
							cmds[n].popup_id = lbl->m_id;
							if (lbl->m_client->getState() == Client::Error)
							{
								cmds[n].icon = "error";
							}
							else
							{
								Protocol *protocol = lbl->m_client->protocol();
								const CommandDef *cmd = protocol->description();
								cmds[n].icon = cmd->icon;
								for (cmd = protocol->statusList(); !cmd->text.isEmpty(); cmd++)
								{
									if (cmd->id == lbl->m_client->getStatus())
									{
										cmds[n].icon = cmd->icon;
										break;
									}
								}
							}
							n++;
						}
					}
				}
				cmd->param = cmds;
				cmd->flags |= COMMAND_RECURSIVE;
				return true;
			}
			break;
		}
    case eEventClientsChanged:
        addClients();
        break;
    case eEventClientChanged:{
        EventClientChanged *ecc = static_cast<EventClientChanged*>(e);
        StatusLabel *lbl = findLabel(ecc->client());
        if (lbl)
            lbl->setPict();
        break;
    }
    case eEventIconChanged:{
            QObjectList list = queryList("StatusLabel");
			for (int i = 0; i < list.size(); ++i) 
				if (StatusLabel *lbl = dynamic_cast<StatusLabel *>(list.at(i)))
					lbl->setPict();
            break;
        }
    default:
        break;
    }
    return false;
}

void StatusFrame::addClients()
{
    list<StatusLabel*> lbls;
    QObjectList l = m_frame->queryList("StatusLabel");
	QObjectList::iterator itObject = l.begin();
    QObject *obj;
    while((obj = *itObject) != NULL)
	{
		if(itObject == l.end())
			break;
        ++itObject;
        lbls.push_back(static_cast<StatusLabel*>(obj));
    }
    for (list<StatusLabel*>::iterator it = lbls.begin(); it != lbls.end(); ++it)
        delete *it;
    for (unsigned i = 0; i < getContacts()->nClients(); i++)
	{
        Client *client = getContacts()->getClient(i);
        QWidget *w = new StatusLabel(m_frame, client, CmdClient + i);
        m_lay->addWidget(w);
        w->show();
    }
    adjustPos();
    repaint();
}

StatusLabel *StatusFrame::findLabel(Client *client)
{
    QObjectList l = m_frame->queryList("StatusLabel");
	QObjectList::iterator itObject = l.begin();
    QObject *obj;
    while ((obj = *itObject) != NULL){
        ++itObject;
        if (static_cast<StatusLabel*>(obj)->m_client == client){
            return static_cast<StatusLabel*>(obj);
        }
    }
    return NULL;
}

QSize StatusFrame::sizeHint() const
{
    QSize res = m_frame->sizeHint();
    res.setWidth(20);
    return res;
}

QSize StatusFrame::minimumSizeHint() const
{
    QSize res = m_frame->minimumSizeHint();
    res.setWidth(20);
    return res;
}

void StatusFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    adjustPos();
}

void StatusFrame::adjustPos()
{
    QSize s = m_frame->minimumSizeHint();
    m_frame->resize(s);
    m_frame->move(width() > s.width() ? width() - s.width() : 0, 0);
    emit showButton(width() < s.width());
    repaint();
    m_frame->repaint();
    QObjectList l = m_frame->queryList("StatusLabel");
	QObjectList::iterator itObject = l.begin();
    QObject *obj;
    while ((obj = *itObject) != NULL)
	{
		if(itObject == l.end())
			break;
        ++itObject;
        static_cast<StatusLabel*>(obj)->repaint();
    }
}

static const char * const arrow_h_xpm[] = {
            "9 7 3 1",
            "	    c None",
            ".	    c #000000",
            "+	    c none",
            "..++..+++",
            "+..++..++",
            "++..++..+",
            "+++..++..",
            "++..++..+",
            "+..++..++",
            "..++..+++"};

StatusWnd::StatusWnd() : QFrame(NULL)
{
	log(L_DEBUG, "StatusWnd::StatusWnd()");
    setFrameStyle(NoFrame);
    m_lay = new QHBoxLayout(this);
    m_frame = new StatusFrame(this);
    m_btn = new QToolButton(this);
    m_btn->setAutoRaise(true);
    m_btn->setPixmap( QPixmap((const char **)arrow_h_xpm));
    m_btn->setMinimumSize(QSize(10, 10));
    m_lay->addWidget(m_frame);
    m_lay->addWidget(m_btn);
    connect(m_frame, SIGNAL(showButton(bool)), this, SLOT(showButton(bool)));
    connect(m_btn, SIGNAL(clicked()), this, SLOT(clicked()));
    EventAddWidget(this, true, EventAddWidget::eStatusWindow).process();
}

void StatusWnd::showButton(bool bState)
{
    if (bState){
        m_btn->show();
    }else{
        m_btn->hide();
    }
}

void StatusWnd::clicked()
{
    Command cmd;
    cmd->popup_id = MenuStatusWnd;
    cmd->flags    = COMMAND_NEW_POPUP;
    EventMenuGet e(cmd);
    e.process();
    Q3PopupMenu *popup = e.menu();
    if (popup){
        QPoint pos = CToolButton::popupPos(m_btn, popup);
        popup->popup(pos);
    }
}

BalloonMsg *StatusWnd::showError(const QString &text, QStringList &buttons, Client *client)
{
    if (!isVisible())
        return NULL;
    StatusLabel *lbl = m_frame->findLabel(client);
    if (lbl == NULL)
        return NULL;
    if (lbl->x() + lbl->width() > width())
        return NULL;
    return new BalloonMsg(NULL, text, buttons, lbl);
}

