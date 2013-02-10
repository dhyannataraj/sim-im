#include "clientsdialog.h"

#include "clients/clientmanager.h"
#include "clients/client.h"
#include "imagestorage/imagestorage.h"
#include "contacts/protocol.h"
#include "newprotocol.h"
#include "log.h"

ClientsDialog::ClientsDialog(const SIM::Services::Ptr& services, QWidget *parent)
    : QDialog(parent),
    m_services(services)
{
	ui.setupUi(this);
	fillClientsList();
}

ClientsDialog::~ClientsDialog()
{

}

void ClientsDialog::addClient()
{
    NewProtocol dlg(m_services, "", this);
    dlg.exec();
    fillClientsList();
}

void ClientsDialog::deleteClient()
{
    SIM::log(SIM::L_DEBUG, "deleteClient");
    auto selected = ui.lw_clients->selectedItems();
    if(selected.size() != 1)
        return;
    int index = ui.lw_clients->row(selected.at(0));
    SIM::ClientPtr client = m_services->clientManager()->client(index);
    m_services->clientManager()->deleteClient(client->name());
    fillClientsList();
}

void ClientsDialog::moveUp()
{

}

void ClientsDialog::moveDown()
{

}

void ClientsDialog::fillClientsList()
{
    ui.lw_clients->clear();
    auto clients = m_services->clientManager()->allClients();
    foreach(const auto& client, clients)
    {
        QIcon icon = SIM::getImageStorage()->icon(client->protocol()->iconId());
        QString name = client->ownerContact()->name();

        QListWidgetItem* item = new QListWidgetItem(icon, name, ui.lw_clients); //item not used! Todo?
    }
}
