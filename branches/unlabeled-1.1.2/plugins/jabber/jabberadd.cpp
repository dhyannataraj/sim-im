/***************************************************************************
                          jabberadd.cpp  -  description
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

#include "jabberadd.h"
#include "jabber.h"
#include "jabberclient.h"
#include "jabbersearch.h"
#include "addresult.h"

#include <qtabwidget.h>
#include <qwizard.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qvalidator.h>

class IdValidator : public QValidator
{
public:
    IdValidator(QWidget *parent);
    virtual QValidator::State validate(QString &input, int &pos) const;
};

IdValidator::IdValidator(QWidget *parent)
        : QValidator(parent)
{
}

QValidator::State IdValidator::validate(QString &input, int &pos) const
{
    QString id = input;
    QString host;
    int p = input.find('@');
    if (p >= 0){
        id = input.left(p);
        host = input.mid(p + 1);
    }
    QRegExp r("[A-Za-z0-9\\.\\-_]+");
    if (id.length() == 0)
        return Intermediate;
    int len = 0;
    if ((r.match(id, 0, &len) != 0) || (len != (int)id.length())){
        pos = input.length();
        return Invalid;
    }
    if (host.length()){
        if ((r.match(id, 0, &len) != 0) || (len != (int)id.length())){
            pos = input.length();
            return Invalid;
        }
    }
    return Acceptable;
}

JabberAdd::JabberAdd(JabberClient *client)
{
    m_client = client;
    m_wizard = NULL;
    m_result = NULL;
    m_idValidator = new IdValidator(edtID);
    edtID->setValidator(m_idValidator);
    connect(tabAdd, SIGNAL(currentChanged(QWidget*)), this, SLOT(currentChanged(QWidget*)));
    connect(edtID, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(edtID, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));
    QStringList services;
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *c = getContacts()->getClient(i);
        if ((c->protocol() != client->protocol()) || (c->getState() != Client::Connected))
            continue;
        JabberClient *jc = static_cast<JabberClient*>(c);
        QString vHost = QString::fromUtf8(jc->data.owner.VHost);
        QStringList::Iterator it;
        for (it = services.begin(); it != services.end(); ++it){
            if ((*it) == vHost)
                break;
        }
        if (it != services.end())
            continue;
        services.append(vHost);
    }
    cmbServices->insertStringList(services);
    connect(cmbServices, SIGNAL(activated(const QString&)), this, SLOT(serviceChanged(const QString&)));
    serviceChanged(cmbServices->currentText());
}

JabberAdd::~JabberAdd()
{
    if (m_result)
        delete m_result;
}

void JabberAdd::currentChanged(QWidget*)
{
    if (m_result)
        m_result->showSearch(tabAdd->currentPageIndex() != 0);
    textChanged("");
}

void JabberAdd::search()
{
    if ((m_wizard == NULL) || !m_wizard->nextButton()->isEnabled())
        return;
    emit goNext();
}

void JabberAdd::textChanged(const QString&)
{
    bool bSearch = false;
    if (tabAdd->currentPageIndex() == 0){
        bSearch = edtID->text().length();
        if (bSearch){
            int pos = 0;
            QString text = edtID->text();
            if (!m_idValidator->validate(text, pos))
                bSearch = false;
        }
    }else if (tabAdd->currentPage()->inherits("JabberSearch")){
        bSearch = static_cast<JabberSearch*>(tabAdd->currentPage())->canSearch();
    }
    if (m_wizard)
        m_wizard->setNextEnabled(this, bSearch);
}

void JabberAdd::showEvent(QShowEvent *e)
{
    JabberAddBase::showEvent(e);
    if (m_wizard == NULL){
        m_wizard = static_cast<QWizard*>(topLevelWidget());
        connect(this, SIGNAL(goNext()), m_wizard, SLOT(goNext()));
    }
    if (m_result == NULL){
        m_result = new AddResult(m_client);
        connect(m_result, SIGNAL(finished()), this, SLOT(addResultFinished()));
        connect(m_result, SIGNAL(search()), this, SLOT(startSearch()));
        m_wizard->addPage(m_result, i18n("Add Jabber contact"));
    }
    currentChanged(NULL);
}

void JabberAdd::addResultFinished()
{
    m_result = NULL;
}

void JabberAdd::startSearch()
{
    if (m_result == NULL)
        return;
    JabberClient *client = findClient(cmbServices->currentText().latin1());
    if (client == NULL)
        return;
    if (tabAdd->currentPageIndex() == 0){
        m_result->addContact(client, edtID->text());
    }else if (tabAdd->currentPage()->inherits("JabberSearch")){
        JabberSearch *search = static_cast<JabberSearch*>(tabAdd->currentPage());
        QString condition = search->condition();
        string search_id = client->search(search->id(), condition.utf8());
        m_result->setSearch(client, search_id.c_str());
    }
}

JabberClient *JabberAdd::findClient(const char *host)
{
    for (unsigned i = 0; i < getContacts()->nClients(); i++){
        Client *client = getContacts()->getClient(i);
        if ((client->protocol() != m_client->protocol()) || (client->getState() != Client::Connected))
            continue;
        JabberClient *jc = static_cast<JabberClient*>(client);
        if (!strcmp(jc->data.owner.VHost, host))
            return jc;
    }
    return NULL;
}

void JabberAdd::serviceChanged(const QString &host)
{
    JabberClient *client = findClient(host.latin1());
    for (list<JabberSearch*>::iterator it = m_search.begin(); it != m_search.end(); ++it){
        tabAdd->removePage(*it);
        delete *it;
    }
    m_search.clear();
    if (client)
        client->get_agents();
}

void *JabberAdd::processEvent(Event *e)
{
    if (e->type() == static_cast<JabberPlugin*>(m_client->protocol()->plugin())->EventAgentFound){
        JabberAgentInfo *data = (JabberAgentInfo*)(e->param());
        if ((cmbServices->currentText() == data->VHost) && data->Search){
            JabberClient *client = findClient(cmbServices->currentText().latin1());
            if (client){
                JabberSearch *search = new JabberSearch(this, client, data->ID, QString::fromUtf8(data->Name));
                m_search.push_back(search);
                tabAdd->addTab(search, QString::fromUtf8(data->Name));
            }
        }
    }
    return NULL;
}

#ifndef WIN32
#include "jabberadd.moc"
#endif

