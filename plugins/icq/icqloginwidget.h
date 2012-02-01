#ifndef ICQLOGINWIDGET_H
#define ICQLOGINWIDGET_H

#include <QWidget>
#include "ui_icqloginwidget.h"

class IcqLoginWidget : public QWidget
{
    Q_OBJECT

public:
    IcqLoginWidget(QWidget *parent = 0);
    virtual ~IcqLoginWidget();

    QString name() const;
    QString password() const;

private:
    Ui::IcqLoginWidget ui;
};

#endif // ICQLOGINWIDGET_H
