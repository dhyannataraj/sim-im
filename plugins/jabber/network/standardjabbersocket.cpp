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

void StandardJabberSocket::connectToHost(const QString& host, int port)
{
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
    return m_buffer.size();
}

QIODevice* StandardJabberSocket::inputStream()
{
    return m_socket;
}

void StandardJabberSocket::slot_connected()
{
    emit connected();
}

void StandardJabberSocket::readReady()
{
    QByteArray data = m_socket.readAll();
    for(int i = 0; i < data.size(); i++)
    {
        m_buffer.push_back(data[i]);
    }
    emit newData();
}
