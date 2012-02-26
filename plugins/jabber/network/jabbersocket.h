/*
 * jabbersocket.h
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#ifndef JABBERSOCKET_H_
#define JABBERSOCKET_H_

#include <QObject>
#include <QString>
#include <QIODevice>

class JabberSocket : public QObject
{
    Q_OBJECT
public:
    virtual ~JabberSocket() {}

    virtual void connectToHost(const QString& host, int port) = 0;
    virtual void disconnectFromHost() = 0;
	
	virtual void startTls() = 0;

    virtual void send(const QByteArray& data) = 0;
    virtual int dataAvailable() = 0;

    virtual QIODevice* inputStream() = 0;

signals:
    void connected();
    void newData();
	void tlsHandshakeDone();

};

#endif /* JABBERSOCKET_H_ */
