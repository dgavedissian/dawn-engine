// ----------------------------------------------------------------------------
// EntityManager.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_ENTITY_MANAGER_HPP__
#define __ONTOLOGY_ENTITY_MANAGER_HPP__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>
#include <ontology/EntityManagerInterface.hpp>
#include <ontology/ListenerDispatcher.hpp>

#include <vector>
#include <memory>

// ----------------------------------------------------------------------------
// forward declarations

namespace Ontology {
    class Component;
    class Entity;
    class EntityManagerListener;
}

namespace Ontology {

/*!
 * @brief Manages the creation and destruction of entities.
 *
 * This class is used to create and destroy entities. When you create a World,
 * you can access this class with World::getEntityManager().
 *
 * @see Entity
 * @see World
 */
class ONTOLOGY_PUBLIC_API EntityManager : public EntityManagerInterface
{
PUBLIC:

    typedef std::vector<Entity> EntityList;

    /*!
     * @brief Construct with world pointer.
     */
    EntityManager(World*);

    /*!
     * @brief Default destructor
     */
    ~EntityManager();

    /*!
     * @brief Creates a new entity and adds it to the world.
     *
     * @param name The name to give the new entity. Doesn't have to be globally
     * unique.
     * @return Returns a reference to the created entity for chaining purposes.
     * @warning The returned reference may become invalidated if you add
     * another entity. Behind the scenes, entities are stored in contiguous
     * memory for optimisation, and adding an entity could cause a re-allocation.
     * 
     * If you want to hold on to the created entity for future use, then call
     * Entity::getID() and store its globally unique identifier. When you need
     * to access the entity again, use EntityManager::getEntity() to get the
     * Entity assigned to that ID.
     */
    Entity& createEntity(const char* name="") override;

    /*!
     * @brief Destroys the specified entity.
     * @param entity The entity to destroy.
     */
    void destroyEntity(Entity& entity) override;

    /*!
     * @brief Destroys all entities sharing the specified name.
     * @param name The name to search for.
     */
    void destroyEntities(const char* name) override;

    /*!
     * @brief Destroys all entities. Everything.
     */
    void destroyAllEntities() override;
    
    /*!
     * @brief Gets a reference to the entity object.
     * @warning The returned reference may become invalidated if you add
     * another entity. Behind the scenes, entities are stored in contiguous
     * memory for optimisation, and adding an entity could cause a re-allocation.
     * @param entityID The identifier of the entity you wish to get the
     * reference of.
     * @return The reference to the entity.
     */
    Entity& getEntity(Entity::ID entityID) override;

    /*!
     * @brief Gets a list of entities of type EntityManager::EntityList.
     *
     * You can iterate over the list using standard iterators.
     */
    const EntityList& getEntityList() const;

    /*!
     * @brief Register as an EntityManagerListener to listen to EntityManager events.
     */
    ListenerDispatcher<EntityManagerListener> event;

PRIVATE:

    /*!
     * @brief Called by entities when they add a new component.
     * @param entity The entity adding a new component.
     * @param component The component being added.
     */
    void informAddComponent(Entity& entity, const Component* component) const override;

    /*!
     * @brief Called by entities when they remove a component.
     * @param entity The entity removing a component.
     * @param component The component being removed.
     */
    void informRemoveComponent(Entity& entity, const Component* component) const override;

    /*!
     * @brief Dispatches a reallocation event if the capacity has changed.
     */
    void handleEntityReallocation(bool force=false);

    EntityList m_EntityList;
    std::size_t m_EntityListCapacity;
};

} // namespace Ontology

#endif // __ONTOLOGY_ENTITY_MANAGER_HPP__
