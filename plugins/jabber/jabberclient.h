/***************************************************************************
                          jabberclient.h  -  description
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

#ifndef _JABBERCLIENT_H
#define _JABBERCLIENT_H

#include <vector>
#include <list>
#include <QStack>
#include <QByteArray>

#include "simapi.h"
#include "contacts/imcontact.h"
#include "jabberstatus.h"
#include "log.h"
#include "jabbercontact.h"
#include "jabbergroup.h"
#include "contacts/client.h"
#include "jabber_api.h"

#include "network/jabbersocket.h"
#include "protocol/inputstreamdispatcher.h"
#include "protocol/jabberauthenticationcontroller.h"

using namespace std;

class JabberProtocol;
class JabberClient;

const unsigned JABBER_SIGN		= 0x0002;

const unsigned SUBSCRIBE_NONE	= 0;
const unsigned SUBSCRIBE_FROM	= 1;
const unsigned SUBSCRIBE_TO		= 2;
const unsigned SUBSCRIBE_BOTH	= (SUBSCRIBE_FROM | SUBSCRIBE_TO);

struct JabberClientData
{
public:
    JabberClientData(JabberClient* client);
    virtual QByteArray serialize();
    virtual void deserialize(Buffer* cfg);
    virtual void serialize(QDomElement& /*element*/) {}
    virtual void deserialize(QDomElement& /*element*/) {}

    virtual SIM::Client* client() { Q_ASSERT_X(false, "JabberClientData::client", "Shouldn't be called"); return 0; }

	QString getID() const { return m_ID;}
    void setID(const QString& ID) { m_ID = ID; }

    QString getServer() const { return m_server;}
    void setServer(const QString& server) { m_server = server; }

    unsigned long getPort() const { return m_port; }
    void setPort(unsigned long port) { m_port = port; }

    bool getUseSSL() const { return m_useSSL; }
    void setUseSSL(bool b) { m_useSSL = b; }

    bool getUsePlain() const { return m_usePlain; }
    void setUsePlain(bool b) { m_usePlain = b; }

    bool getUseVHost() const { return m_useVHost; }
    void setUseVHost(bool b) { m_useVHost = b; }

    bool getRegister() const { return m_register; }
    void setRegister(bool b) { m_register = b; }

    unsigned long getPriority() const { return m_priority; }
    void setPriority(unsigned long priority) { m_priority = priority; }

    QString getListRequest() const { return m_listRequest; }
    void setListRequest(const QString& list) { m_listRequest = list; }

    QString getVHost() const { return m_vHost; }
    void setVHost(const QString& vhost) { m_vHost = vhost; }

    bool isTyping() const { return m_typing; }
    void setTyping(bool t) { m_typing = t; }

    bool isRichText() const { return m_richText; }
    void setRichText(bool rt) { m_richText = rt; }

    bool getUseVersion() const { return m_useVersion; }
    void setUseVersion(bool b) { m_useVersion = b; }

    bool getProtocolIcons() const { return m_protocolIcons; }
    void setProtocolIcons(bool b) { m_protocolIcons = b; } 

    unsigned long getMinPort() const { return m_minPort; }
    void setMinPort(unsigned long port) { m_minPort = port; }

    unsigned long getMaxPort() const { return m_maxPort; }
    void setMaxPort(unsigned long port) { m_maxPort = port; }

    QString getPhoto() const { return m_photo; }
    void setPhoto(const QString& photo) { m_photo = photo; }

    QString getLogo() const { return m_logo; }
    void setLogo(const QString& logo) { m_logo = logo; }

    bool getAutoSubscribe() const { return m_autoSubscribe; }
    void setAutoSubscribe(bool b) { m_autoSubscribe = b; }

    bool getAutoAccept() const { return m_autoAccept; }
    void setAutoAccept(bool b) { m_autoAccept = b; }

    bool getUseHttp() const { return m_useHttp; }
    void setUseHttp(bool b) { m_useHttp = b; }

    QString getUrl() const { return m_url; }
    void setUrl(const QString& url) { m_url = url; }

    bool getInfoUpdated() const { return m_infoUpdated; }
    void setInfoUpdated(bool b) { m_infoUpdated = b; }

    JabberContactPtr owner;

    virtual void deserializeLine(const QString& key, const QString& value);
public:

	QString m_ID;
    QString m_server;
    unsigned long m_port;
    bool m_useSSL;
    bool m_usePlain;
    bool m_useVHost;
    bool m_register;
    QString m_resource;
    unsigned long m_priority;
    QString m_listRequest;
    QString m_vHost;
    bool m_typing;
    bool m_richText;
    bool m_useVersion;
    bool m_protocolIcons;
    unsigned long m_minPort;
    unsigned long m_maxPort;
    QString m_photo;
    QString m_logo;
    bool m_autoSubscribe;
    bool m_autoAccept;
    bool m_useHttp;
    QString m_url;
    bool m_infoUpdated;
};


struct JabberListRequest
{
    QString             jid;
    QString             grp;
    QString             name;
    bool                bDelete;
};

struct DiscoItem
{
    QString             id;
    QString             jid;
    QString             node;
    QString             name;
    QString             type;
    QString             category;
    QString             features;
};

// XEP-0092 Software Version
struct ClientVersionInfo
{
    QString             jid;
    QString             node;
    QString             name;
    QString             version;
    QString             os;
};

// XEP-0012 Last Activity
struct ClientLastInfo
{
    QString             jid;
    unsigned int        seconds;
};

// XEP-0090 Entity Time
struct ClientTimeInfo
{
    QString             jid;
    QString             utc;
    QString             tz;
    QString             display;
};

class JABBER_EXPORT JabberClient : public QObject, public SIM::Client
{
    Q_OBJECT
public:

    JabberClient(JabberProtocol*, const QString& name);
    virtual ~JabberClient();
    virtual QString name();
	virtual QString retrievePasswordLink();
    virtual SIM::IMContactPtr createIMContact();
    virtual void addIMContact(const SIM::IMContactPtr& contact);
    virtual SIM::IMContactPtr getIMContact(const SIM::IMContactId& id);
    virtual SIM::IMGroupPtr createIMGroup();

    virtual QWidget* createSetupWidget(const QString& id, QWidget* parent);
    virtual void destroySetupWidget();
    virtual QStringList availableSetupWidgets() const;

    virtual QWidget* createStatusWidget();

    virtual SIM::IMStatusPtr currentStatus();
    virtual void changeStatus(const SIM::IMStatusPtr& status);
    virtual SIM::IMStatusPtr savedStatus();

    virtual SIM::IMContactPtr ownerContact();
    virtual void setOwnerContact(SIM::IMContactPtr contact);

    virtual bool deserialize(Buffer* cfg);
    virtual bool loadState(SIM::PropertyHubPtr state);
    virtual SIM::PropertyHubPtr saveState();

    virtual SIM::MessageEditorFactory* messageEditorFactory() const;

    virtual QWidget* createSearchWidow(QWidget *parent);
    virtual QList<SIM::IMGroupPtr> groups();
    virtual QList<SIM::IMContactPtr> contacts();

    JabberStatusPtr getDefaultStatus(const QString& id) const;

    void setID(const QString &id);
    QString getID() const;
    
    QString getUsername() const;

    QString getServer() const;
    void setServer(const QString& server);

    QString getVHost() const;
    void setVHost(const QString& vhost);

    unsigned short getPort() const;
    void setPort(unsigned long port);
    
    bool getUseSSL() const;
    void setUseSSL(bool b);

    bool getUsePlain() const;
    void setUsePlain(bool b);

    bool getUseVHost() const;
    void setUseVHost(bool b);

    bool getRegister() const;
    void setRegister(bool b);

    QString getResource() const;
    void setResource(const QString& resource);

    unsigned long getPriority() const;
    void setPriority(unsigned long p);

    QString getListRequest() const;
    void setListRequest(const QString& request);

    bool getTyping() const;
    void setTyping(bool t);

    bool getRichText() const;
    void setRichText(bool rt);

    bool getUseVersion();
    void setUseVersion(bool b);

    bool getProtocolIcons() const;
    void setProtocolIcons(bool b);

    unsigned long getMinPort() const;
    void setMinPort(unsigned long port);

    unsigned long getMaxPort() const;
    void setMaxPort(unsigned long port);

    QString getPhoto() const;
    void setPhoto(const QString& photo);

    QString getLogo() const;
    void setLogo(const QString& logo);

    bool getAutoSubscribe() const;
    void setAutoSubscribe(bool b);

    bool getAutoAccept() const;
    void setAutoAccept(bool b);
    
    bool getUseHTTP() const;
    void setUseHTTP(bool b);

    QString getURL() const;
    void setURL(const QString& url);

    bool getInfoUpdated() const;
    void setInfoUpdated(bool b);

    JabberClientData* clientPersistentData;

protected:

    void init();
    void addDefaultStates();


private:
    QString m_name;
    QString m_resource;
    QList<JabberStatusPtr> m_defaultStates;
    JabberStatusPtr m_currentStatus;
    JabberStatusPtr m_nextStatus;

    JabberSocket* m_socket;
    InputStreamDispatcher* m_dispatcher;

    JabberAuthenticationController::SharedPointer m_auth;
};

#endif

// vim: set expandtab:


