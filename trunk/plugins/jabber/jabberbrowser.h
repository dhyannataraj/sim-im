/***************************************************************************
                          jabberbrowser.h  -  description
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

#ifndef _JABBERBROWSER_H
#define _JABBERBROWSER_H

#include "jabberclient.h"
#include "simapi.h"
#include "stl.h"

#include <qmainwindow.h>
#include <qwizard.h>

class ListView;
class QListViewItem;
class QStatusBar;
class CToolBar;
class DiscoInfo;
class JabberWizard;
class QLabel;

const unsigned COL_NAME				= 0;
const unsigned COL_JID				= 1;
const unsigned COL_NODE				= 2;
const unsigned COL_CATEGORY			= 3;
const unsigned COL_TYPE				= 4;
const unsigned COL_FEATURES			= 5;
const unsigned COL_ID_DISCO_ITEMS	= 6;
const unsigned COL_ID_DISCO_INFO	= 7;
const unsigned COL_ID_BROWSE		= 8;
const unsigned COL_MODE				= 9;

class JabberSearch;
class JIDSearch;

class JabberWizard : public QWizard, public SIM::EventReceiver
{
    Q_OBJECT
public:
    JabberWizard(QWidget *parent, const QString &title, const char *icon, JabberClient *client, const char *jid, const char *node, const char *type);
    JabberSearch *m_search;
    QLabel		 *m_result;
    void initTitle();
protected slots:
    void setNext();
    void search();
    void textChanged(const QString&);
    void slotSelected(const QString&);
protected:
    void *processEvent(SIM::Event *e);
    std::string m_type;
    std::string m_id;
};

class JabberBrowser : public QMainWindow, public SIM::EventReceiver
{
    Q_OBJECT
public:
    JabberBrowser();
    ~JabberBrowser();
    void goUrl(const QString &url, const QString &node);
    void save();
    void setClient(JabberClient *client);
    DiscoInfo *m_info;
    ListView  *m_list;
signals:
    void enableOptions(bool);
    void addSearch(QWidget*, SIM::Client*, const QString&);
protected slots:
    void selectionChanged();
    void currentChanged(QListViewItem*);
    void dragStart();
    void showReg();
    void showConfig();
    void search();
    void textChanged(const QString&);
    void configFinished();
    void regFinished();
protected:
    void *processEvent(SIM::Event*);
    void setNavigation();
    void stop(const QString &err);
    void go(const QString &url, const QString &node);
    void addHistory(const QString &str);
    bool haveFeature(const char*);
    bool haveFeature(const char*, const QString&);
    QListViewItem *findItem(unsigned col, const char *id);
    QListViewItem *findItem(unsigned col, const char *id, QListViewItem *item);
    void setItemPict(QListViewItem *item);
    void adjustColumn(QListViewItem *item);
    void loadItem(QListViewItem *item);
    void checkDone();
    bool checkDone(QListViewItem*);
    void startProcess();
    void changeMode();
    void changeMode(QListViewItem *item);
    void showEvent(QShowEvent*);
    bool		 m_bInProcess;
    JabberClient *m_client;
    CToolBar	 *m_bar;
    std::vector<std::string>	m_history;
    std::vector<std::string>	m_nodes;
    QString		 m_historyStr;
    int			 m_historyPos;
    JabberWizard	*m_reg;
    JabberWizard	*m_config;
    JIDSearch	 *m_search;
    std::string		 m_search_id;
    std::string		 m_reg_id;
    std::string		 m_config_id;
    bool		 m_bError;
    friend class DiscoInfo;
};

#endif

