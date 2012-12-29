#include "standarduserwndcontroller.h"
#include "userwnd.h"
#include "icontainer.h"
#include "log.h"
#include "clients/client.h"
#include "simgui/messageeditor.h"
#include "clients/clientmanager.h"
#include "contacts/contact.h"
#include "contacts/contactlist.h"

using SIM::log;
using SIM::L_DEBUG;

StandardUserWndController::StandardUserWndController(int contactId) : m_id(contactId)
{
    m_userWnd = createUserWnd(contactId);
}

StandardUserWndController::~StandardUserWndController()
{
    log(L_DEBUG, "StandardUserWndController::~StandardUserWndController(): %d", m_id);
}

int StandardUserWndController::id() const
{
    return m_id;
}

void StandardUserWndController::setUserWnd(IUserWnd* wnd)
{
    if(m_userWnd == wnd)
        return;
    if(m_userWnd)
        delete m_userWnd;
    m_userWnd = wnd;
}

IUserWnd* StandardUserWndController::userWnd() const
{
    return m_userWnd;
}

void StandardUserWndController::setContainer(IContainer* cont)
{
    m_container = cont;
}

IContainer* StandardUserWndController::container() const
{
    return m_container;
}

void StandardUserWndController::raise()
{

}

void StandardUserWndController::addMessageToView(const SIM::MessagePtr& message)
{
    log(L_DEBUG, "StandardUserWndController::addMessageToView");
    m_userWnd->addMessageToView(message);
}

int StandardUserWndController::messagesCount() const
{
    return m_userWnd->messagesInViewArea();
}

void StandardUserWndController::setMessageType(const QString& type)
{
    QString selectedClientId = m_userWnd->selectedClientId();
    SIM::ClientPtr client = SIM::getClientManager()->client(selectedClientId);
    if(!client)
    {
        // We just remove message editor if it is there in that case
        m_userWnd->setMessageEditor(0);
        return;
    }
    SIM::MessageEditor* editor = client->messageEditorFactory()->createMessageEditor(sourceContact(), targetContact(), type, m_userWnd);
    connect(editor, SIGNAL(messageSendRequest(SIM::MessagePtr)), this, SLOT(slot_messageSendRequest(SIM::MessagePtr)));
    m_userWnd->setMessageEditor(editor);
}

void StandardUserWndController::slot_messageSendRequest(const SIM::MessagePtr& message)
{
    emit messageSendRequest(message);
}

IUserWnd* StandardUserWndController::createUserWnd(int id)
{
    return new UserWnd(id, false, false);
}

SIM::IMContactPtr StandardUserWndController::targetContact() const
{
    SIM::ContactPtr contact = SIM::getContactList()->contact(m_id);
    if(!contact)
        return SIM::IMContactPtr();

    QString selectedClientId = m_userWnd->selectedClientId();
    return contact->clientContact(selectedClientId);
}

SIM::IMContactPtr StandardUserWndController::sourceContact() const
{
    QString selectedClientId = m_userWnd->selectedClientId();
    SIM::ClientPtr client = SIM::getClientManager()->client(selectedClientId);
    if(!client)
        return SIM::IMContactPtr();

    return client->ownerContact();
}
