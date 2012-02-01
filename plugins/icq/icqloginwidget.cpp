#include "icqloginwidget.h"

IcqLoginWidget::IcqLoginWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}

IcqLoginWidget::~IcqLoginWidget()
{

}

QString IcqLoginWidget::name() const
{
	return ui.e_login->text();
}

QString IcqLoginWidget::password() const
{
	return ui.e_password->text();
}
