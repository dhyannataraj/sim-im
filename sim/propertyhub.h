
#ifndef SIM_PROPERTYHUB_H
#define SIM_PROPERTYHUB_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QDomElement>
#include <QSharedPointer>

#include "simapi.h"

namespace testPropertyHub
{
    class Test;
}

namespace SIM
{
    class PropertyHub;
    typedef QSharedPointer<PropertyHub> PropertyHubPtr;
    class EXPORT PropertyHub
    {
    public:
        static PropertyHubPtr create();
        static PropertyHubPtr create(const QString& ns);

        PropertyHub(); // FIXME make protected
        PropertyHub(const QString& ns); // FIXME make protected
        virtual ~PropertyHub();

        void setValue(const QString& key, const QVariant& value);
        QVariant value(const QString& key);
        void setStringMapValue(const QString& mapname, int key, const QString& value);
        QString stringMapValue(const QString& mapname, int key);
        void clearStringMap(const QString& mapname);
        QList<QString> allKeys();

        bool addPropertyHub(PropertyHubPtr hub);
        PropertyHubPtr propertyHub(const QString& hubNamespace);
        QStringList propertyHubNames();

        bool serialize( QDomElement element );
        bool deserialize( QDomElement element );

        void clearValues();
        void clearPropertyHubs();

        // This is to parse old
        void parseSection(const QString& string);

        QString getNamespace() { return m_namespace; }
    protected:
        bool serializeVariant( QDomElement element, const QVariant& v );
        bool deserializeVariant( const QDomElement &element, QVariant &v );

        bool serializeVariantMap( QDomElement element, const QVariantMap& map );
        bool deserializeVariantMap( const QDomElement &element, QVariantMap &map );

        bool serializeStringList( QDomElement element, const QStringList& list );
        bool deserializeStringList( const QDomElement &element, QStringList &list );

        bool serializeByteArray( QDomElement element, const QByteArray& array );
        bool deserializeByteArray( const QDomElement &element, QByteArray &array );

    private:
        typedef QMap<QString, PropertyHubPtr> PropertyHubMap;

        QString m_namespace;
        QVariantMap m_data;
        PropertyHubMap m_hubs;

        friend class testPropertyHub::Test;
    };

}

#endif

// vim: set expandtab:

