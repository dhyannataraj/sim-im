/*
 * iqcontroller.h
 */

#ifndef IQCONTROLLER_H_
#define IQCONTROLLER_H_

#include <QObject>
#include <QList>
#include "taghandler.h"
#include "iqrequest.h"
#include "network/jabbersocket.h"

class IqController : public QObject, public TagHandler
{
    Q_OBJECT
public:
    IqController();
    virtual ~IqController();

	void setSocket(JabberSocket* socket);

    virtual bool canHandle(const QString& tagName) const;

    virtual void startElement(const QDomElement& root);
    virtual void endElement(const QString& name);
    virtual void characters(const QString& ch);

    void sendRequest(const IqRequestPtr& request);

private:
	struct RequestEntry
	{
		int id;
		int timestamp;
		IqRequestPtr request;
	};

	QList<RequestEntry> m_requests;

    int m_currentId;

	JabberSocket* m_socket;
};

#endif /* IQCONTROLLER_H_ */
