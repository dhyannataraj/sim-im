
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QApplication>
#include "events/eventhub.h"
#include "imagestorage/imagestorage.h"
#include "imagestorage/avatarstorage.h"
#include "commands/commandhub.h"
#include "events/standardevent.h"
#include "events/logevent.h"

#include "messaging/messagepipe.h"
#include "messaging/messageoutpipe.h"

#include "contacts/contactlist.h"
#include "tests/stubs/stubimagestorage.h"
#include "tests/mocks/mockimagestorage.h"

#include <QModelIndex>
#include "test.h"
#include "tests/simlib-testing.h"

void registerEvents()
{
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init_abort"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("quit"));
    SIM::getEventHub()->registerEvent(SIM::LogEvent::create());
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    qRegisterMetaType<QModelIndex>("QModelIndex");
    auto services = SIM::makeMockServices();
    SIM::createEventHub();
    SIM::createAvatarStorage(services->profileManager());
    SIM::createMessagePipe();
    SIM::createOutMessagePipe();
    SIM::createCommandHub();
    registerEvents();
    int ret = RUN_ALL_TESTS();
#ifdef WIN32
    getchar();
#endif
    SIM::destroyAvatarStorage();
    SIM::destroyOutMessagePipe();
    SIM::destroyMessagePipe();
	return ret;
}

