/***************************************************************************
                          icqsearch.h  -  description
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

#ifndef _ICQSEARCH_H
#define _ICQSEARCH_H

#include "simapi.h"
#include "stl.h"
#include "icqsearchbase.h"

class ICQClient;
class AdvSearch;
class AIMSearch;
class GroupRadioButton;

class ICQSearch : public ICQSearchBase, public EventReceiver
{
    Q_OBJECT
public:
    ICQSearch(ICQClient *client, QWidget *parent);
    ~ICQSearch();
signals:
    void setAdd(bool);
    void addResult(QWidget*);
    void showResult(QWidget*);
    void setColumns(const QStringList&, int, QWidget*);
    void addItem(const QStringList&, QWidget*);
    void searchDone(QWidget*);
protected slots:
    void advDestroyed();
    void radioToggled(bool);
    void advClick();
    void search();
	void searchStop();
	void searchMail(const QString&);
	void searchName(const QString&, const QString&, const QString&);
	void createContact(const QString&, unsigned tmpFlags, Contact *&contact);
	void createContact(unsigned tmpFlags, Contact *&contact);
protected:
    enum SearchType
    {
        None,
        UIN,
        Mail,
        Name,
        Full
    };
    void *processEvent(Event*);
    void showEvent(QShowEvent*);
    void setAdv(bool);
    void icq_search();
	void addColumns();
    void add(const QString &screen, unsigned tmpFlags, Contact *&contact);
    list<unsigned>		m_uins;
    ICQClient			*m_client;
    QWidget				*m_adv;
    bool				m_bAdv;
    bool				m_bAdd;
    SearchType			m_type;
    unsigned short		m_id_icq;
    unsigned short		m_id_aim;
    unsigned long		m_uin;
    string				m_first;
    string				m_last;
    string				m_nick;
    string				m_mail;
    unsigned short		m_age;
    char				m_gender;
    unsigned short		m_lang;
    string				m_city;
    string				m_state;
    unsigned short		m_country;
    string				m_company;
    string				m_depart;
    string				m_position;
    unsigned short		m_occupation;
    unsigned short		m_past;
    string				m_past_text;
    unsigned short		m_interests;
    string				m_interests_text;
    unsigned short		m_affilations;
    string				m_affilations_text;
    string				m_keywords;
    bool				m_bOnline;
};

#endif

