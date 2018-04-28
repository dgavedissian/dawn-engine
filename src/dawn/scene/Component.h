/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "ontology/Component.hpp"

namespace dw {
class Entity;

class DW_API Component : public Ontology::Component {
public:
    virtual ~Component() = default;

    virtual void onAddToEntity(Entity* parent);
};
}  // namespace dw
