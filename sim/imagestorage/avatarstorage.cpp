/*
 * avatarstorage.cpp
 *
 *  Created on: Jul 2, 2011
 *      Author: todin
 */

#include "avatarstorage.h"
#include "standardavatarstorage.h"

namespace SIM
{

static AvatarStorage* gs_avatarStorage = 0;

void createAvatarStorage(const ProfileManager::Ptr& profileManager)
{
    Q_ASSERT(!gs_avatarStorage);
    gs_avatarStorage = new StandardAvatarStorage(profileManager);
}

void destroyAvatarStorage()
{
    Q_ASSERT(gs_avatarStorage);
    delete gs_avatarStorage;
    gs_avatarStorage = 0;
}

AvatarStorage* getAvatarStorage()
{
    Q_ASSERT(gs_avatarStorage);
    return gs_avatarStorage;
}

void setAvatarStorage(AvatarStorage* storage)
{
    if(gs_avatarStorage)
        delete gs_avatarStorage;
    gs_avatarStorage = storage;
}

} /* namespace SIM */
