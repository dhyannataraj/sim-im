/***************************************************************************
                          commands.cpp  -  description
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

#include "commands.h"
#include "toolbtn.h"
#include "toolsetup.h"
#include "core.h"
#include "cmenu.h"
#include "log.h"

#include <qapplication.h>
#include <q3popupmenu.h>
#include <qwidget.h>
#include <q3accel.h>
//Added by qt3to4:
#include <QEvent>

using namespace SIM;

Commands::Commands()
{
    qApp->installEventFilter(this);
}

Commands::~Commands()
{
    CMDS_MAP::iterator it;
    for (it = bars.begin(); it != bars.end(); ++it){
        delete (*it).second;
    }
    MENU_MAP::iterator itm;
    for (itm = menus.begin(); itm != menus.end(); ++itm){
        MenuDef &def = (*itm).second;
        delete def.menu;
        delete def.def;
    }
}

CommandsDef *Commands::createBar(unsigned id)
{
    CMDS_MAP::iterator it = bars.find(id);
    if (it != bars.end())
        return (*it).second;
    CommandsDef *def = new CommandsDef(id, false);
    bars.insert(CMDS_MAP::value_type(id, def));
    return def;
}

void Commands::removeBar(unsigned id)
{
    CMDS_MAP::iterator it = bars.find(id);
    if (it == bars.end())
        return;
    delete (*it).second;
    bars.erase(it);
}

void Commands::clear()
{
    for (MENU_MAP::iterator it = menus.begin(); it != menus.end(); ++it){
        if ((*it).second.menu){
            delete (*it).second.menu;
            (*it).second.menu = NULL;
        }
    }
}

CommandsDef *Commands::createMenu(unsigned id)
{
    MENU_MAP::iterator it = menus.find(id);
    if (it != menus.end())
        return (*it).second.def;
    MenuDef def;
    def.def  = new CommandsDef(id, true);
    def.menu = NULL;
    def.param = NULL;
    menus.insert(MENU_MAP::value_type(id, def));
    return def.def;
}

void Commands::removeMenu(unsigned id)
{
    MENU_MAP::iterator it = menus.find(id);
    if (it == menus.end())
        return;
    if ((*it).second.menu)
        delete (*it).second.menu;
    delete (*it).second.def;
    menus.erase(it);
}

CToolBar *Commands::show(unsigned id, QMainWindow *parent)
{
    CMDS_MAP::iterator it = bars.find(id);
    if (it == bars.end())
        return NULL;
    (*it).second->setConfig(CorePlugin::m_plugin->getButtons(id));
    return new CToolBar((*it).second, parent);
}

CMenu *Commands::get(CommandDef *cmd)
{
    MENU_MAP::iterator it = menus.find(cmd->popup_id);
    if (it == menus.end())
        return NULL;
    MenuDef &d = (*it).second;
    if (d.menu && ((cmd->flags & COMMAND_NEW_POPUP) == 0)){
        d.menu->setParam(cmd->param);
        return d.menu;
    }
    d.def->setConfig(CorePlugin::m_plugin->getMenues(cmd->popup_id));
    CMenu *menu = new CMenu(d.def);
    menu->setParam(cmd->param);
    if ((cmd->flags & COMMAND_NEW_POPUP) == 0)
        d.menu = menu;
    return menu;
}

CMenu *Commands::processMenu(unsigned id, void *param, int key)
{
    MENU_MAP::iterator it = menus.find(id);
    if (it == menus.end())
        return NULL;
    MenuDef &d = (*it).second;
    if (key){
        CommandsList list(*d.def, true);
        CommandDef *cmd;
        while ((cmd = ++list) !=NULL){
            int cmdKey;
            if ((key & Qt::ALT) && ((key & ~Qt::MODIFIER_MASK) != Qt::Key_Alt)){
                if (cmd->text.isEmpty())
                    continue;
                cmdKey = Q3Accel::shortcutKey(i18n(cmd->text));
                if((cmdKey & ~Qt::UNICODE_ACCEL) == key)
				{
                    cmd->param = param;
                    EventCommandExec eCmd(cmd);
                    if (eCmd.process())
                        break;
                }
            }
            if (cmd->accel.isEmpty())
                continue;
            cmdKey = Q3Accel::stringToKey(i18n(cmd->accel));
            if (cmdKey == key){
                cmd->param = param;
                EventCommandExec eCmd(cmd);
                if (eCmd.process())
                    break;
            }
        }
        if (cmd == NULL)
            return NULL;
    }
    if (d.menu){
        d.menu->setParam(param);
        return d.menu;
    }
    d.def->setConfig(CorePlugin::m_plugin->getMenues(id));
    d.menu = new CMenu(d.def);
    d.menu->setParam(param);
    return d.menu;
}

CommandsDef *Commands::getDef(unsigned id)
{
    MENU_MAP::iterator it = menus.find(id);
    if (it == menus.end())
        return NULL;
    return (*it).second.def;
}

bool Commands::processEvent(Event *e)
{
	switch (e->type())
	{
		case eEventPluginsUnload:
			clear();
			break;
		case eEventToolbar:
			{
				EventToolbar *et = static_cast<EventToolbar*>(e);
				switch(et->action())
				{
					case EventToolbar::eAdd:
						createBar(et->id());
						break;
					case EventToolbar::eShow:
						et->setToolbar(show(et->id(), et->parent()));
						break;
					case EventToolbar::eRemove:
						removeBar(et->id());
						break;
				}
				return true;
			}
		case eEventMenu:
			{
				EventMenu *em = static_cast<EventMenu*>(e);
				switch(em->action()) {
					case EventMenu::eAdd:
						createMenu(em->id());
						break;
					case EventMenu::eRemove:
						removeMenu(em->id());
						break;
					case EventMenu::eCustomize:
						customizeMenu(em->id());
						break;
				}
				return true;
			}
		case eEventMenuGet:
			{
				EventMenuGet *egm = static_cast<EventMenuGet*>(e);
				egm->setMenu(get(egm->def()));
				return true;
			}
		case eEventMenuGetDef:
			{
				EventMenuGetDef *mgd = static_cast<EventMenuGetDef*>(e);
				mgd->setCommandsDef(getDef(mgd->id()));
				return true;
			}
		case eEventMenuProcess:
			{
				EventMenuProcess *emp = static_cast<EventMenuProcess*>(e);
				emp->setMenu(processMenu(emp->id(), emp->param(), emp->key()));
				return true;
			}
		default:
			break;
	}
	return false;
}

bool Commands::eventFilter(QObject *o, QEvent *e)
{
    if ((e->type() == QEvent::Show) && o->inherits("QMenu")){
        if (!o->inherits("CMenu")){
            QObject *parent = o->parent();
            if (parent){
                unsigned id = 0;
                if (parent->inherits("MainWindow")){
                    id = ToolBarMain;
                }else if (parent->inherits("CToolBar")){
                    CToolBar *bar = static_cast<CToolBar*>(parent);
                    id = bar->m_def->id();
                }
                if (id){
                    Q3PopupMenu *popup = static_cast<Q3PopupMenu*>(o);
                    popup->insertItem(i18n("Customize toolbar..."), this, SLOT(popupActivated()));
                    cur_id = id;
                }
            }
        }
    }
    return QObject::eventFilter(o, e);
}

void Commands::popupActivated()
{
    CMDS_MAP::iterator it = bars.find(cur_id);
    if (it == bars.end())
        return;
    customize((*it).second);
}

void Commands::customize(CommandsDef *def)
{
    QWidgetList list = QApplication::topLevelWidgets();
    QWidget * w;
	ToolBarSetup *wnd = NULL;
    foreach (w,list)
	{
		if(!w->inherits("ToolBarSetup"))
			continue;
		ToolBarSetup *swnd = static_cast<ToolBarSetup*>(w);
		if (swnd->m_def != def)
			continue;
		wnd = swnd;
		break;
	}
    if (wnd == NULL)
        wnd = new ToolBarSetup(this, def);
    raiseWindow(wnd);
}

void Commands::customizeMenu(unsigned long id)
{
    MENU_MAP::iterator it = menus.find(id);
    if (it == menus.end())
        return;
    MenuDef &d = (*it).second;
    d.def->setConfig(CorePlugin::m_plugin->getMenues(id));
    customize(d.def);
}

void Commands::set(CommandsDef *def, const char *str)
{
    if (def->isMenu()){
        CorePlugin::m_plugin->setMenues(def->id(), str);
    }else{
        CorePlugin::m_plugin->setButtons(def->id(), str);
        EventToolbarChanged(def).process();
    }
}

