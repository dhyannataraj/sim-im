#ifndef CLIENTSDIALOG_H
#define CLIENTSDIALOG_H

#include <QtGui/QDialog>
#include "ui_clientsdialog.h"

class ClientsDialog : public QDialog
{
    Q_OBJECT
public:
    ClientsDialog(QWidget *parent = 0);
    ~ClientsDialog();

public slots:
    void addClient();
    void deleteClient();
    void moveUp();
    void moveDown();

private:
    void fillClientsList();
    Ui::ClientsDialog ui;
};

#endif // CLIENTSDIALOG_H
