// ----------------------------------------------------------------------------
// ListenerDispatcher.hpp
// ----------------------------------------------------------------------------

#ifndef __ONTOLOGY_LISTENER_DISPATCHER_HPP__
#define __ONTOLOGY_LISTENER_DISPATCHER_HPP__

// ----------------------------------------------------------------------------
// include files

#include <ontology/Config.hpp>

#include <map>
#include <string>

namespace Ontology {

/*!
 * @brief Generic class for handling dispatching messages to listeners.
 * A listener (or better known as an **observer**) is an object which can
 * register itself to a dispatcher in order to receive notifications of
 * specific events by using callback functions.
 *
 * For this to work, three classes need to exist. The **Listener Interface**,
 * the **Listener** - which must inherit the listener interface and implement
 * its abstract methods - and a **dispatcher**.
 *
 * The dispatcher holds a list of objects inheriting the listener interface,
 * and provides functions through which the programmer can dispatch an event
 * to all registered objects.
 *
 * The following code example demonstrates this behaviour.
 * @code
 * struct ListenerInterface {
 *     virtual void printMessage(std::string message) = 0;
 * }
 *
 * struct Listener : public ListenerInterface {
 *     virtual void printMessage(std::string message) { std::cout << "received: " << message << std::endl; }
 * }
 *
 * int main() {
 *
 *     // instantiate the dispatcher object. The template argument specifies
 *     // the type of listener objects it should manage.
 *     ListenerDispatcher<ListenerInterface> dispatcher;
 *
 *     // instantiate some objects derived from the "ListenerInterface" class
 *     // and register them to the dispatcher object so they can receive
 *     // notifications
 *     Listener a, b;
 *     dispatcher.addListener(&a, "listener 1");
 *     dispatcher.addListener(&b, "listener 2");
 *
 *     // dispatch a message using the dispatcher. All registered objects
 *     // will receive this message
 *     dispatcher.dispatch(&ListenerInterface::printMessage, "hello world!");
 *
 *     return 0;
 * }
 * @endcode
 */
template <class LISTENER_CLASS>
class ONTOLOGY_PUBLIC_API ListenerDispatcher
{
PUBLIC:

    /*!
     * @brief Default constructor
     */
    ListenerDispatcher();

    /*!
     * @brief Default destructor
     */
    ~ListenerDispatcher();

    /*!
     * @brief Registers a listener to be notified on events.
     * The listener object must derive from the listener interface.
     * @note If the listener is already registered, this method will silently
     * fail.
     * @param listener A pointer to the listener object to register.
     * @param listenerName A globally unique identifier string for this
     * listener.
     */
    void addListener(LISTENER_CLASS* listener, std::string listenerName);

    /*!
     * @brief Unregisters a listener by pointer
     * @note If the listener is not registered to begin with, this method will
     * silently fail.
     * @param listener A pointer to a listener to unregister
     */
    void removeListener(LISTENER_CLASS* listener);

    /*!
     * @brief Unregisters a listener by name
     * @note If the listener is not registered to begin with, this method will
     * silently fail.
     * @param listenerName A unique string identifying the listener.
     */
    void removeListener(std::string listenerName);

    /*!
     * @brief Removes all listeners
     */
    void removeAllListeners();

    /*!
     * @brief Dispatches a message to all listeners
     * @param func A pointer to a member function of the listener interface class.
     * For example:
     * @code &ListenerInterface::doThing @endcode
     * @param params Parameter list of values to be dispatched to the
     * listeners. These must map the function signature of the function declared
     * in the listener interface.
     */
    template <class RET_TYPE, class... ARGS, class... PARAMS>
    void dispatch(RET_TYPE (LISTENER_CLASS::*func)(ARGS...), PARAMS&&... params) const;

    /*!
     * @brief Dispatches a message to all listeners
     * If any of the listeners return false, then this method will return false.
     * If all listeners return true, then this method will return true.
     * @note As soon as a listener returns false, this method will return. All
     * listeners that would have been notified afterwards are skipped.
     * @param func A pointer to a member function of the listener class.
     * For example:
     * @code &ListenerInterface::doThing @endcode
     * @param params Parameter list of values to be dispatched to the
     * listeners. These must map the function signature of the function declared
     * in the listener interface.
     */
    template <class... ARGS, class... PARAMS>
    bool dispatchAndFindFalse(bool (LISTENER_CLASS::*func)(ARGS...), PARAMS&&... params) const;

PRIVATE:

    std::map<std::string, LISTENER_CLASS*> m_Listeners;
};

} // namespace Ontology

#endif // __ONTOLOGY_LISTENER_DISPATCHER_HPP__