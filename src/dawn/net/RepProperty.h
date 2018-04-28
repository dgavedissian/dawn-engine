/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
// An interface to a replicated property.
class DW_API RepPropertyBinding {
public:
    virtual ~RepPropertyBinding() = default;
    virtual void onAddToEntity(Entity& entity) = 0;
    virtual void serialise(OutputStream& out) = 0;
    virtual void deserialise(InputStream& in) = 0;
};

// Useful aliases.
using RepPropertyPtr = SharedPtr<RepPropertyBinding>;
using RepPropertyList = Vector<RepPropertyPtr>;

// Helper class which can be used to create type erased replicated property bindings.
class DW_API RepProperty {
public:
    template <typename Component, typename PropertyType>
    using PropertyMemberPtr = PropertyType Component::*;
    template <typename Component, typename PropertyType>
    using PropertyReferenceFunc = PropertyType& (Component::*)();
    template <typename Component, typename PropertyType>
    using PropertyGetterFunc = PropertyType (Component::*)();
    template <typename Component, typename PropertyType>
    using PropertySetterFunc = void (Component::*)(const PropertyType&);

    // Create a binding to a pointer to a member variable.
    template <typename Component, typename PropertyType>
    static SharedPtr<RepPropertyBinding> bind(
        PropertyMemberPtr<Component, PropertyType> member_ptr);

    // Create a binding to a pointer to a member function which returns a non-const reference.
    template <typename Component, typename PropertyType>
    static SharedPtr<RepPropertyBinding> bind(
        PropertyReferenceFunc<Component, PropertyType> reference_func);

    // Create a binding to two pointers to getter/setter member functions.
    template <typename Component, typename PropertyType>
    static SharedPtr<RepPropertyBinding> bind(
        PropertyGetterFunc<Component, PropertyType> getter_func,
        PropertySetterFunc<Component, PropertyType> setter_func);

private:
    // A replicated property binding which implements `onAddToEntity` for a particular component.
    template <typename Component>
    class DW_API RepPropertyBindingInComponent : public RepPropertyBinding {
    public:
        RepPropertyBindingInComponent();
        virtual ~RepPropertyBindingInComponent() = default;

        void onAddToEntity(Entity& entity) override;

    protected:
        Component* component_;
    };

    // A replicated property binding using a pointer to a member variable.
    template <typename Component, typename PropertyType>
    class DW_API RepPropertyBinding_Member : public RepPropertyBindingInComponent<Component> {
    public:
        RepPropertyBinding_Member(PropertyMemberPtr<Component, PropertyType> member_ptr);

        void serialise(OutputStream& out) override;
        void deserialise(InputStream& in) override;

    private:
        PropertyMemberPtr<Component, PropertyType> member_ptr_;
    };

    // A replicated property binding using a pointer to a member function which returns a non-const
    // reference.
    template <typename Component, typename PropertyType>
    class DW_API RepPropertyBinding_ReferenceFunction
        : public RepPropertyBindingInComponent<Component> {
    public:
        RepPropertyBinding_ReferenceFunction(
            PropertyReferenceFunc<Component, PropertyType> reference_func);

        void serialise(OutputStream& out) override;
        void deserialise(InputStream& in) override;

    private:
        PropertyReferenceFunc<Component, PropertyType> reference_func_;
    };

    // A replicated property binding using two pointers to getter/setter member functions.
    template <typename Component, typename PropertyType>
    class DW_API RepPropertyBinding_Accessors : public RepPropertyBindingInComponent<Component> {
    public:
        RepPropertyBinding_Accessors(PropertyGetterFunc<Component, PropertyType> getter,
                                     PropertySetterFunc<Component, PropertyType> setter);

        void serialise(OutputStream& out) override;
        void deserialise(InputStream& in) override;

    private:
        PropertyGetterFunc<Component, PropertyType> getter_func_;
        PropertySetterFunc<Component, PropertyType> setter_func_;
    };
};
}  // namespace dw

// Implementation.
#include "RepProperty.i.h"
