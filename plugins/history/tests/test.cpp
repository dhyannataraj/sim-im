
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
#include "events/actioncollectionevent.h"

void registerEvents()
{
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("init_abort"));
    SIM::getEventHub()->registerEvent(SIM::StandardEvent::create("quit"));
    SIM::getEventHub()->registerEvent(SIM::ActionCollectionEvent::create("contact_menu"));
    SIM::getEventHub()->registerEvent(SIM::LogEvent::create());
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    SIM::createEventHub();
    registerEvents();
    StubObjects::StubImageStorage imagestorage;
    SIM::setImageStorage(&imagestorage);
    SIM::createProfileManager("");
    SIM::createAvatarStorage();
    SIM::createCommandHub();
    SIM::createContactList();
    int ret = RUN_ALL_TESTS();
#ifdef WIN32
    getchar();
#endif
    SIM::destroyContactList();
    SIM::destroyCommandHub();
    SIM::destroyAvatarStorage();
    SIM::destroyProfileManager();
    return ret;
}


