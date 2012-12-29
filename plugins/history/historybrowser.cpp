#include "historybrowser.h"

#include "contacts/contact.h"
#include "contacts/contactlist.h"
#include "clients/clientmanager.h"
#include "log.h"
#include "xsl.h"

using namespace SIM;

HistoryBrowser::HistoryBrowser(const HistoryStoragePtr& storage, int contactId, QWidget *parent)
    : QDialog(parent),
      m_storage(storage),
      m_contactId(contactId)
{
	ui.setupUi(this);
	XSL* xsl = new XSL("SIM.7");
	ui.msgView->setXSL(xsl);
	fillMsgView();
}

HistoryBrowser::~HistoryBrowser()
{

}

void HistoryBrowser::fillMsgView()
{
	SIM::ContactPtr contact = SIM::getContactList()->contact(m_contactId);
	SIM::IMContactPtr imcontact = contact->clientContact(0);
	QString contactId = imcontact->id().toString();

	SIM::Client* client = imcontact->client();
	QString ownerId = client->ownerContact()->id().toString();

	auto incoming = m_storage->getMessages(contactId, ownerId, QDateTime(), QDateTime::currentDateTime());
	auto outgoing = m_storage->getMessages(ownerId, contactId, QDateTime(), QDateTime::currentDateTime());

	auto all = incoming + outgoing;

	qSort(all.begin(), all.end(), [](const SIM::MessagePtr& msg1, const SIM::MessagePtr& msg2) { return msg1->timestamp() < msg2->timestamp(); });

	log(L_DEBUG, "Total messages: %d", all.count());

	foreach(const SIM::MessagePtr& message, all)
	{
		ui.msgView->addMessage(message);
	}
}
