/*
 * iqrequest.h
 */

#ifndef IQREQUEST_H_
#define IQREQUEST_H_

#include <QObject>
#include <QSharedPointer>
#include <QDomElement>

class IqRequest : public QObject
{
    Q_OBJECT
public:
    IqRequest();
    virtual ~IqRequest();

	void setBody(const QString& body) { m_body = body; }
	QString body() const { return m_body; }

    void setId(int id) { m_id = id; }
    int id() const { return m_id; }

    void setRoot(const QDomElement& root) { m_root = root; }
    QDomElement root() { return m_root; }

signals:
    void done();

private:
    int m_id;
    QDomElement m_root;
	QString m_body;

};
typedef QSharedPointer<IqRequest> IqRequestPtr;

#endif /* IQREQUEST_H_ */
