
#ifndef SIM_PLUGIN_H
#define SIM_PLUGIN_H

#include "misc.h"
#include <QWidget>
#include <QByteArray>
#include <QString>
#include <QSharedPointer>
#include "services.h"

namespace SIM {

    struct PluginInfo;

    class EXPORT Plugin
    {
    public:
        Plugin();
        virtual ~Plugin();
        virtual QWidget *createConfigWindow(QWidget* /* *parent */ ) { return NULL; }
        virtual QByteArray getConfig() { return QByteArray(); }

        void setName(const QString& n);
        QString name();

        PluginInfo* getInfo();


    private:
        class PluginPrivate* p;
    };

    typedef QSharedPointer<Plugin> PluginPtr;

}

#endif

