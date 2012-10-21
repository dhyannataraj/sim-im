#ifndef ICONSET_H
#define ICONSET_H

#include <QString>
#include <QIcon>
#include <QPixmap>

namespace SIM {

class IconSet
{
public:
    virtual ~IconSet() {}

    virtual QString id() const = 0;
    virtual bool hasIcon(const QString& iconId) = 0;
    virtual QIcon icon(const QString& iconId) = 0;
    virtual QImage image(const QString& iconId) = 0;
    virtual QPixmap pixmap(const QString& iconId) = 0;
    //virtual QList<IconSet *> getIconSets() = 0;
    virtual QStringList textSmiles()=0;
    virtual QString parseSmiles(const QString& input) = 0;
    virtual QString parseAllSmiles(const QString& input)=0;
};

} // namespace SIM

#endif // ICONSET_H
