/*
 * jabberauthenticationcontroller.h
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#ifndef JABBERAUTHENTICATIONCONTROLLER_H_
#define JABBERAUTHENTICATIONCONTROLLER_H_

#include "../network/jabbersocket.h"
#include "taghandler.h"
#include <QSharedPointer>

class JabberAuthenticationController : public QObject, public TagHandler
{
    Q_OBJECT
public:
    typedef QSharedPointer<JabberAuthenticationController> SharedPointer;

    JabberAuthenticationController();
    virtual ~JabberAuthenticationController();

    virtual void streamOpened();
    virtual void incomingStanza(const XmlElement::Ptr& element);

    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setHostname(const QString& hostname);
    void setResource(const QString& resource);
    QString fullJid() const { return m_fullJid; }

    void startAuthentication(const QString& host, int port);

    void setSocket(JabberSocket* socket);

public slots:
    void connected();
	void tlsHandshakeDone();

signals:
	void newStream();
	void authenticationCompleted();

private:
	QString makeResponseToChallenge(const QString& challengeString);

    JabberSocket* m_socket;
    QString m_host;
	QList<QString> m_features;
	enum State
	{
		Initial,
		TlsNegotiation,
		ReadyToAuthenticate,
		DigestMd5WaitingChallenge,
		DigestMd5WaitingSecondChallenge,
		DigestMd5WaitingSuccess,
		RestartingStream,
		Authenticated,
		ResourceBinding,
		Error
	};
	State m_state;

    QString m_username;
    QString m_hostname;
    QString m_password;
    QString m_resource;
    QString m_fullJid;
};

#endif /* JABBERAUTHENTICATIONCONTROLLER_H_ */
