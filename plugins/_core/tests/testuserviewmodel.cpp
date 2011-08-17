
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "tests/gtest-qt.h"

#include <QPixmap>
#include <QSignalSpy>

#include "core_api.h"
#include "events/eventhub.h"
#include "events/contactevent.h"
#include "roster/userviewmodel.h"
#include "contacts/contactlist.h"
#include "tests/mocks/mockcontactlist.h"
#include "tests/mocks/mockimcontact.h"
#include "tests/mocks/mockimstatus.h"

namespace
{
    using namespace SIM;
    using namespace MockObjects;
    using ::testing::NiceMock;
    using ::testing::Return;

    static const int ContactId = 12;

    class TestUserViewModel : public ::testing::Test
    {
    protected:
        virtual void SetUp()
        {
            SIM::createContactList();
            contactList = SIM::getContactList();
        }

        virtual void TearDown()
        {
            SIM::destroyContactList();
            statuses.clear();
        }

        QList<NiceMockIMStatusPtr> statuses;

        ContactPtr makeContact(int id, bool offline, const QPixmap& icon = QPixmap())
        {
            NiceMockIMStatusPtr imstatus = NiceMockIMStatusPtr(new NiceMock<MockIMStatus>());
            ON_CALL(*imstatus.data(), flag(IMStatus::flOffline)).WillByDefault(Return(offline));
            ON_CALL(*imstatus.data(), icon()).WillByDefault(Return(icon));
            statuses.append(imstatus);

            NiceMockIMContactPtr imcontact = NiceMockIMContactPtr(new NiceMock<MockIMContact>());
            ON_CALL(*imcontact.data(), status()).WillByDefault(Return(imstatus));

            ContactPtr contact = ContactPtr(new Contact(id));
            contact->addClientContact(imcontact);
            return contact;
        }

        ContactPtr insertContactToContactList(bool offline)
        {
            QList<int> contactIds;
            contactIds.append(ContactId);
            ContactPtr contact = makeContact(ContactId, offline);
            contactList->addContact(contact);
            return contact;
        }

        ContactPtr insertContactToContactListWithStatusIcon(bool offline, const QPixmap& icon)
        {
            QList<int> contactIds;
            contactIds.append(ContactId);
            ContactPtr contact = makeContact(ContactId, offline, icon);
            contactList->addContact(contact);
            return contact;
        }

        QPixmap makeTestIcon()
        {
            QPixmap img(5, 5);
            img.fill(0xff123456);
            return img;
        }

        QModelIndex createContactIndex(UserViewModel& model, int row, bool offline)
        {
            int offRow = UserViewModel::OfflineRow;
            int onRow = UserViewModel::OnlineRow;
            return model.index(row, 0, model.index(offline ? offRow : onRow, 0));
        }

        SIM::ContactList* contactList;
    };

    TEST_F(TestUserViewModel, columnCount_returns1)
    {
        UserViewModel model(contactList);

        ASSERT_EQ(1, model.columnCount());
    }

    TEST_F(TestUserViewModel, rowCount_withoutParent_returns1)
    {
        UserViewModel model(contactList);

        int rows = model.rowCount();

        ASSERT_EQ(1, rows);
    }

    TEST_F(TestUserViewModel, rowCount_withoutParent_returns2_withShowOffline)
    {
        UserViewModel model(contactList);
        model.setShowOffline(true);

        int rows = model.rowCount();

        ASSERT_EQ(2, rows);
    }

    TEST_F(TestUserViewModel, index_withoutParent_correct)
    {
        UserViewModel model(contactList);
        QModelIndex index;

        index = model.index(0, 0);
        ASSERT_TRUE(index.isValid()) << "Online group index";

        index = model.index(1, 0);
        ASSERT_TRUE(index.isValid()) << "Offline group index";
    }

    TEST_F(TestUserViewModel, rowCount_online)
    {
        insertContactToContactList(false);
        UserViewModel model(contactList);

        int rows = model.rowCount(model.index(UserViewModel::OnlineRow, 0));

        ASSERT_EQ(1, rows);
    }

    TEST_F(TestUserViewModel, rowCount_offline)
    {
        insertContactToContactList(true);
        UserViewModel model(contactList);

        int rows = model.rowCount(model.index(UserViewModel::OfflineRow, 0));

        ASSERT_EQ(1, rows);
    }

    TEST_F(TestUserViewModel, data_online_name)
    {
        ContactPtr contact = insertContactToContactList(false);
        contact->setName("Foo");
        UserViewModel model(contactList);

        QModelIndex index = createContactIndex(model, 0, false);
        ASSERT_EQ(0, index.column());
        QString contactName = model.data(index, Qt::DisplayRole).toString();

        ASSERT_EQ(contact->name(), contactName);
    }

    TEST_F(TestUserViewModel, data_offline_name)
    {
        ContactPtr contact = insertContactToContactList(true);
        contact->setName("Bar");
        UserViewModel model(contactList);

        QModelIndex index = createContactIndex(model, 0, true);
        ASSERT_EQ(0, index.column());
        QString contactName = model.data(index, Qt::DisplayRole).toString();

        ASSERT_EQ(contact->name(), contactName);
    }

    TEST_F(TestUserViewModel, data_offline_icon)
    {
        QPixmap icon = makeTestIcon();
        ContactPtr contact = insertContactToContactListWithStatusIcon(true, icon);
        contact->setName("Bar");
        UserViewModel model(contactList);

        QModelIndex index = createContactIndex(model, 0, true);
        ASSERT_EQ(0, index.column());
        QVariant currentIconVariant = model.data(index, Qt::DecorationRole);

        ASSERT_TRUE(currentIconVariant.isValid());
        QPixmap currentIcon = currentIconVariant.value<QPixmap>();
        ASSERT_TRUE(currentIcon.toImage() == icon.toImage());
    }

    TEST_F(TestUserViewModel, reactsToEvent_contactChangeStatus)
    {
        ContactPtr contact = insertContactToContactList(false);
        contact->setName("Foo");
        UserViewModel model(contactList);
        QSignalSpy spy(&model, SIGNAL(dataChanged(QModelIndex, QModelIndex)));

        SIM::getEventHub()->triggerEvent("contact_change_status", SIM::ContactEventData::create(ContactId));

        ASSERT_EQ(1, spy.count());
    }
}
