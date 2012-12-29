
#ifndef SIMAPP_H
#define SIMAPP_H

#include "cfg.h"

#include <QApplication>
#include <QSessionManager>
#include "simapi.h"
#include "services.h"

class EXPORT SimApp : public QApplication
{
    Q_OBJECT
public:
    SimApp(int &argc, char **argv);
    ~SimApp();

    bool initializePlugins();

    SIM::Services::Ptr services() const { return m_services; }
protected:
    void registerMetaTypes();
    void commitData(QSessionManager&);
    void saveState(QSessionManager&);

private:
    void initializeServices();

    SIM::Services::Ptr m_services;
};

#endif

// vim: set expandtab:

