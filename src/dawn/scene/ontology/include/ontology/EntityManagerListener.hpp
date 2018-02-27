// ----------------------------------------------------------------------------
// EntityManagerListener.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_ENTITY_MANAGER_LISTENER_HPP__
#define __ONTOLOGY_ENTITY_MANAGER_LISTENER_HPP__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>

#include <vector>

// ----------------------------------------------------------------------------
// forward declarations

namespace Ontology {
    class Component;
    class Entity;
}

namespace Ontology {

/*!
 * @brief Listener interface for classes that want to listen to EntityManager events.
 */
class ONTOLOGY_PUBLIC_API EntityManagerListener
{
PUBLIC:

    /*!
     * @brief Called when a new entity was created.
     * @param entity The new entity.
     */
    virtual void onCreateEntity(Entity& entity);

    /*!
     * @brief Called when an entity is destroyed.
     * @param entity The entity being destroyed.
     */
    virtual void onDestroyEntity(Entity& entity);

    /*!
     * @brief Called when an entity adds a new component.
     * @param entity The entity adding a new component.
     * @param component The new component being added.
     */
    virtual void onAddComponent(Entity& entity, const Component* component);

    /*!
     * @brief Called when an entity removes a component.
     * @param entity The entity removing the component.
     * @param component The component being removed.
     */
    virtual void onRemoveComponent(Entity& entity, const Component* component);

    /*!
     * @brief Called when EntityManager has re-allocated the memory for its entities
     */
    virtual void onEntitiesReallocated(std::vector<Entity>& entityList);
};

} // namespace Ontology

#endif // __ONTOLOGY_ENTITY_MANAGER_LISTENER_HPP__
