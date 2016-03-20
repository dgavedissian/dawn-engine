/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

NAMESPACE_BEGIN

class Entity;
class Camera;
class PhysicsWorld;

/// Class used to keep track of entities being updated
class DW_API SceneManager
{
public:
    SceneManager(PhysicsWorld* physicsMgr, Ogre::SceneManager* ogreSceneMgr);
    ~SceneManager();

    template <class T, class... Args>
    T* create(Args... args)
    {
        T* entity;
        auto i = mEntityTypePoolMap.find(&typeid(T));
        if (i != mEntityTypePoolMap.end())
        {
            entity = (*i).second->alloc<T>(this, args...);
        }
        else
        {
            entity = new T(this, args...);
        }
        mEntities.push_back(entity);
        return entity;
    }

    template <class T>
    void reserve(uint slots)
    {
		mEntityTypePoolMap.emplace(&typeid(T), makeShared<FixedMemoryPool<T>>(slots));
    }

    void clearEntities();

    /// Remove an entity by value. This is quite slow with O(n) complexity.
    /// @param entity The entity to find and remove from the entity List
    DEPRECATED void removeEntity(Entity* entity);

    /// Calls update on each entity
    /// @param dt Time elapsed
    void update(float dt);

    /// Calls PreRender on each entity
    void preRender(Camera* camera);

    /// Gets the physics manager associated with this scene manager
    PhysicsWorld* getPhysicsWorld() { return mPhysicsWorld; }

private:
    PhysicsWorld* mPhysicsWorld;
    Ogre::SceneManager* mSceneMgr;

    // Owned entities
    Vector<Entity*> mEntities;

    // Memory pools
    HashMap<const std::type_info*, SharedPtr<MemoryPool>> mEntityTypePoolMap;

#ifdef DW_DEBUG
    bool mIterating;
#endif

    friend class Camera;

private:
    void deleteEntity(Entity* entity);

};

NAMESPACE_END
