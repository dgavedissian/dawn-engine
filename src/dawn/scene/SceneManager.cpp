/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "SceneManager.h"
#include "Entity.h"

NAMESPACE_BEGIN

SceneManager::SceneManager(PhysicsWorld* physicsMgr, Ogre::SceneManager* ogreSceneMgr)
    : mPhysicsWorld(physicsMgr),
      mSceneMgr(ogreSceneMgr)
{
}

SceneManager::~SceneManager()
{
    clearEntities();
}

void SceneManager::clearEntities()
{
    for (auto i = mEntities.begin(); i != mEntities.end(); i++)
        deleteEntity(*i);
    mEntities.clear();
}

void SceneManager::removeEntity(Entity* entity)
{
#ifdef DW_DEBUG
    assert(mIterating == false);
#endif

    // Free memory
    deleteEntity(entity);

    // Search for the location of this entity and remove it from the List
    auto i = std::find(mEntities.begin(), mEntities.end(), entity);
    if (i != mEntities.end())
    {
        *i = std::move(mEntities.back());
        mEntities.pop_back();
    }
}

void SceneManager::update(float dt)
{
#ifdef DW_DEBUG
    mIterating = true;
#endif

    // Update all entities - any new entities that are added in this loop would be added to the end
    // but not updated.
    size_t size = mEntities.size();
    for (size_t i = 0; i < size; ++i)
    {
        if (!mEntities[i]->update(dt))
        {
            // Free memory
            deleteEntity(mEntities[i]);

            // Swap with the final element that will be updated, then swap the last
            // non-updated element with this one.
            // A is what's being removed; B is the last updated element; C is the
            // back
            // element and | is the boundary between active/inactive this frame.
            // 1) ---A---B|---C
            // 2) ---B---B|---C
            // 3) ---B---C|---C
            // 4) ---B---C|---
            // 5) ---B---|C---
            mEntities[i] = std::move(mEntities[size - 1]);
            mEntities[size - 1] = std::move(mEntities.back());
            mEntities.pop_back();
            i--;
            size--;
        }
    }

#ifdef DW_DEBUG
    mIterating = false;
#endif
}

void SceneManager::preRender(Camera* camera)
{
    // Pre-render all entities
    for (size_t i = 0; i < mEntities.size(); ++i)
        mEntities[i]->preRender(camera);
}

void SceneManager::deleteEntity(Entity* entity)
{
    auto it = mEntityTypePoolMap.find(&typeid(*entity));
    if (it != mEntityTypePoolMap.end())
    {
        it->second->free(entity);
    }
    else
    {
        delete entity;
    }
}

NAMESPACE_END
