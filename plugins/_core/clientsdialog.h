#ifndef CLIENTSDIALOG_H
#define CLIENTSDIALOG_H

#include <QtGui/QDialog>
#include "ui_clientsdialog.h"
#include "services.h"

class ClientsDialog : public QDialog
{
    Q_OBJECT
public:
    ClientsDialog(const SIM::ProtocolManager::Ptr& protocolManager, QWidget *parent = 0);
    ~ClientsDialog();

public slots:
    void addClient();
    void deleteClient();
    void moveUp();
    void moveDown();

private:
    void fillClientsList();
    Ui::ClientsDialog ui;
    SIM::ProtocolManager::Ptr m_protocolManager;
};

#endif // CLIENTSDIALOG_H
