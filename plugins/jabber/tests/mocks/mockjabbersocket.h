/*
 * mockjabbersocket.h
 *
 *  Created on: Feb 18, 2012
 *      Author: todin
 */

#ifndef MOCKJABBERSOCKET_H_
#define MOCKJABBERSOCKET_H_

#include "gmock/gmock.h"
#include "network/jabbersocket.h"

namespace MockObjects
{
    class MockJabberSocket : public JabberSocket
    {
    public:
        MOCK_METHOD2(connectToHost, void(const QString& host, int port));
        MOCK_METHOD0(disconnectFromHost, void());

        MOCK_METHOD0(startStream, void());

        MOCK_METHOD1(send, void(const QByteArray& data));
        MOCK_METHOD0(dataAvailable, int());

        MOCK_METHOD0(inputStream, QIODevice*());
    };
}

#endif /* MOCKJABBERSOCKET_H_ */
