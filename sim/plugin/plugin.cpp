
#include "plugin.h"
#include "log.h"
#include "pluginmanager.h"

namespace SIM {

	class PluginPrivate
	{
	public:
		PluginPrivate()
		{
		}

		virtual ~PluginPrivate()
		{
		}

		void setName(const QString& n)
		{
			m_name = n;
		}

		QString name()
		{
			return m_name;
		}

		bool isProtocolPlugin()
		{
			return m_protocol;
		}

		void setProtocolPlugin(bool proto)
		{
			m_protocol = proto;
		}

		void setAlwaysEnabled(bool ae)
		{
			m_alwaysEnabled = ae;
		}

		bool isAlwaysEnabled()
		{
			return m_alwaysEnabled;
		}
		
	private:
		QString m_name;
		bool m_protocol;
		bool m_alwaysEnabled;
	};

	Plugin::Plugin() : p(new PluginPrivate)
	{
		p->setProtocolPlugin(false);
		p->setAlwaysEnabled(false);
	}

	Plugin::~Plugin()
	{
		log(L_DEBUG, "Plugin::~Plugin(%s)", qPrintable(name()));
		delete p;
	}

	void Plugin::setName(const QString& name)
	{
		p->setName(name);
	}

	QString Plugin::name()
	{
		return p->name();
	}

	PluginInfo* Plugin::getInfo()
	{
		return getPluginManager()->getPluginInfo(name());
	}


}
