/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class Entity;
class Camera;
class PhysicsManager;

/// Class used to keep track of entities being updated
class DW_API SceneManager
{
public:
    SceneManager(PhysicsManager* physicsMgr, Ogre::SceneManager* ogreSceneMgr);
    ~SceneManager();

    template <class T, class... Args>
    T* Create(Args... args)
    {
        T* entity;
        auto i = mEntityTypePoolMap.find(&typeid(T));
        if (i != mEntityTypePoolMap.end())
        {
            entity = (*i).second->New<T>(this, args...);
        }
        else
        {
            entity = new T(this, args...);
        }
        mEntities.push_back(entity);
        return entity;
    }

    template <class T>
    void Reserve(uint slots)
    {
        mEntityTypePoolMap[&typeid(T)] = unique_ptr<MemoryPool>(new FixedMemoryPool<T>(slots));
    }

    void ClearEntities();

    /// Remove an entity by value. This is quite slow with O(n) complexity.
    /// @param entity The entity to find and remove from the entity list
    DEPRECATED void RemoveEntity(Entity* entity);

    /// Calls update on each entity
    /// @param dt Time elapsed
    void Update(float dt);

    /// Calls PreRender on each entity
    void PreRender(Camera* camera);

    /// Gets the physics manager associated with this scene manager
    PhysicsManager* GetPhysicsMgr() { return mPhysicsManager; }

private:
    PhysicsManager* mPhysicsManager;
    Ogre::SceneManager* mSceneMgr;

    // Owned entities
    std::vector<Entity*> mEntities;

    // Memory pools
    std::map<const std::type_info*, unique_ptr<MemoryPool>> mEntityTypePoolMap;

#ifdef DW_DEBUG
    bool mIterating;
#endif

    friend class Camera;

private:
    void DeleteEntity(Entity* entity);

};

NAMESPACE_END
