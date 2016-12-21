// ----------------------------------------------------------------------------
// System.hxx
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_SYSTEM_HXX__
#define __ONTOLOGY_SYSTEM_HXX__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>
#include <ontology/TypeContainers.hpp>

#include <string>

#ifdef ONTOLOGY_THREAD
#   include <boost/thread/thread.hpp>
#endif

// ----------------------------------------------------------------------------
// forward declarations

namespace Ontology {
    class Entity;
    class World;
}

namespace Ontology {

template <class... T>
inline TypeSet TypeSetGenerator();

/*!
 * @brief Used to declare that a system should not receive any entities.
 *
 * See SystemManager or SystemManager::addSystem() for more information.
 */
struct None {};

/*!
 * @brief A system acts upon entities and their components.
 *
 * It is here where all of the logic should be placed. Systems are periodically
 * udpated (when World::update() is called). Override the method
 * System::processEntity() to receive an entity that is ready to be processed
 * as an argument. Override System::initialise() to set up your System class.
 *
 * Each system will only process the entities it can support. Multiple systems
 * can process the same entity. What defines which entities are processed by
 * which systems can be defined when first instantiating the system from the
 * World class.
 *
 * @see World
 * @see SystemManager
 */
class ONTOLOGY_PUBLIC_API System
{
PRIVATE:
    typedef std::vector< std::reference_wrapper<Entity> > EntityList;
PUBLIC:

    /*!
     * @brief Default constructor.
     */
    System();

    /*!
     * @brief Allow destruction through base class pointer.
     */
    virtual ~System();
    
    /*!
     * @brief Called when systems should initialise. Override this.
     */
    virtual void initialise() = 0;

    /*!
     * @brief Called when an entity requires processing. Override this.
     */
    virtual void processEntity(Entity&) = 0;
    
    /*!
     * @brief Called when an entity wishes to be configured by this system.
     * Override this.
     */
    virtual void configureEntity(Entity&, std::string param="") = 0;
    
    /*!
     * @brief Checks if this system is initialised.
     */
    bool isInitialised() const;
    
    /*!
     * @brief Call this as many times as you wish, the system will only initialise once.
     */
    ONTOLOGY_LOCAL_API void initialiseGuard(std::string systemName);

    /*!
     * @brief Declare which components your system will support.
     * 
     * This causes only those entities that actually have all of the
     * components a system requires to be passed to the system's process loop.
     * If you don't specify anything, the system will receive all entities
     * registered to the world.
     * 
     * Supported components are supplied via a list of template
     * arguments. E.g.
     * @code
     * mySystem.supportsComponents<
     *     Position2D,
     *     Velocity2D>();
     * @endcode
     * This would cause the system to only receive entities that have at
     * least a Position2D and a Velocity2D component.
     */
    template <class... T>
    inline System& supportsComponents();

    /*!
     * @brief Gets the typeset of supported components.
     */
    ONTOLOGY_LOCAL_API const TypeSet& getSupportedComponents() const;

    /*!
     * @brief Declare which systems are required to be executed before this one.
     * 
     * This allows you to constrain the execution order of systems.
     * 
     * If nothing is specified, the default setting is to execute after the
     * last system that was added. This means that if no system declares
     * execution dependencies, all systems will be executed linearly, one after
     * another.
     * 
     * The Systems are passed as a list of template arguments. Sometimes, you'll
     * want MovementSystem to execute before CollisionSystem, in which case you
     * would write:
     * @code
     * collisionSystem.executesAfter<
     *     MovementSystem>();
     * @endcode
     */
    template <class... T>
    inline System& executesAfter();

    /*!
     * @brief Gets the typeset of depending systems.
     */
    ONTOLOGY_LOCAL_API const TypeSet& getDependingSystems() const;

    /*!
     * @brief Called by the SystemManager when it receives an update event from an entity.
     *
     * This causes the system to reconsider whether or not it can process the
     * entity in question. If it can, it will add the entity to its internal
     * list of supported entities.
     */
    ONTOLOGY_LOCAL_API void informEntityUpdate(Entity&);

    /*!
     * @brief Called by the SystemManager when it receives an entity destroyed event.
     *
     * This causes the system to remove the entity from its internal list of
     * supported entities, if it exists.
     */
    ONTOLOGY_LOCAL_API void informDestroyedEntity(const Entity&);

    ONTOLOGY_LOCAL_API void informEntitiesReallocated(std::vector<Entity>&);

    /*!
     * @brief Informs the system of the world it is part of.
     */
    ONTOLOGY_LOCAL_API void setWorld(World*);

    /*!
     * @brief Called when the system should update all of its entities.
     */
    ONTOLOGY_LOCAL_API void update();

PROTECTED:

    /*!
     * @brief Access the world the system belongs to with this.
     */
    World* world;

PRIVATE:

    TypeSet         m_SupportedComponents;
    TypeSet         m_DependingSystems;
    EntityList      m_EntityList;
    bool            m_Initialised;

#ifdef ONTOLOGY_THREAD
    void joinableThreadEntryPoint();
    void waitForNotify();
    boost::condition_variable m_ConditionVariable;
    boost::mutex m_Mutex;
    EntityList::iterator m_ThreadedEntityIterator;
#endif
};

} // namespace Ontology

#endif // __ONTOLOGY_SYSTEM_HXX__
