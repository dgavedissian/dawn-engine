// ----------------------------------------------------------------------------
// SystemManager.hxx
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_SYSTEM_MANAGER_HXX__
#define __ONTOLOGY_SYSTEM_MANAGER_HXX__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>
#include <ontology/EntityManagerListener.hpp>
#include <ontology/TypeContainers.hpp>

#include <iostream>
#include <set>
#include <cassert>

// ----------------------------------------------------------------------------
// forward declarations

namespace Ontology {
    class System;
    class World;
}

namespace Ontology {

/*!
 * @brief Manages the creation and deletion of systems.
 *
 * The system manager can be retrieved from an instantiated world with
 * World::getSystemManager(). The system manager is responsible for creating,
 * destroying and updating your systems.
 *
 * Systems can be configured to update only a selection of entities based on
 * the requirements of components, and can also be configured to execute before
 * other systems (execution dependencies). See SystemManager::addSystem for
 * more information.
 *
 * @note SystemManager supports chaining and the programmer is encouraged to
 * use this feature. The following demonstrates the recommended syntax:
 * @code
 * // creates and registers some systems with our world
 * world.getSystemManager().addSystem<MainLoop>()
 *     .suppportsComponents<
 *         Ontology::None>());
 * world.getSystemManager().addSystem<MovementSystem>()
 *     .supportsComponents<
 *             Position,
 *             Velocity>()
 *     .executeAfter<
 *             CollisionSystem>());
 * world.getSystemManager().addSystem<CollisionSystem>()
 *     .supportsComponents<
 *             Position>());
 * 
 * // initialises all systems
 * world.getSystemManager().initialise();
 * @endcode
 * Here, **MainLoop** will not receive any entities at all because
 * Ontology::None was declared. **MovementSystem** will only receive entities that
 * have a Position and Velocity. **CollisionSystem* will only receive entities that
 * have a Position.
 *
 * Also note the use of **executesAfter()**, which guarantees
 * CollisionSystem to be executed **before** MovementSystem.
 *
 * @note Make sure to call SystemManager::initialise() after adding all of your
 * systems.
 */
class ONTOLOGY_PUBLIC_API SystemManager :
    public EntityManagerListener
{
PUBLIC:

    /*!
     * @brief Default constructor.
     */
    SystemManager(World* world);

    /*!
     * @brief Default destructor.
     *
     * Will destroy all systems in reverse order to how they were added.
     */
    ~SystemManager();

    /*!
     * @brief Adds a new system to the world.
     *
     * The type of system is passed as a template argument. If your system
     * class has a constructor that requires arguments, those can be directly
     * supplied as arguments to this function.
     * @code
     * world.getSystemManager().addSystem<MySystem>(ConstructorArg1, ...);
     * @endcode
     * @note Systems are stored such that the order in which they are added
     * to the system manager is the order in which they are initialised.
     * Their destructors are called in reverse order when destroying the world.
     *
     * @return Returns a reference to the newly added system.
     */
    template <class T, class... Args>
    inline T& addSystem(Args&&... args);
    
    /*!
     * @brief Adds a new polymorphic system to the world.
     * 
     * The base type of the system is passed as the first template argument,
     * the derived type of the system is passed as the second template
     * argument. If your system class has a constructor that requires
     * arguments, those can be directly supplied as arguments to this function.
     * @code
     * world.getSystemManager().addSystem<BaseClass, DerivedClass>(ConstructorArg1, ...);
     * @endcode
     * @note Systems are stored such that the order in which they are added
     * to the system manager is the order in which they are initialised.
     * Their destructors are called in reverse order when destroying the world.
     * @return Returns a derived reference to the newly added system.
     */
    template <class Base, class Derived, class... Args>
    Derived& addPolymorphicSystem(Args&&... args);

    /*!
     * @brief Removes the specified system from the world.
     *
     * Example:
     * @code
     * world.getSystemManager().removeSystem<MovementSystem>();
     * @endcode
     * @return Returns a reference to this SystemManager, allowing the user
     * to chain.
     */
    template <class T>
    SystemManager& removeSystem();
    
    /*!
     * @brief Gets the specified system.
     * 
     * Example:
     * @code
     * world.getSystemManager().getSystem<MovementSystem>()->doThings();
     * @endcode
     * @return A pointer to the specified System
     */
    template <class T>
    T* getSystemPtr() const;

    /*!
     * @brief Gets the specified system.
     * 
     * Example:
     * @code
     * world.getSystemManager().getSystem<MovementSystem>().doThings();
     * @endcode
     * @return A reference to the specified system.
     */
    template <class T>
    inline T& getSystem() const;
    
    /*!
     * @brief Check to see if a system exists.
     * 
     * The system type to check for is passed as a template parameter.
     * @code
     * if(world.getSystemManager().hasSystem<MySystem>())
     * {
     *     // do stuff...
     * }
     * @endcode
     * @return Returns true if the specified system exists, otherwise false is
     * returned.
     */
    template <class T>
    bool hasSystem() const;

    /*!
     * @brief Passes important data to a new System object so it functions correctly.
     * @note Should not be called by the user. This is an internal function.
     */
    ONTOLOGY_LOCAL_API void configureSystem();

    /*!
     * @brief Passes required information to the specified system so it is functional.
     * @note Should not be called by the user. This is an internal function.
     */
    void initSystem(System*);

    /*!
     * @brief Initialises all currently registered systems.
     *
     * @note The order in which the systems are added is the order in which
     * they are initialised.
     */
    void initialise();

    /*!
     * @brief Updates all currently registered systems. Don't call this.
     *
     * @note Don't call this manually, use World::update() instead.
     */
    void update();

PRIVATE:

    // EntityManagerListener methods
    void onDestroyEntity(Entity&) override;
    void onAddComponent(Entity&, const Component*) override;
    void onRemoveComponent(Entity&, const Component*) override;
    void onEntitiesReallocated(std::vector<Entity>&) override;

    /*!
     * @brief Triggers dependency resolution of the system execution order.
     *
     * Systems are able to define which other systems should be executed before
     * them. This method resolves these dependencies so the execution order
     * becomes known.
     */
    void computeExecutionOrder();

    /*!
     * @brief Resolves system dependencies.
     */
    TypeSet::iterator resolveDependencies(const std::type_info* node,
                             const TypeMap<System*>& systemLookup,
                             TypeSet& resolving,
                             TypeSet& unresolved);

    /*!
     * @brief Returns true if the specified system is part of the execution list.
     */
    bool isInExecutionList(const System* const) const;

    TypeVectorPairUniquePtr<System> m_SystemList;
    std::vector<System*>            m_ExecutionList;
    World*                          m_World;
};

} // namespace Ontology

#endif // __ONTOLOGY_SYSTEM_MANAGER_HXX__
