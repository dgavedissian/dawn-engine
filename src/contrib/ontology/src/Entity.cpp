// ----------------------------------------------------------------------------
// Entity.cpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include <ontology/Entity.hpp>
#include <ontology/System.hpp>

namespace Ontology {

Entity::ID Entity::GUIDCounter = 0;

// ----------------------------------------------------------------------------
Entity::Entity(const char* name, const EntityManagerInterface* creator) :
    m_Name(name),
    m_Creator(creator),
    m_ID(GUIDCounter++)
{
}

// ----------------------------------------------------------------------------
Entity::~Entity()
{
    // dispatch remove component events
    for(const auto& it : m_ComponentMap)
        m_Creator->informRemoveComponent(*this, it.second.get());
}

// ----------------------------------------------------------------------------
bool Entity::supportsSystem(const System& system) const
{
    for(const auto& it : system.getSupportedComponents())
        if(m_ComponentMap.find(it) == m_ComponentMap.end())
            return false;
    return true;
}

// ----------------------------------------------------------------------------
const char* Entity::getName() const
{
    return m_Name;
}

// ----------------------------------------------------------------------------
Entity::ID Entity::getID() const
{
    return m_ID;
}

} // namespace Ontology
