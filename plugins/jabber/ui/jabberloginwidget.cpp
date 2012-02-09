#include "jabberloginwidget.h"

JabberLoginWidget::JabberLoginWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}

JabberLoginWidget::~JabberLoginWidget()
{

}

QString JabberLoginWidget::jid() const
{
    return ui.e_jid->text();
}

QString JabberLoginWidget::password() const
{
    return ui.e_password->text();
}

QString JabberLoginWidget::server() const
{
    return ui.e_server->text();
}

int JabberLoginWidget::port() const
{
    return ui.sb_port->value();
}
