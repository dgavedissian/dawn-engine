/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
using ComponentId = dw::StringHash::HashType;

class DW_API Component : public Object {
public:
    DW_OBJECT(Component);

    Component(Context* context) = default;

    virtual void update(float dt) {}

    ComponentId id() const { return type().value(); }

};
}
