/***************************************************************************
                          mainwin.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "simapi.h"

#include "mainwin.h"
#include "core.h"
#include "roster/userview.h"
#include "contacts/contact.h"
#include "contacts/contactlist.h"
#include "events/eventhub.h"
#include "commands/commandhub.h"
#include "commands/uicommandlist.h"
#include "imagestorage/imagestorage.h"
#include "clients/clientmanager.h"
#include "container/userwnd.h"
#include "events/actioncollectionevent.h"
#include "clientsdialog.h"

#include "userconfig/userconfigcontext.h"
#include "userconfig/userconfigcontroller.h"

#include <QApplication>
#include <QPixmap>
#include <QLayout>
#include <QTimer>
#include <QSizeGrip>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include "profile/profilemanager.h"

#include "log.h"

using namespace SIM;

static const char* MessageTypeIdProperty = "message_type_id";
static const char* ContactIdProperty = "contact_id";

MainWindow::MainWindow(CorePlugin* core)
    : QMainWindow(NULL, Qt::Window)
    , m_core(core)
    , m_noresize(false)
    , m_systray(new QSystemTrayIcon(this))
    , m_trayIconMenu(new QMenu(this))
{
    log(L_DEBUG, "MainWindow::MainWindow()");
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setWindowIcon(getImageStorage()->icon("SIM"));
    
    QStringList actions = getCommandHub()->actionsForTag("trayicon");
    
    createTrayIcon(actions);

    m_systray->show();
    updateTitle();

    m_bar = new SIM::SimToolbar(this);
    addToolBar(m_bar);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_layout = new QVBoxLayout(m_centralWidget);
    m_layout->setMargin(0);

    statusBar()->show();
    statusBar()->setSizeGripEnabled(false);
    statusBar()->installEventFilter(this);

    m_view = new UserView(core);
    m_view->init();
    connect(m_view, SIGNAL(contactChatRequested(int)), this, SLOT(contactChatRequested(int)));
    connect(m_view, SIGNAL(contactMenuRequested(QPoint,int)), this, SLOT(contactMenuRequested(QPoint,int)));
    addWidget(m_view);

    QAction* show_only_online = getCommandHub()->action("show_only_online");
    setShowOnlyOnlineContacts(show_only_online->isChecked());
    connect(show_only_online, SIGNAL(triggered(bool)), this, SLOT(setShowOnlyOnlineContacts(bool)));
    connect(m_systray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
    delete m_view;
    delete m_bar;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if (m_noresize)
        return;
    QMainWindow::resizeEvent(e);
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
//    if (e->type() == QEvent::ChildRemoved){
//        QChildEvent *ce = static_cast<QChildEvent*>(e);
//        std::list<QWidget*>::iterator it;
//        for (it = statusWidgets.begin(); it != statusWidgets.end(); ++it){
//            if (*it == ce->child()){
//                statusWidgets.erase(it);
//                break;
//            }
//        }
//        if(statusWidgets.size() == 0)
//        {
//            statusBar()->hide();
//        }
//    }
    return QMainWindow::eventFilter(o, e);
}

void MainWindow::loadDefaultMainToolbar()
{
    SIM::UiCommandList list;
    list.appendCommand("show_only_online");
    list.appendCommand("separator");
    list.appendCommand("common_status");
    list.appendCommand("separator");
    //list.appendCommand("main_menu");

    m_bar->load(list);
    QIcon arrowDown = SIM::getImageStorage()->icon("1downarrow");
    QAction* mainMenu = new QAction(arrowDown, QString(), m_bar);
    connect(mainMenu, SIGNAL(triggered()), this, SLOT(mainMenuRequested()));
    m_bar->addAction(mainMenu);
}

void MainWindow::populateMainToolbar()
{
    loadDefaultMainToolbar();
}

void MainWindow::init()
{
    log(L_DEBUG, "MainWindow::init()");
    updateTitle();
    populateMainToolbar();
    refreshStatusWidgets();
    loadSettings();
}

void MainWindow::setShowOnlyOnlineContacts(bool show)
{
    m_view->setShowOffline(!show);
}

//void MainWindow::quit()
//{
//    close();
//}

//void MainWindow::closeEvent(QCloseEvent *e)
//{
//	CorePlugin::instance()->prepareConfig();
//    save_state();
//    m_core->propertyHub()->setValue("mainwindow_toolbar_actions", m_bar->saveCommandList());
//    QMainWindow::closeEvent(e);
//    qApp->quit();
//}

void MainWindow::addWidget(QWidget *w)
{
    w->setParent(m_centralWidget);
    w->move(QPoint());
    m_layout->addWidget(w);
    if(isVisible())
        w->show();
}

void MainWindow::refreshStatusWidgets()
{
    qDeleteAll(m_statusWidgets);
    m_statusWidgets.clear();
    QList<ClientPtr> clients = getClientManager()->allClients();
    foreach(const ClientPtr& client, clients)
    {
        QWidget* statusWidget = client->createStatusWidget();
        if(statusWidget)
        {
            m_statusWidgets.append(statusWidget);
            statusWidget->setParent(statusBar());
            statusBar()->addWidget(statusWidget);
            statusWidget->show();
        }
    }
}

void MainWindow::updateTitle()
{
    QString title;
    ContactPtr owner = getContactList()->ownerContact();
    if (owner)
        title = owner->name();
    if (title.isEmpty())
        title = "SIM";
    setWindowTitle(title);
}

//void MainWindow::focusInEvent(QFocusEvent *e)
//{
//    QMainWindow::focusInEvent(e);
//    m_view->setFocus();
//}

UserView* MainWindow::userview() const
{
    return m_view;
}

void MainWindow::contactChatRequested(int contactId)
{
    IContainerManager* manager = m_core->containerManager();
    manager->contactChatRequested(contactId, "generic");
}

void MainWindow::contactMenuRequested(const QPoint& pos, int contactId)
{
    raiseContactMenu(m_view->mapToGlobal(pos), contactId);
}

// TODO refactor
void MainWindow::raiseContactMenu(const QPoint& pos, int contactId)
{
    SIM::ContactPtr contact = getContactList()->contact(contactId);
    if(!contact)
        return;

    // TODO several contacts in metacontact
    QMenu menu(this);
    for(int i = 0; i < contact->clientContactCount(); i++)
    {
        SIM::IMContactPtr imcontact = contact->clientContact(i);
        QList<MessageTypeDescriptor> messageTypes = imcontact->allMessageTypes();

        foreach(const MessageTypeDescriptor& desc, messageTypes)
        {
            QAction* action = new QAction(&menu);
            connect(action, SIGNAL(triggered()), this, SLOT(sendMessageRequested()));
            action->setProperty(MessageTypeIdProperty, desc.id);
            action->setProperty(ContactIdProperty, contactId);
            action->setIcon(getImageStorage()->icon(desc.iconId));
            action->setText(desc.text);
            menu.addAction(action);
        }
    }

    QAction* action = new QAction(&menu);
    action->setSeparator(true);
    menu.addAction(action);

    QAction* contactInfoAction = new QAction(&menu);
    contactInfoAction->setIcon(getImageStorage()->icon("info"));
    contactInfoAction->setText(I18N_NOOP("User &info"));
    contactInfoAction->setProperty(ContactIdProperty, contactId);
    connect(contactInfoAction, SIGNAL(triggered()), this, SLOT(contactInfo()));
    menu.addAction(contactInfoAction);

    auto data = SIM::ActionCollectionEventData::create("contact_menu", QString::number(contact->id()));
    getEventHub()->triggerEvent("contact_menu", data);

    if(data->actions()->actions.length() > 0)
    {
    	foreach(QAction* action, data->actions()->actions)
		{
    		menu.addAction(action);
		}
    }

    menu.exec(pos);
}

void MainWindow::sendMessageRequested()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if(!action)
        return;

    QString messageTypeId = action->property(MessageTypeIdProperty).toString();
    if(messageTypeId.isNull())
        return;

    int contactId = action->property(ContactIdProperty).toInt();
    if(!contactId)
        return;

    IContainerManager* manager = m_core->containerManager();
    manager->contactChatRequested(contactId, "generic");
}

void MainWindow::mainMenuRequested()
{
    QMenu menu;

    SIM::ActionCollectionEventDataPtr data = SIM::ActionCollectionEventData::create("main_menu", QString());
    getEventHub()->triggerEvent("main_menu", data);

    if(data->actions()->actions.length() > 0)
    {
        foreach(QAction* action, data->actions()->actions)
        {
            menu.addAction(action);
        }
    }

    addClientsDialogAction(&menu);

    QAction* separator = new QAction(&menu);
    separator->setSeparator(true);
    menu.addAction(separator);

    QAction* quit = new QAction(SIM::getImageStorage()->icon("exit"), tr("Quit"), &menu);
    connect(quit, SIGNAL(triggered()), this, SLOT(quitApp()));
    menu.addAction(quit);

    menu.exec(QCursor::pos());
}

void MainWindow::showClientsDialog()
{
    ClientsDialog dlg(this);
    dlg.exec();
    refreshStatusWidgets();
}

void MainWindow::contactInfo()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if(!action)
        return;

    int contactId = action->property(ContactIdProperty).toInt();
    if(!contactId)
        return;

    SIM::ContactPtr contact = SIM::getContactList()->contact(contactId);
    if(!contact)
        return;

    UserConfigContextPtr context = UserConfigContext::create(contact);
    UserConfigController controller;
    controller.init(context);

    controller.exec();
}

void MainWindow::createTrayIcon(QStringList actions) //Todo make configurable
{

    foreach(QString action, actions)
                m_trayIconMenu->addAction(getCommandHub()->action(action));

    m_trayIconMenu->addSeparator();
    QAction * actionQuit = new QAction(getImageStorage()->icon("SIM"), tr("&Quit"), this);
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quitApp()));
    m_trayIconMenu->addAction(actionQuit);

    m_systray->setIcon(getImageStorage()->icon("SIM"));
    m_systray->setContextMenu(m_trayIconMenu);
}

void MainWindow::addClientsDialogAction(QMenu* menu)
{
    QAction* action = new QAction(tr("Clients..."), menu);
    connect(action, SIGNAL(triggered()), this, SLOT(showClientsDialog()));
    menu->addAction(action);
}

void MainWindow::quitApp()
{
    this->close();
    QTimer::singleShot(0, m_core, SLOT(cmdQuit())); //And quit core :)
}

void MainWindow::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    QMessageBox msgBox;
    switch (reason) {

    case QSystemTrayIcon::Trigger: // single click
        if (isVisible () )
            hide();
        else
            show();
        break;
    case QSystemTrayIcon::DoubleClick:
        msgBox.setText("The systray has been double clicked");
        msgBox.exec();
        break;
    case QSystemTrayIcon::MiddleClick:
        msgBox.setText("The systray has been middle clicked");
        msgBox.exec();
        break;
    default:
        ;
    }
}

void MainWindow::loadSettings()
{
    if (!getProfileManager())
        return;
    PropertyHubPtr rootHub = getProfileManager()->currentProfile()->config()->rootHub();

    if (rootHub->propertyHub("windows").isNull())
        return;
    PropertyHubPtr hub = rootHub->propertyHub("windows")->propertyHub("MainWindow");

    if (hub.isNull())
        return;
    restoreGeometry(hub->value("windowGeometry").toByteArray());
    restoreState(hub->value("windowState").toByteArray());
}

void MainWindow::saveSettings()
{
    if (!getProfileManager())
        return;
    PropertyHubPtr rootHub = getProfileManager()->currentProfile()->config()->rootHub();
    if (rootHub->propertyHub("windows").isNull())
        rootHub->addPropertyHub(PropertyHub::create("windows"));

    PropertyHubPtr hub = PropertyHub::create("MainWindow");
    rootHub->propertyHub("windows")->addPropertyHub(hub);

    hub->setValue("windowGeometry",saveGeometry());
    hub->setValue("windowState",saveState());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
