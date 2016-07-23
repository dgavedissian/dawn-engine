// ----------------------------------------------------------------------------
// SystemManager.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_SYSTEM_MANAGER_HPP__
#define __ONTOLOGY_SYSTEM_MANAGER_HPP__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Exception.hpp>
#include <ontology/SystemManager.hxx>
#include <ontology/System.hpp>
#include <ontology/Type.hpp>

namespace Ontology {

// ----------------------------------------------------------------------------
template <class T, class... Args>
inline T& SystemManager::addSystem(Args&&... args)
{
    return this->addPolymorphicSystem<T, T>(args...);
}

// ----------------------------------------------------------------------------
template <class Base, class Derived, class... Args>
Derived& SystemManager::addPolymorphicSystem(Args&&... args)
{
    ONTOLOGY_ASSERT(m_SystemList.find(&typeid(Base)) == m_SystemList.end(), DuplicateComponentException, "SystemManager::addPolymorphicSystem<Base, Derived, Args...>",
        std::string("System of type \"") + getTypeName<Base>() + "\" already registered with this manager"
    )
    
    Derived* system = new Derived(args...);
    m_SystemList.emplace_back(
        &typeid(Base),
        std::unique_ptr<Base>(system)
    );
    this->initSystem(system);
    return *system;
}

// ----------------------------------------------------------------------------
template <class T>
SystemManager& SystemManager::removeSystem()
{
    const auto it = m_SystemList.find(&typeid(T));
    if(it == m_SystemList.end())
        return *this;
    
    m_SystemList.erase(it);
    this->computeExecutionOrder();
    return *this;
}

// ----------------------------------------------------------------------------
template <class T>
T* SystemManager::getSystemPtr() const
{
    const auto it = m_SystemList.find(&typeid(T));
    ONTOLOGY_ASSERT(it != m_SystemList.end(), InvalidSystemException, SystemManager::getSystem<T>,
        std::string("System of type \"") + getTypeName<T>() + "\" not registered with this manager"
    )
    return static_cast<T*>(it->second.get());
}

// ----------------------------------------------------------------------------
template <class T>
inline T& SystemManager::getSystem() const
{
    return *this->getSystemPtr<T>();
}

// ----------------------------------------------------------------------------
template <class T>
bool SystemManager::hasSystem() const
{
    if(m_SystemList.find(&typeid(T)) == m_SystemList.end())
        return false;
    return true;
}

} // namespace Ontology

#endif // __ONTOLOGY_SYSTEM_MANAGER_HPP__
