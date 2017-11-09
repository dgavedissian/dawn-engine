/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "ecs/Component.h"
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

using ReplicatedPropertyPtr = UniquePtr<ReplicatedPropertyBase>;

template <typename C, typename P>
class DW_API ReplicatedProperty : public ReplicatedPropertyBase {
public:
    ReplicatedProperty(P C::*member_ptr) :
        component_member_ptr_{member_ptr}, component_{nullptr}{
    }

    void onAddToEntity(Entity& entity) override {
        component_ = entity.component<C>();
    }

    void serialise(OutputStream& out) override {
        assert(component_);
        stream::write<P>(out, component_->*component_member_ptr_);
    }

    void deserialise(InputStream& in) override {
        assert(component_);
        component_->*component_member_ptr_ = stream::read<P>(in);
    }

    static ReplicatedPropertyPtr bind(P C::*member_ptr) {
        return makeUnique<ReplicatedProperty<C, P>>(member_ptr);
    }

private:
    P C::*component_member_ptr_;
    C* component_;
};

class DW_API NetData : public Component {
public:
    NetData(const Vector<ReplicatedPropertyPtr> properties);
    void onAddToEntity(Entity* parent);

    void serialise(OutputStream& out);
    void deserialise(InputStream& in);

private:
    Vector<ReplicatedPropertyPtr> properties_;
};
}