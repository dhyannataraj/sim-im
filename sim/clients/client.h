
#ifndef SIM_CLIENT_H
#define SIM_CLIENT_H

#include <QSharedPointer>
#include "cfg.h"
#include "contacts/imstatus.h"
#include "contacts/imgroup.h"
#include "log.h"
#include "simapi.h"
#include "simgui/messageeditorfactory.h"

namespace SIM
{
	class Protocol;
	class EXPORT Client
	{
	public:
		Client(Protocol* protocol);
		virtual ~Client();

		virtual QString name() = 0;
		Protocol *protocol() const { return m_protocol; }

		virtual IMContactPtr createIMContact() = 0;
		virtual void addIMContact(const IMContactPtr& contact) = 0;
		virtual IMContactPtr getIMContact(const IMContactId& id) = 0;
		virtual IMGroupPtr createIMGroup() = 0;

		virtual QWidget* createSetupWidget(const QString& id, QWidget* parent) = 0;
		virtual void destroySetupWidget() = 0;
		virtual QStringList availableSetupWidgets() const = 0;

		virtual QWidget* createStatusWidget() = 0;

		virtual IMStatusPtr currentStatus() = 0;
		virtual void changeStatus(const IMStatusPtr& status) = 0;
		virtual IMStatusPtr savedStatus() = 0;

		virtual IMContactPtr ownerContact() = 0;

		virtual bool deserialize(Buffer* buf) = 0;
		virtual bool loadState(PropertyHubPtr state) = 0;
		virtual PropertyHubPtr saveState() = 0;

		virtual MessageEditorFactory* messageEditorFactory() const = 0;

		virtual QWidget* createSearchWidow(QWidget *parent) = 0;
		virtual QList<IMGroupPtr> groups() = 0;
		virtual QList<IMContactPtr> contacts() = 0;

		QString password() const;
		void setPassword(const QString& password);
		void setCryptedPassword(const QString& password) { setPassword(uncryptPassword(password)); }

		virtual QString retrievePasswordLink() = 0;
		
		PropertyHubPtr properties() { return m_data; }

	protected:
		static QString cryptPassword(const QString& passwd);
		static QString uncryptPassword(const QString& passwd);

	private:
		PropertyHubPtr m_data;
		Protocol* m_protocol;
		QString m_password;
	};

	typedef QSharedPointer<Client> ClientPtr;
	typedef QWeakPointer<Client> ClientWeakPtr;

}

#endif

// vim: set expandtab:

