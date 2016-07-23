// ----------------------------------------------------------------------------
// ListenerDispatcher.hpp
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// include files

#include <ontology/ListenerDispatcher.hxx>
#include <ontology/Type.hpp>

#ifdef _DEBUG
#   include <iostream>
#   include <typeinfo>
#endif // _DEBUG

namespace Ontology {

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
ListenerDispatcher<LISTENER_CLASS>::ListenerDispatcher()
{
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
ListenerDispatcher<LISTENER_CLASS>::~ListenerDispatcher()
{
#ifdef _DEBUG
    for(auto it : m_Listeners)
        std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
                << ">::~ListenerDispatcher] Warning: Listener \"" << it.first
                << "\" still registered, yet dispatcher is being destroyed!"
                << std::endl;
#endif // _DEBUG
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
void ListenerDispatcher<LISTENER_CLASS>::addListener(LISTENER_CLASS* listener, std::string listenerName)
{
#ifdef _DEBUG
    if(m_Listeners.find(listenerName) != m_Listeners.end())
    {
        std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
                  << ">::::addListener] Warning: listenerName \""
                  << listenerName << "\" already registered" << std::endl;
        return;
    }
    for(auto it : m_Listeners)
    {
        if(it.second == listener)
        {
            std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
                      << ">::addListener] Warning: listener pointer already registered"
                      << std::endl;
            return;
        }
    }
#endif // _DEBUG

    m_Listeners[listenerName] = listener;
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
void ListenerDispatcher<LISTENER_CLASS>::removeListener(LISTENER_CLASS* listener)
{
    for(auto it = std::begin(m_Listeners); it != std::end(m_Listeners); ++it)
    {
        if(it->second == listener)
        {
            m_Listeners.erase(it);
            return;
        }
    }
#ifdef _DEBUG
    std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
              << ">::removeListener] Warning: listener pointer not registered"
              << std::endl;
#endif // _DEBUG
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
void ListenerDispatcher<LISTENER_CLASS>::removeListener(std::string listenerName)
{
    auto it = m_Listeners.find(listenerName);
    if(it == m_Listeners.end())
    {
#ifdef _DEBUG
        std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
                  << ">::removeListener] Warning: listener \"" << listenerName
                  << "\" not registered" << std::endl;
#endif // _DEBUG
        return;
    }
    m_Listeners.erase(it);
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
void ListenerDispatcher<LISTENER_CLASS>::removeAllListeners()
{
#ifdef _DEBUG
    for(auto it : m_Listeners)
        std::cout << "[ListenerDispatcher<" << getTypeName<LISTENER_CLASS>()
                << ">::removeAllListeners] Warning: Listener \"" << it.first
                << "\" still registered" << std::endl;
#endif // _DEBUG

    m_Listeners.clear();
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
template <class RET_TYPE, class... ARGS, class... PARAMS>
void ListenerDispatcher<LISTENER_CLASS>::
    dispatch(RET_TYPE (LISTENER_CLASS::*func)(ARGS...), PARAMS&&... params) const
{
    for(auto it : m_Listeners)
        (it.second->*func)(params...);
}

// ----------------------------------------------------------------------------
template <class LISTENER_CLASS>
template <class... ARGS, class... PARAMS>
bool ListenerDispatcher<LISTENER_CLASS>::
    dispatchAndFindFalse(bool (LISTENER_CLASS::*func)(ARGS...), PARAMS&&... params) const
{
    for(auto it : m_Listeners)
        if(!(it.second->*func)(params...))
            return false;
    return true;
}

} // namespace Ontology
