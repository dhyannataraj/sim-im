#include "jabberstatuswidget.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>

#include "imagestorage/imagestorage.h"

JabberStatusWidget::JabberStatusWidget(JabberClient* client, QWidget *parent) :
        SIM::StatusWidget(parent), m_client(client)
{
    setAttribute(Qt::WA_AlwaysShowToolTips, true);
    updateTooltip();
}

SIM::Client* JabberStatusWidget::client()
{
    return m_client;
}

QMenu* JabberStatusWidget::menu() const
{
    SIM::IMStatusPtr status = m_client->currentStatus();
    if(!status)
        return 0;
    QAction* a;
    QMenu* m = new QMenu();

    a = m->addAction(SIM::getImageStorage()->icon("Jabber_online"), I18N_NOOP("Online"), this, SLOT(online()));
    a->setCheckable(true);
    a->setChecked(status->id() == "online");

    a = m->addAction(SIM::getImageStorage()->icon("Jabber_offline"), I18N_NOOP("Offline"), this, SLOT(offline()));
    a->setCheckable(true);
    a->setChecked(status->id() == "offline");

    return m;
}

void JabberStatusWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if(isBlinking())
        drawBlink();
    else
        drawStatus();
}

void JabberStatusWidget::resizeEvent(QResizeEvent* event)
{
    if(event->size().width() != event->size().height())
        resize(QSize(event->size().height(), event->size().height()));
    SIM::StatusWidget::resizeEvent(event);
}


void JabberStatusWidget::drawBlink()
{
    QPixmap p = SIM::getImageStorage()->pixmap(blinkState() ? "Jabber_online" : "Jabber_offline");
    QPainter painter(this);
    painter.drawPixmap(rect(), p);
}

void JabberStatusWidget::drawStatus()
{
    SIM::IMStatusPtr status = m_client->currentStatus();
    if(!status)
        return;
    QPainter painter(this);
    painter.drawPixmap(rect(), status->icon());
}

QSize JabberStatusWidget::sizeHint() const
{
    SIM::IMStatusPtr status = m_client->currentStatus();
    if(!status)
        return QSize();
    return status->icon().size();
}

void JabberStatusWidget::online()
{
    client()->changeStatus(m_client->getDefaultStatus("online"));
    updateTooltip();
}

void JabberStatusWidget::offline()
{
    client()->changeStatus(m_client->getDefaultStatus("offline"));
    updateTooltip();
}

void JabberStatusWidget::updateTooltip()
{
    QString tooltip;
    tooltip.append(QString("Jabber ") + m_client->ownerContact()->name() + "<br>");
    tooltip.append(client()->currentStatus()->name());
    setToolTip(tooltip);
}
