// ----------------------------------------------------------------------------
// EntityManagerInterface.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_ENTITY_MANAGER_INTERFACE_HPP__
#define __ONTOLOGY_ENTITY_MANAGER_INTERFACE_HPP__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>
#include <ontology/Entity.hxx>

namespace Ontology {

class Component;
class Entity;
class World;

struct EntityManagerInterface
{
    EntityManagerInterface(World* world) : world(world) {}
    virtual ~EntityManagerInterface() {}
    virtual Entity& createEntity(const char* name="") = 0;
    virtual void destroyEntity(Entity& entity) = 0;
    virtual void destroyEntities(const char* name) = 0;
    virtual void destroyAllEntities() = 0;
    virtual Entity& getEntity(Entity::ID) = 0;
    ONTOLOGY_LOCAL_API virtual void informAddComponent(Entity& entity, const Component* component) const = 0;
    ONTOLOGY_LOCAL_API virtual void informRemoveComponent(Entity& entity, const Component* component) const = 0;
    World* world;
};

} // namespace Ontology

#endif // __ONTOLOGY_ENTITY_MANAGER_INTERFACE_HPP__
