#include "clientsdialog.h"

#include "clientmanager.h"
#include "contacts/client.h"
#include "imagestorage/imagestorage.h"
#include "contacts/protocol.h"
#include "newprotocol.h"
#include "log.h"

ClientsDialog::ClientsDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	fillClientsList();
}

ClientsDialog::~ClientsDialog()
{

}

void ClientsDialog::addClient()
{
    NewProtocol dlg("", this);
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
    SIM::ClientPtr client = SIM::getClientManager()->client(index);
    SIM::getClientManager()->deleteClient(client->name());
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
    auto clients = SIM::getClientManager()->allClients();
    foreach(const auto& client, clients)
    {
        QIcon icon = SIM::getImageStorage()->icon(client->protocol()->iconId());
        QString name = client->ownerContact()->name();

        QListWidgetItem* item = new QListWidgetItem(icon, name, ui.lw_clients); //item not used! Todo?
    }
}
