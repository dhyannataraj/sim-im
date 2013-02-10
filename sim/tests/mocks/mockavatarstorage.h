/*
 * mockavatarstorage.h
 *
 *  Created on: Jul 12, 2011
 */

#ifndef MOCKAVATARSTORAGE_H_
#define MOCKAVATARSTORAGE_H_

#include "gmock/gmock.h"
#include "imagestorage/avatarstorage.h"

namespace MockObjects
{
    class MockAvatarStorage : public SIM::AvatarStorage
    {
    public:
        virtual ~MockAvatarStorage() {}

        MOCK_METHOD3(addAvatar, void(const SIM::IMContactId& contactId, const QImage& image, const QString& type));
        MOCK_METHOD2(getAvatar, QImage(const SIM::IMContactId& contactId, const QString& type));

        MOCK_CONST_METHOD0(id, QString());
        MOCK_METHOD1(hasIcon, bool(const QString& iconId));
        MOCK_METHOD1(icon, QIcon(const QString& iconId));
        MOCK_METHOD1(image, QImage(const QString& iconId));
        MOCK_METHOD1(pixmap, QPixmap(const QString& iconId));

		MOCK_METHOD1(hasSmile, bool(const QString& txtSmile));
		MOCK_METHOD0(textSmiles, QStringList());
		MOCK_METHOD1(parseSmiles, QString(const QString& input));
		MOCK_METHOD1(parseAllSmiles, QString(const QString& input));
		MOCK_METHOD2(parseAllSmilesByName, void(const QString& name, QIcon& icon));
		MOCK_METHOD1(getSmileName, QString(const QString& iconId));
		MOCK_METHOD2(getSmileNamePretty, QString(const QString& iconId, bool localized));

    };
}


#endif /* MOCKAVATARSTORAGE_H_ */
