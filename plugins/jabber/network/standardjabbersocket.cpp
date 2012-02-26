/*
 * standardjabbersocket.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "standardjabbersocket.h"
#include "log.h"

using namespace SIM;


StandardJabberSocket::StandardJabberSocket()
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(readReady()));
}

StandardJabberSocket::~StandardJabberSocket()
{
}


void StandardJabberSocket::connectToHost(const QString& host, int port)
{
    m_host = host;
    m_socket.connectToHost(host, port);
}

void StandardJabberSocket::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

void StandardJabberSocket::startTls()
{
	log(L_DEBUG, "StandardJabberSocket::startTls()");
	m_socket.startClientEncryption();
}

void StandardJabberSocket::send(const QByteArray& data)
{
    m_socket.write(data);
}

int StandardJabberSocket::dataAvailable()
{
    return m_socket.bytesAvailable();
}

QIODevice* StandardJabberSocket::inputStream()
{
    return &m_socket;
}

void StandardJabberSocket::slot_connected()
{
    emit connected();
}

void StandardJabberSocket::readReady()
{
    log(L_DEBUG, "readyRead");
    emit newData();
}

void StandardJabberSocket::encrypted()
{
	emit tlsHandshakeDone();
}
