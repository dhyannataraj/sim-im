/*
 * standardjabbersocket.h
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#ifndef STANDARDJABBERSOCKET_H_
#define STANDARDJABBERSOCKET_H_

#include "jabbersocket.h"
#include <boost/circular_buffer.hpp>
#include <QTcpServer>

class StandardJabberSocket : public JabberSocket
{
    Q_OBJECT
public:
    StandardJabberSocket();
    virtual ~StandardJabberSocket();

    virtual void connectToHost(const QString& host, int port) ;
    virtual void disconnectFromHost();

    virtual void send(const QByteArray& data);
    virtual int dataAvailable();

    virtual QIODevice* inputStream();

private slots:
    void slot_connected();
    void readReady();

private:
    boost::circular_buffer<char> m_buffer;
    QTcpSocket m_socket;
};

#endif /* STANDARDJABBERSOCKET_H_ */
