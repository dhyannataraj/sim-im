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

    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setHostname(const QString& hostname);
    void setResource(const QString& resource);
    QString fullJid() const { return m_fullJid; }

    void startAuthentication(const QString& host, int port);

    void setSocket(JabberSocket* socket);

    virtual bool canHandle(const QString& tagName) const;

    virtual void startElement(const QDomElement& root);
    virtual void endElement(const QString& name);
    virtual void characters(const QString& ch);

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
