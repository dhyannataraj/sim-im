#ifndef JABBERLOGINWIDGET_H
#define JABBERLOGINWIDGET_H

#include <QtGui/QWidget>
#include "ui_jabberloginwidget.h"

class JabberLoginWidget : public QWidget
{
    Q_OBJECT

public:
    JabberLoginWidget(QWidget *parent = 0);
    ~JabberLoginWidget();

    QString jid() const;
    QString password() const;
    QString server() const;
    int port() const;

private:
    Ui::JabberLoginWidget ui;
};

#endif // JABBERLOGINWIDGET_H
