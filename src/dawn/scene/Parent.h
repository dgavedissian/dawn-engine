/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "ecs/Component.h"
#include "ecs/Entity.h"

namespace dw {
struct Parent : public Component {
    Parent(EntityId parent) : parent{parent} {
    }

    EntityId parent;
};
}
