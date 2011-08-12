/*
 * actionmenucommonstatus.cpp
 *
 *  Created on: Aug 12, 2011
 */

#include <QMenu>
#include "actionmenucommonstatus.h"
#include "imagestorage/imagestorage.h"
#include "commonstatus.h"

ActionMenuCommonStatus::ActionMenuCommonStatus(CommonStatus* status) : QAction(0),
    m_commonStatus(status)
{
    QMenu* menu = new QMenu();

    QList<CommonStatus::CommonStatusDescription> statuses = status->allCommonStatuses();

    foreach(const CommonStatus::CommonStatusDescription& status, statuses)
    {
        QAction* action = new QAction(menu);
        action->setIcon(SIM::getImageStorage()->icon(status.iconId));
        action->setText(status.name);
        menu->addAction(action);
    }
    setMenu(menu);
}

ActionMenuCommonStatus::~ActionMenuCommonStatus()
{
}

