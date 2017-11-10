/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
#include "ecs/Entity.h"
#include "io/InputStream.h"
#include "io/OutputStream.h"

namespace dw {

class DW_API ReplicatedPropertyBase {
public:
    virtual ~ReplicatedPropertyBase() = default;
    virtual void onAddToEntity(Entity& entity) = 0;
    virtual void serialise(OutputStream& out) = 0;
    virtual void deserialise(InputStream& in) = 0;
};

using ReplicatedPropertyPtr = SharedPtr<ReplicatedPropertyBase>;
using ReplicatedPropertyList = Vector<ReplicatedPropertyPtr>;

class DW_API ReplicatedProperty {
public:
    template <typename C, typename P> using PropertyMemberPtr = P C::*;
    template <typename C, typename P> using PropertyReferenceFunc = P& (C::*)();
    template <typename C, typename P> using PropertyGetterFunc = P (C::*)();
    template <typename C, typename P> using PropertySetterFunc = void (C::*)(const P&);

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyMemberPtr<C, P> member_ptr) {
        return makeShared<ReplicatedProperty_Member<C, P>>(member_ptr);
    }

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyReferenceFunc<C, P> reference_func) {
        return makeShared<ReplicatedProperty_ReferenceFunction<C, P>>(reference_func);
    }

    template <typename C, typename P>
    static ReplicatedPropertyPtr bind(PropertyGetterFunc<C, P> getter_func,
                                      PropertySetterFunc<C, P> setter_func) {
        return makeShared<ReplicatedProperty_Accessors<C, P>>(getter_func, setter_func);
    }

private:
    template <typename C>
    class DW_API ReplicatedProperty_Component : public ReplicatedPropertyBase {
    public:
        ReplicatedProperty_Component() : component_(nullptr) {
        }
        virtual ~ReplicatedProperty_Component() = default;

        void onAddToEntity(Entity& entity) override {
            component_ = entity.component<C>();
            assert(component_ != nullptr);
        }

    protected:
        C* component_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_Member : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_Member(PropertyMemberPtr<C, P> member_ptr) : member_ptr_(member_ptr) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, this->component_->*member_ptr_);
        }

        void deserialise(InputStream& in) override {
            this->component_->*member_ptr_ = stream::read<P>(in);
        }

    private:
        PropertyMemberPtr<C, P> member_ptr_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_ReferenceFunction : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_ReferenceFunction(PropertyReferenceFunc<C, P> reference_func)
            : reference_func_(reference_func) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, (this->component_->*reference_func_)());
        }

        void deserialise(InputStream& in) override {
            (this->component_->*reference_func_)() = stream::read<P>(in);
        }

    private:
        PropertyReferenceFunc<C, P> reference_func_;
    };

    template <typename C, typename P>
    class DW_API ReplicatedProperty_Accessors : public ReplicatedProperty_Component<C> {
    public:
        ReplicatedProperty_Accessors(PropertyGetterFunc<C, P> getter,
                                     PropertySetterFunc<C, P> setter)
            : getter_func_(getter), setter_func_(setter) {
        }

        void serialise(OutputStream& out) override {
            stream::write<P>(out, (this->component_->*getter_func_)());
        }

        void deserialise(InputStream& in) override {
            (this->component_->*setter_func_)(stream::read<P>(in));
        }

    private:
        PropertyGetterFunc<C, P> getter_func_;
        PropertySetterFunc<C, P> setter_func_;
    };
};

class DW_API NetData : public Component {
public:
    NetData(ReplicatedPropertyList properties);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

private:
    ReplicatedPropertyList properties_;
};
}  // namespace dw