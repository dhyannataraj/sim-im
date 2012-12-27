/*
 * standardavatarstorage.h
 *
 *  Created on: Jul 2, 2011
 *      Author: todin
 */

#ifndef STANDARDAVATARSTORAGE_H_
#define STANDARDAVATARSTORAGE_H_

#include <QList>
#include <QDir>
#include <QImage>
#include <QMap>

#include "avatarstorage.h"
#include "misc.h"

namespace SIM {

class SIM_EXPORT StandardAvatarStorage: public SIM::AvatarStorage
{
public:
    StandardAvatarStorage();
    virtual ~StandardAvatarStorage();

    virtual void addAvatar(const IMContactId& contactId, const QImage& image, const QString& type = QString());
    virtual QImage getAvatar(const IMContactId& contactId, const QString& type = QString());

    virtual QString id() const;
    virtual bool hasIcon(const QString& iconId);
    virtual bool hasSmile(const QString& txtSmile){return false;};
    virtual QIcon icon(const QString& iconId);
    virtual QImage image(const QString& iconId);
    virtual QPixmap pixmap(const QString& iconId);
    virtual QString parseSmiles(const QString& input) {return input;};
    virtual QStringList textSmiles(){return QStringList();};
    virtual QString parseAllSmiles(const QString& input){return input;};
    virtual void parseAllSmilesByName(const QString& name, QIcon &icon){};
    virtual QString getSmileName(const QString& iconId) {return QString();}
    virtual QString getSmileNamePretty(const QString& iconId, bool localized=false){return QString();};
protected:
    virtual bool saveImage(const QString& path, const QImage& image);
    virtual QImage loadImage(const QString& path);

private:
    class StandardAvatarStoragePimpl* d;

    QImage getFile(const QString& id);

    QString makeFilename(const IMContactId& id, const QString& type = QString());
    QString makeUri(const IMContactId& id, const QString& type = QString());
    QString basePath() const;
};

}; /* End namespace SIM */
#endif /* STANDARDAVATARSTORAGE_H_ */
