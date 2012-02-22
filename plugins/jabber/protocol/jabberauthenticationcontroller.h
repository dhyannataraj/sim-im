/*
 * jabberauthenticationcontroller.h
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#ifndef JABBERAUTHENTICATIONCONTROLLER_H_
#define JABBERAUTHENTICATIONCONTROLLER_H_

#include "network/jabbersocket.h"
#include "taghandler.h"
#include <QSharedPointer>

class JabberAuthenticationController : public QObject, public TagHandler
{
    Q_OBJECT
public:
    typedef QSharedPointer<JabberAuthenticationController> SharedPointer;

    JabberAuthenticationController();
    virtual ~JabberAuthenticationController();

    void startAuthentication(const QString& host, int port);

    void setSocket(JabberSocket* socket);

    virtual QString element() const;

    virtual void startElement(const QString& name, const QXmlAttributes);
    virtual void endElement(const QString& name);
    virtual void characters(const QString& ch);
public slots:
    void connected();

private:
    JabberSocket* m_socket;
};

#endif /* JABBERAUTHENTICATIONCONTROLLER_H_ */
