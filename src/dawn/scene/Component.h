/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

namespace dw {
class Entity;

// TODO: Kill this class.
class DW_API Component {
public:
    virtual ~Component() = default;

    // TODO: Kill this.
    virtual void onAddToEntity(Entity* parent);
};
}  // namespace dw
