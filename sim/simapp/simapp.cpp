
#include "simapp.h"
#include "cfg.h"
#include "log.h"
#include "misc.h"
#include "messaging/message.h"
#include "plugin/pluginmanager.h"
#include "clients/standardclientmanager.h"
#include "profile/standardprofilemanager.h"
#include "paths.h"

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
    auto protocolManager = ProtocolManager::Ptr(new ProtocolManager());
    m_services->setProtocolManager(protocolManager);
    auto profileManager = ProfileManager::Ptr(new StandardProfileManager(SIM::PathManager::configRoot()));
    m_services->setClientManager(ClientManager::Ptr(new StandardClientManager(profileManager, protocolManager)));
    m_services->setProfileManager(profileManager);
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

