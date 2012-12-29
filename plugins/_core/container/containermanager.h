#ifndef CONTAINERMANAGER_H
#define CONTAINERMANAGER_H

#include <QObject>
#include <QList>
#include "container.h"
#include "icontainermanager.h"
#include "sendmessageprocessor.h"
#include "receivemessageprocessor.h"
#include "icontainercontroller.h"
#include "core_api.h"
#include "services.h"

class CorePlugin;
class CORE_EXPORT ContainerManager : public QObject, public IContainerManager
{
    Q_OBJECT
public:
    explicit ContainerManager(const SIM::Services::Ptr& services, CorePlugin* parent);
    virtual ~ContainerManager();

    virtual bool init();
    virtual void contactChatRequested(int contactId, const QString& messageType);

    virtual void messageSent(const SIM::MessagePtr& msg);
    virtual void messageReceived(const SIM::MessagePtr& msg);

    virtual ContainerMode containerMode() const;
    virtual void setContainerMode(ContainerMode mode);

signals:

public slots:
    void containerClosed(int id);

protected:
    virtual ContainerControllerPtr makeContainerController();

private:

    void addContainer(const ContainerControllerPtr& cont);
    int containerCount();
    ContainerControllerPtr containerController(int index);
    ContainerControllerPtr containerControllerById(int id);
    void removeContainer(int index);
    void removeContainerById(int id);

    UserWndControllerPtr findUserWnd(int id);
    ContainerControllerPtr containerControllerForUserWnd(int userWndId);

    SendMessageProcessor* m_sendProcessor;
    ReceiveMessageProcessor* m_receiveProcessor;

    QList<ContainerControllerPtr> m_containers;
    int m_containerControllerId;
    ContainerMode m_containerMode;
    SIM::Services::Ptr m_services;
    CorePlugin* m_core;
};

#endif // CONTAINERMANAGER_H
