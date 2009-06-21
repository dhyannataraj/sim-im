
#ifndef SIM_PROPERTYHUB_H
#define SIM_PROPERTYHUB_H

#include <QObject>
#include <QString>

#include "simapi.h"

namespace SIM
{
	class EXPORT PropertyHub : virtual public QObject
	{
		Q_OBJECT
	public:
		PropertyHub(const QString& ns);
		virtual ~PropertyHub();

		bool save();
		bool load();
        // This is to parse old
        void parseSection(const QString& string);
	private:
		QString m_namespace;
	};
}

#endif

