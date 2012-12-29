#ifndef CONTAINERCONTROLLER_H
#define CONTAINERCONTROLLER_H

#include <QObject>
#include "messaging/message.h"
#include "userwndcontroller.h"
#include "core_api.h"
#include "icontainer.h"
#include "icontainercontroller.h"
#include "services.h"

class CORE_EXPORT ContainerController : public QObject, public IContainerController
{
    Q_OBJECT
public:
    explicit ContainerController(const SIM::Services::Ptr& services, int id);
    virtual ~ContainerController();

    virtual int id() const;

    virtual void sendMessage(const SIM::MessagePtr& msg);
    virtual void addUserWnd(int contactId);
    virtual IUserWnd* userWndById(int id);
    virtual UserWndControllerPtr userWndController(int id);
    virtual void raiseUserWnd(int id);

signals:
    void closed(int id);

public slots:
    void containerClosed();
    void messageSendRequest(const SIM::MessagePtr& message);

protected:
    virtual UserWndControllerPtr createUserWndController(int id);

private:
    QList<UserWndControllerPtr> m_controllers;
    IContainerPtr m_view;
    int m_id;
    SIM::Services::Ptr m_services;
};

#endif // CONTAINERCONTROLLER_H
