#ifndef JISPICONSET_H
#define JISPICONSET_H

#include "iconset.h"
#include "qzip/qzip.h"
#include <QMap>

namespace SIM {

class JispIconSet : public IconSet
{
public:
    JispIconSet();

    bool load(const QString& filename);

    virtual QString id() const;
    virtual QString name() const;
    virtual bool hasIcon(const QString& iconId);
    virtual bool hasSmile(const QString& txtSmile);
    virtual QIcon icon(const QString& iconId);
    virtual QImage image(const QString& iconId);
    virtual QPixmap pixmap(const QString& iconId);
    virtual QString parseSmiles(const QString& input);
    virtual QString parseAllSmiles(const QString& input){return input;};
    virtual QStringList textSmiles();
    virtual bool isTextIconAdded(const QString& iconId);
    virtual QString getSmileName(const QString& iconId);
    virtual QString getSmileNamePretty(const QString& iconId, bool localized=false);
private:
    bool parse(const QByteArray& arr);

    QMap<QString, QString> m_images;
    QMap<QString, QString> m_smiles;
    QStringList m_smileKeys;
    QString m_id;
    QString m_name;
    UnZip m_uz;
};

} // namespace SIM

#endif // JISPICONSET_H
