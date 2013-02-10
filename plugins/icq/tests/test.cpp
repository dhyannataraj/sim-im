#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QApplication>
#include "events/eventhub.h"
#include "imagestorage/imagestorage.h"
#include "imagestorage/avatarstorage.h"
#include "profile/profilemanager.h"
#include "commands/commandhub.h"
#include "events/standardevent.h"
#include "events/logevent.h"
#include "contacts/contactlist.h"
#include "tests/stubs/stubimagestorage.h"
#include "messaging/messagepipe.h"
#include "messaging/messageoutpipe.h"
#include "core.h"
#include "services.h"
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
    auto services = SIM::makeMockServices();
    SIM::createEventHub();
    registerEvents();
    StubObjects::StubImageStorage imagestorage;
    SIM::setImageStorage(&imagestorage);
    SIM::createAvatarStorage(services->profileManager());
    SIM::createCommandHub();
    SIM::createMessagePipe();
    SIM::createOutMessagePipe();
    CorePlugin* core = new CorePlugin(SIM::makeMockServices());
    int ret = RUN_ALL_TESTS();
    delete core;
#ifdef WIN32
    getchar();
#endif
    return ret;
}


