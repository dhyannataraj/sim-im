/***************************************************************************
                          aimsearch.cpp  -  description
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

#include "aimsearch.h"
#include "icqsearch.h"
#include "searchresult.h"
#include "icqclient.h"

#include <qlineedit.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qwizard.h>
#include <qpushbutton.h>
#include <qcombobox.h>

AIMSearch::AIMSearch(ICQClient *client)
{
    m_client = client;
    m_wizard = NULL;
    m_result = NULL;
    fillGroups();
    edtScreen->setValidator(new AIMValidator(edtScreen));
    edtUin->setValidator(new QIntValidator(10000, 0x7FFFFFFF, edtUin));
	QStringList l;
	for (const ext_info *c = getCountryCodes(); c->nCode; c++){
		for (const ext_info *cc = getCountries(); cc->nCode; cc++){
			if (cc->nCode == c->nCode){
				l.append(i18n(cc->szName));
				break;
			}
		}
	}
	l.sort();
	cmbCountry->insertItem("");
	cmbCountry->insertStringList(l);
    connect(tabSearch, SIGNAL(currentChanged(QWidget*)), this, SLOT(currentChanged(QWidget*)));
    connect(edtScreen, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(edtUin, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(edtMail, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(edtFirst, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(edtLast, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    connect(edtScreen, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtUin, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtMail, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtFirst, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtLast, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtMidle, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtMaiden, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtNick, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtStreet, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtCity, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtState, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtZip, SIGNAL(returnPressed()), this, SLOT(search()));
}

AIMSearch::~AIMSearch()
{
    if (m_result && m_wizard){
        if (m_wizard->inherits("QWizard"))
            m_wizard->removePage(m_result);
        delete m_result;
    }
}

void *AIMSearch::processEvent(Event *e)
{
    switch (e->type()){
    case EventGroupDeleted:
    case EventGroupChanged:
        fillGroups();
        break;
    }
    return NULL;
}

void AIMSearch::fillGroups()
{
    cmbGroup->clear();
    cmbGroupICQ->clear();
    Group *grp;
    ContactList::GroupIterator it;
    while ((grp = ++it) != NULL){
        if (grp->id() == 0)
            continue;
        cmbGroup->insertItem(grp->getName());
        cmbGroupICQ->insertItem(grp->getName());
    }
}

void AIMSearch::showEvent(QShowEvent *e)
{
    AIMSearchBase::showEvent(e);
    if (m_wizard == NULL){
        m_wizard = static_cast<QWizard*>(topLevelWidget());
        connect(this, SIGNAL(goNext()), m_wizard, SLOT(goNext()));
    }
    if (m_result == NULL){
        m_result = new ICQSearchResult(m_wizard, m_client);
        connect(m_result, SIGNAL(finished()), this, SLOT(resultFinished()));
        connect(m_result, SIGNAL(startSearch()), this, SLOT(startSearch()));
        m_wizard->addPage(m_result, i18n("AIM search results"));
    }
    m_result->clear();
    changed();
}

void AIMSearch::currentChanged(QWidget*)
{
    changed();
}

void AIMSearch::textChanged(const QString&)
{
    changed();
}

void AIMSearch::changed()
{
    bool bSearch = false;
    switch (tabSearch->currentPageIndex()){
    case 0:
        bSearch = !edtScreen->text().isEmpty();
        break;
    case 1:
        bSearch = !edtUin->text().isEmpty();
        break;
    case 2:
        bSearch = !edtMail->text().isEmpty();
        break;
    case 3:
        bSearch = !edtFirst->text().isEmpty() ||
			!edtLast->text().isEmpty();
        break;
    }
    if (m_wizard)
        m_wizard->setNextEnabled(this, bSearch);
}

void AIMSearch::search()
{
    if ((m_wizard == NULL) || !m_wizard->nextButton()->isEnabled())
        return;
    emit goNext();
}

void AIMSearch::resultFinished()
{
    m_result = NULL;
}

void AIMSearch::startSearch()
{
    m_result->clear();
    QString screen;
    QComboBox *cmbGrp;
    unsigned long uin = 0;
	unsigned short id;
    switch (tabSearch->currentPageIndex()){
    case 0:
        screen = edtScreen->text();
        cmbGrp = cmbGroup;
        break;
    case 1:
        uin = atol(edtUin->text().latin1());
        if (uin)
            screen = QString::number(uin);
        cmbGrp = cmbGroupICQ;
        break;
    case 2:
		id = m_client->aimEMailSearch(edtMail->text().utf8());
        return;
    case 3:{
		QString country = cmbCountry->text(cmbCountry->currentItem());
		if (!country.isEmpty()){
			const ext_info *c;
			for (c = getCountries(); c->nCode; c++)
				if (i18n(c->szName) == country)
					break;
			const ext_info *cc;
			for (cc = getCountryCodes(); cc->nCode; cc++)
				if (cc->nCode == c->nCode)
					break;
			if (cc->nCode){
				country = cc->szName;
				country = country.upper();
			}else{
				country = "";
			}
		}
		id = m_client->aimInfoSearch(edtFirst->text().utf8(), edtLast->text().utf8(),
			edtMidle->text().utf8(), edtMaiden->text().utf8(),
			country.utf8(), edtStreet->text().utf8(),
			edtCity->text().utf8(), edtNick->text().utf8(),
			edtZip->text().utf8(), edtState->text().utf8());
        return;
		   }
    }
    if (screen.isEmpty())
        return;
    int nGrp = cmbGrp->currentItem();
    ContactList::GroupIterator it;
    Contact *contact;
    ICQUserData *data = m_client->findContact(screen.latin1(), NULL, false, contact);
    if (data){
        if (contact->getGroup()){
            m_result->setText(i18n("%1 already in contact list") .arg(screen));
            return;
        }
    }else{
        data = m_client->findContact(screen.latin1(), NULL, true, contact);
    }
    Group *grp;
    while ((grp = ++it) != NULL){
        if (grp->id() == 0)
            continue;
        if (nGrp-- == 0){
            contact->setGroup(grp->id());
            Event e(EventContactChanged, contact);
            e.process();
            break;
        }
    }
    m_result->setText(i18n("%1 added to contact list") .arg(screen));
}

#ifndef WIN32
#include "aimsearch.moc"
#endif

