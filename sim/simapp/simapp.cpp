
#include "simapp.h"
#include "cfg.h"
#include "log.h"
#include "misc.h"
#include "messaging/message.h"
#include "plugins.h"

using namespace SIM;

SimApp::SimApp(int &argc, char **argv)
      : QApplication(argc, argv),
      m_services(new Services())
{
    registerMetaTypes();
    initializeServices();

    setQuitOnLastWindowClosed(true);
}

SimApp::~SimApp()
{

}

bool SimApp::initializePlugins()
{
    if(!SIM::getPluginManager()->initialize(m_services))
        return false;
    return SIM::getPluginManager()->isLoaded();
}

void SimApp::initializeServices()
{
    m_services->setProtocolManager(ProtocolManager::Ptr(new ProtocolManager()));
}

void SimApp::commitData(QSessionManager&)
{
}

void SimApp::saveState(QSessionManager &sm)
{
    QApplication::saveState(sm);
}

void SimApp::registerMetaTypes()
{
    qRegisterMetaType<SIM::MessagePtr>("SIM::MessagePtr");
}

// vim: set expandtab:

