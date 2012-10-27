#ifndef IMAGESTORAGE_H
#define IMAGESTORAGE_H

#include <QObject>
#include <QIcon>
#include <QImage>
#include <QPixmap>

#include "simapi.h"
#include "iconset.h"

namespace SIM {

class EXPORT ImageStorage
{
public:
    virtual ~ImageStorage() {}

    virtual QIcon icon(const QString& id) = 0;
    virtual QImage image(const QString& id) = 0;
    virtual QPixmap pixmap(const QString& id) = 0;

    virtual bool addIconSet(IconSet* set) = 0;
    virtual bool removeIconset(const QString& id) = 0;
    virtual QList<IconSet *> getIconSets()=0;
    virtual QString textSmiles()=0;
    virtual QString parseAllSmiles(const QString& input)=0;
    virtual QString parseAllSmilesByName(const QString& name)=0;
    virtual QStringList uniqueSmileKeys()=0;
    virtual QString getSmileName(const QString& iconId)=0;
    virtual QString getSmileNamePretty(const QString& iconId, bool localized=false)=0;
};

EXPORT ImageStorage* getImageStorage();
void EXPORT setImageStorage(ImageStorage* storage);
void EXPORT createImageStorage();
void EXPORT destroyImageStorage();

} // namespace SIM

#endif // IMAGESTORAGE_H
