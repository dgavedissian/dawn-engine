// ----------------------------------------------------------------------------
// EntityManagerListener.cpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include <ontology/EntityManagerListener.hpp>

namespace Ontology {

// ----------------------------------------------------------------------------
void EntityManagerListener::onCreateEntity(Entity&)
{
}

// ----------------------------------------------------------------------------
void EntityManagerListener::onDestroyEntity(Entity&)
{
}

// ----------------------------------------------------------------------------
void EntityManagerListener::onAddComponent(Entity&, const Component*)
{
}

// ----------------------------------------------------------------------------
void EntityManagerListener::onRemoveComponent(Entity&, const Component*)
{
}

// ----------------------------------------------------------------------------
void EntityManagerListener::onEntitiesReallocated(std::vector<Entity>&)
{
}

} // namespace Ontology
