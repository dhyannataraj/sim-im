#ifndef STANDARDIMAGESTORAGE_H
#define STANDARDIMAGESTORAGE_H

#include "imagestorage.h"

namespace SIM
{
    class StandardImageStorage : public ImageStorage
    {
    public:
        StandardImageStorage();
        virtual ~StandardImageStorage();

        QIcon icon(const QString& id);
        QImage image(const QString& id);
        QPixmap pixmap(const QString& id);

        bool addIconSet(IconSet* set);
        QList<IconSet *> getIconSets();
        QString parseAllSmiles(const QString& input);
        bool removeIconset(const QString& id);
        QStringList uniqueSmileKeys();
        QString textSmiles(){return QString();};
        QString getSmileName(const QString& iconId);
        QString getSmileNamePretty(const QString& iconId, bool localized=false);
    private:
        QList<IconSet*> m_sets;
    };
}

#endif // STANDARDIMAGESTORAGE_H
