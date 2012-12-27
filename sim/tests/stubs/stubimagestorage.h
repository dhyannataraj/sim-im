#ifndef STUBIMAGESTORAGE_H
#define STUBIMAGESTORAGE_H

#include "imagestorage/imagestorage.h"


namespace StubObjects
{
    class StubImageStorage : public SIM::ImageStorage
    {
        virtual QIcon icon(const QString& id) { return QIcon(); }
        virtual QImage image(const QString& id) { return QImage(); }
        virtual QPixmap pixmap(const QString& id) { return QPixmap(); }

        virtual bool addIconSet(SIM::IconSet* set) { return true; }
        virtual bool removeIconset(const QString& id) { return true; }

        virtual QString parseSmiles(const QString& input) { return QString(); }
		virtual QList<SIM::IconSet*> getIconSets() { return QList<SIM::IconSet*>(); }
		virtual QString textSmiles() { return QString(); }
		virtual QString parseAllSmiles(const QString& input) { return QString(); }
		virtual QString parseAllSmilesByName(const QString& name) { return QString(); }
		virtual QStringList uniqueSmileKeys() { return QStringList(); }
		virtual QString getSmileName(const QString& iconId) { return QString(); }
		virtual QString getSmileNamePretty(const QString& iconId, bool localized = false) { return QString(); }
    };
}

#endif // STUBIMAGESTORAGE_H
