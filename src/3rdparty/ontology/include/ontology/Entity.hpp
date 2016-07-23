//----------------------------------------------------------------------------
//Entity.hpp
//----------------------------------------------------------------------------

#ifndef __ONTOLOGY_ENTITY_HPP__
#define __ONTOLOGY_ENTITY_HPP__

//----------------------------------------------------------------------------
// include files

#include <ontology/Component.hpp>
#include <ontology/Entity.hxx>
#include <ontology/EntityManager.hpp>
#include <ontology/Exception.hpp>
#include <ontology/SystemManager.hpp>
#include <ontology/World.hpp>
#include <ontology/Type.hpp>

namespace Ontology {

//----------------------------------------------------------------------------
template<class T, class... Args>
Entity& Entity::addComponent(Args&&... args)
{
    ONTOLOGY_ASSERT(m_ComponentMap.find(&typeid(T)) == m_ComponentMap.end(), DuplicateComponentException, Entity::addComponent<T>,
        std::string("Component of type \"") + getTypeName<T>() + "\" already registered with this entity"
    )

    Component* component = new T(args...);
    m_ComponentMap[&typeid(T)] = std::unique_ptr<Component>(component);
    m_Creator->informAddComponent(*this, component);
    return *this;
}

//----------------------------------------------------------------------------
template<class T>
void Entity::removeComponent()
{
    const auto it = m_ComponentMap.find(&typeid(T));
    if(it == m_ComponentMap.end())
        return;
    
    m_Creator->informRemoveComponent(*this, it->second.get());
    m_ComponentMap.erase(it);
}

//----------------------------------------------------------------------------
template<class T>
inline T& Entity::getComponent() const
{
    return *this->getComponentPtr<T>();
}

//----------------------------------------------------------------------------
template<class T>
T* Entity::getComponentPtr() const
{
    const auto it = m_ComponentMap.find(&typeid(T));
    ONTOLOGY_ASSERT(it != m_ComponentMap.end(), InvalidComponentException, Entity::getComponent<T>,
        std::string("Component of type \"") + getTypeName<T>() + "\" not registered with this entity"
    )
    return static_cast<T*>(it->second.get());
}

//----------------------------------------------------------------------------
template <class T>
bool Entity::hasComponent() const
{
    const auto it = m_ComponentMap.find(&typeid(T));
    if(it == m_ComponentMap.end())
        return false;
    return true;
}

//----------------------------------------------------------------------------
template <class T>
Entity& Entity::configure(std::string param)
{
    SystemManager& sm = m_Creator->world->getSystemManager();
    ONTOLOGY_ASSERT(sm.hasSystem<T>(), InvalidSystemException, Entity::configure<T>,
        std::string("System of type \"" + getTypeName<T>() + "\" was not found when trying to configure this entity")
    )
    static_cast<System*>(sm.getSystemPtr<T>())->configureEntity(*this, param);
    return *this;
}

} // namespace Ontology

#endif // __ONTOLOGY_ENTITY_HPP__
