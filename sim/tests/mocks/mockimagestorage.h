/*
 * mockimagestorage.h
 *
 *  Created on: Aug 31, 2011
 */

#ifndef MOCKIMAGESTORAGE_H_
#define MOCKIMAGESTORAGE_H_

#include "imagestorage/imagestorage.h"

namespace MockObjects
{
    class MockImageStorage : public SIM::ImageStorage
    {
    public:
        MOCK_METHOD1(icon, QIcon(const QString& id));
        MOCK_METHOD1(image, QImage(const QString& id));
        MOCK_METHOD1(pixmap, QPixmap(const QString& id));

        MOCK_METHOD1(addIconSet, bool(SIM::IconSet* set));
        MOCK_METHOD1(removeIconset, bool(const QString& id));
		MOCK_METHOD0(getIconSets, QList<SIM::IconSet*>());
		MOCK_METHOD0(textSmiles, QString());
		MOCK_METHOD1(parseAllSmiles, QString(const QString& input));
		MOCK_METHOD1(parseAllSmilesByName, QString(const QString& name));
		MOCK_METHOD0(uniqueSmileKeys, QStringList());
		MOCK_METHOD1(getSmileName, QString(const QString& iconId));
		MOCK_METHOD2(getSmileNamePretty, QString(const QString& iconId, bool localized));
    };
}

#endif /* MOCKIMAGESTORAGE_H_ */
