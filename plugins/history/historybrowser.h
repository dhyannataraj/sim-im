#ifndef HISTORYBROWSER_H
#define HISTORYBROWSER_H

#include <QtGui/QDialog>
#include "ui_historybrowser.h"
#include "historystorage.h"

class HistoryBrowser : public QDialog
{
    Q_OBJECT

public:
    HistoryBrowser(const HistoryStoragePtr& storage, int contactId, QWidget *parent = 0);
    ~HistoryBrowser();

private:
    void fillMsgView();

    Ui::HistoryBrowserClass ui;
    HistoryStoragePtr m_storage;
    int m_contactId;
};

#endif // HISTORYBROWSER_H
