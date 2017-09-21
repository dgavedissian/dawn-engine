/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "scene/SceneManager.h"

namespace dw {

SceneManager::SceneManager(Context* context) : Object(context) {
    root_node_ = makeShared<Transform>(Vec3::zero, Quat::identity, nullptr);
}

SceneManager::~SceneManager() {
}

void SceneManager::update(float) {
}

Transform* SceneManager::rootNode() const {
    return root_node_.get();
}

void SceneManager::preRender(Camera_OLD*) {
}
}  // namespace dw
