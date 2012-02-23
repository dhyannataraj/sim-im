/*
 * testjabberauthenticationcontroller.cpp
 *
 *  Created on: Feb 10, 2012
 *      Author: todin
 */

#include "../protocol/jabberauthenticationcontroller.h"

#include "gtest/gtest.h"

#include "mocks/mockjabbersocket.h"

namespace
{
    using namespace testing;

    class TestJabberAuthenticationController : public Test
    {
    public:
        virtual void SetUp()
        {
            sock = new MockObjects::MockJabberSocket();
            auth.setSocket(sock);
        }

        virtual void TearDown()
        {
            delete sock;
        }

        JabberAuthenticationController auth;
        MockObjects::MockJabberSocket* sock;
    };

    TEST_F(TestJabberAuthenticationController, afterConnect_startsStream)
    {
        EXPECT_CALL(*sock, send(_));

        auth.connected();
    }
}
