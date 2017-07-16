//----------------------------------------------------------------------------
//Entity.hxx
//----------------------------------------------------------------------------

#ifndef __ONTOLOGY_ENTITY_HXX__
#define __ONTOLOGY_ENTITY_HXX__

//----------------------------------------------------------------------------
//include files

#include <ontology/Config.hpp>
#include <ontology/TypeContainers.hpp>

#include <map>
#include <typeinfo>
#include <string>
#include <cassert>
#include <memory>

//----------------------------------------------------------------------------
//forward declarations

namespace Ontology{
    class Component;
    struct EntityManagerInterface;
    class System;
}

namespace Ontology{

/*!
 * @brief An entity is used to bind components together.
 *
 * Entities can be created through the world and are primarily used to simply
 * contain Components. You can add or remove components from an entity by using
 * its methods Entity::addComponent(new MyComponent) and
 * Entity::removeComponent<Component>(). You can retrieve and modify a
 * component's data at any time with Entity::getComponent<Component>().
 *
 * @note
 * Please note that all methods support chaining, and that the recommended
 * creation of entities is through chaining. The following is an example
 * demonstrating this:
 * @code
 * world.getEntityManager().createEntity("My Entity")
 *     .addComponent<Position>(0, 0)
 *     .addComponent<Velocity>(1, 2)
 *     .addComponent<Sprite>("player.png")
 *     ;
 * @endcode
 * @see Component
 */
class Entity
{
PUBLIC:

    typedef std::size_t ID;

    /*!
     * @brief Construct an entity with a name.
     */
    Entity(const char* name, const EntityManagerInterface* creator);

    /*!
     * @brief Allow destruction through base class pointer.
     */
    virtual ~Entity();

    /*!
     * @brief Add a component to this entity.
     *
     * @param component The component to add to this Entity. The Component
     * type is passed as a template argument. If the Component has a
     * constructor that takes arguments, you may supply those directly as
     * arguments to this function call. E.g.
     * @code
     * myEntity.addComponent<Position2D>(4.2, 6.0);
     * @endcode
     *
     * @note You can only register one instance of every type of component.
     * @return Returns a reference to this Entity. This is to allow chaining.
     */
    template<class T, class... Args>
    Entity& addComponent(Args&&...);

    /*!
     * @brief Remove a component from this entity.
     * 
     * The component to be removed is passed as a template argument. E.g.
     * @code
     * myEntity.removeComponent<Position2D>();
     * @endcode
     */
    template<class T>
    void removeComponent();

    /*!
     * @brief Get a component from the entity.
     * @return A reference to the requested component.
     */
    template <class T>
    inline T& getComponent() const;
    
    /*!
     * @brief Get a component from the entity.
     * @return A pointer to the requested component.
     */
    template <class T>
    T* getComponentPtr() const;
    
    /*!
     * @brief Checks if the specified component exists.
     * @return True if the entity has the specified component, false if otherwise.
     */
    template <class T>
    bool hasComponent() const;
    
    /*!
     * @brief Tells the specified system to configure this entity.
     */
    template <class T>
    Entity& configure(std::string param="");

    /*!
     * @brief Returns true if this entity is supported by the specified system.
     *
     * If the entity owns all the components the specified system has requested
     * then the system is able to support this entity. If the system has an
     * empty list of supported components, then this method will also return
     * true.
     */
    bool supportsSystem(const System&) const;

    /*!
     * @brief Gets the name of the entity.
     * @return A const char pointer to the entity's name string.
     */
    const char* getName() const;
    
    /*!
     * @brief Gets this entity's globally unique Identifier.
     */
    ID getID() const;

PRIVATE:

    static ID                       GUIDCounter;
    ID                              m_ID;
    TypeMapSharedPtr<Component>     m_ComponentMap;
    const char*                     m_Name;
    const EntityManagerInterface*   m_Creator;
};

} // namespace Ontology

#endif // __ONTOLOGY_ENTITY_HXX__
