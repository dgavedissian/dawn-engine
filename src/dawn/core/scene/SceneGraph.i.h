/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
template <typename T>
SceneGraph<T>::SceneGraph()
    : root_(&pool_, LargePosition::origin, Quat::identity),
      background_root_(&pool_, Vec3::zero, Quat::identity, Vec3::one) {
}

template <typename T> SceneGraph<T>::~SceneGraph() {
}

template <typename T> LargeSceneNode<T>& SceneGraph<T>::root() {
    return root_;
}

template <typename T> SceneNode<T>& SceneGraph<T>::backgroundNode() {
    return background_root_;
}

}  // namespace dw
