/*
 * standardjabbersocket.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "standardjabbersocket.h"

StandardJabberSocket::StandardJabberSocket()
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(slot_connected()));
    connect(&m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

StandardJabberSocket::~StandardJabberSocket()
{
}

void StandardJabberSocket::startStream()
{
    QString stream("<stream:stream xmlns='jabber:client' "
            "xmlns:stream='http://etherx.jabber.org/streams' to='%1' version='1.0'").
            arg(m_host);
    send(stream.toUtf8());
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
    emit newData();
}
