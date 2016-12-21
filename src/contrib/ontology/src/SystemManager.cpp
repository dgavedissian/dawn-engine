// ----------------------------------------------------------------------------
// SystemManager.cpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>
#include <ontology/Exception.hpp>
#include <ontology/SystemManager.hpp>
#include <ontology/World.hpp>
#include <ontology/Type.hpp>

#include <stdexcept>

#ifdef ONTOLOGY_THREAD
#   include <boost/asio/io_service.hpp>
#endif // ONTOLOGY_THREAD

namespace Ontology {

// ----------------------------------------------------------------------------
SystemManager::SystemManager(World* world) :
    m_World(world)
{
}

// ----------------------------------------------------------------------------
SystemManager::~SystemManager()
{
    for(auto it = m_SystemList.rbegin(); it != m_SystemList.rend(); ++it)
        it->second.reset(nullptr);
}

// ----------------------------------------------------------------------------
void SystemManager::initSystem(System* system)
{
    system->setWorld(m_World);
}

// ----------------------------------------------------------------------------
void SystemManager::initialise()
{
    this->computeExecutionOrder();
    for(const auto& it : m_SystemList)
        it.second->initialiseGuard(demangleTypeName(it.first->name()));
}

// ----------------------------------------------------------------------------
void SystemManager::update()
{
    for(const auto& system : m_ExecutionList)
        system->update();
}

// ----------------------------------------------------------------------------
void SystemManager::computeExecutionOrder()
{
    // no need to process if no systems are available
    if(!m_SystemList.size())
        return;

    // create temporary lists holding pointers to all systems so lookup time
    // is faster
    TypeMap<System*> systemLookup;
    TypeSet unresolved;
    for(const auto& it : m_SystemList)
    {
        systemLookup[it.first] = it.second.get();
        unresolved.insert(it.first);
    }

    // dependency resolution
    // see http://www.electricmonk.nl/log/2008/08/07/dependency-resolving-algorithm/
    // make sure starting point has dependencies
    TypeSet resolving;
    TypeSet::const_iterator systemIt = unresolved.begin();
    while(systemIt != unresolved.end())
    {
        systemIt = this->resolveDependencies(*systemIt, systemLookup, resolving, unresolved);
    }

#ifdef _DEBUG
    std::cout << "final execution order (" << m_ExecutionList.size() << " systems):" << std::endl;
    for(auto it : m_ExecutionList)
        for(auto m : systemLookup)
            if(it == m.second)
            {
                std::cout << "  " << demangleTypeName(m.first->name()) << std::endl;
                break;
            }
#endif
}

// ----------------------------------------------------------------------------
TypeSet::iterator SystemManager::resolveDependencies(const std::type_info* node,
                                        const TypeMap<System*>& systemLookup,
                                        TypeSet& resolving,
                                        TypeSet& unresolved)
{
    // lookup system referred to by node
    System* system = systemLookup.find(node)->second;
#ifdef _DEBUG
    std::cout << "Processing dependencies of system " << demangleTypeName(node->name()) << std::endl;
#endif

    // node is unresolved, mark as such
    resolving.insert(node);

    // loop through all depending systems
    for(const auto& edge : system->getDependingSystems())
    {
        // handle depending on a non-registered system type
        const auto& edgeSystemIt = systemLookup.find(edge);
        ONTOLOGY_ASSERT(edgeSystemIt != systemLookup.end(), InvalidSystemException, SystemManager::resolveDependencies,
            std::string("System \"") +
            demangleTypeName(node->name()) +
            "\" depends on system \"" +
            demangleTypeName(edge->name()) +
            "\", but it isn't registered as a system"
        )
        const System* edgeSystem = edgeSystemIt->second;

#ifdef _DEBUG
        std::cout << "  depends on " << edgeSystemIt->first->name() << std::endl;
#endif

        // don't process edges already part of the final execution list
        if(!this->isInExecutionList(edgeSystem))
        {
            // handle circular dependencies
            ONTOLOGY_ASSERT(resolving.find(edge) != resolving.end(), CircularDependencyException, SystemManager::resolveDependencies,
                std::string("circular dependency detected with systems \"") +
                demangleTypeName(node->name()) +
                "\" and \"" +
                demangleTypeName(edge->name()) + "\""
            )
            this->resolveDependencies(edge, systemLookup, resolving, unresolved);
        }
#ifdef _DEBUG
        else
            std::cout << "   --> already in execution list" << std::endl;
#endif
    }
    resolving.erase(resolving.find(node));

    m_ExecutionList.push_back(system);
    return unresolved.erase(unresolved.find(node));
}

// ----------------------------------------------------------------------------
bool SystemManager::isInExecutionList(const System* const system) const
{
    for(const auto& it : m_ExecutionList)
        if(it == system)
            return true;
    return false;
}

// ----------------------------------------------------------------------------
void SystemManager::onAddComponent(Entity& entity, const Component* component)
{
    for(const auto& it : m_SystemList)
        it.second->informEntityUpdate(entity);
}

// ----------------------------------------------------------------------------
void SystemManager::onRemoveComponent(Entity& entity, const Component* component)
{
    for(const auto& it : m_SystemList)
        it.second->informEntityUpdate(entity);
}

// ----------------------------------------------------------------------------
void SystemManager::onDestroyEntity(Entity& entity)
{
    for(const auto& it : m_SystemList)
        it.second->informDestroyedEntity(entity);
}

// ----------------------------------------------------------------------------
void SystemManager::onEntitiesReallocated(std::vector<Entity>& entityList)
{
    for(const auto& it : m_SystemList)
        it.second->informEntitiesReallocated(entityList);
}

} // namespace Ontology
