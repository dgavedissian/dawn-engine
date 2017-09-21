/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/Universe.h"

namespace dw {
Universe::Universe(Context* context) : Object(context) {
    root_node_ = makeShared<Transform>(Vec3::zero, Quat::identity, nullptr);
}

Universe::~Universe() {
}

void Universe::update(float) {
}

Transform* Universe::rootNode() const {
    return root_node_.get();
}
}  // namespace dw
