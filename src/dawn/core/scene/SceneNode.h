/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "scene/Component.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/math/Defs.h"
#include "core/scene/LargePosition.h"

namespace dw {

template <typename T> class SceneNode;
template <typename T> class LargeSceneNode;

namespace detail {
template <typename T> class SceneNodePool {
public:
    LargeSceneNode<T>* newLargeSceneNode(const LargePosition& p, const Quat& o);
    SceneNode<T>* newSceneNode(const Vec3& p, const Quat& o, const Vec3& s);

    void free(LargeSceneNode<T>* node);
    void free(SceneNode<T>* node);
};

struct Transform {
    Vec3 position;
    Quat orientation;
    Vec3 scale;

    Mat4 calculateModelMatrix() const;
};
}  // namespace detail

template <typename T> class LargeSceneNode {
public:
    LargeSceneNode(detail::SceneNodePool<T>* pool, const LargePosition& p, const Quat& o);

    DEPRECATED void setRelativeToCamera(bool relative_to_camera);

    /// Note: Model matrices only make sense if they're relative to a position.
    Mat4 calculateModelMatrix(const LargePosition& camera_position) const;

    /// Note: View matrices are always based on the origin.
    Mat4 calculateViewMatrix() const;

    LargeSceneNode<T>* parent() const;

    LargeSceneNode<T>* addChild(LargeSceneNode<T>* child);
    SceneNode<T>* addChild(SceneNode<T>* child);
    LargeSceneNode<T>* newLargeChild(const LargePosition& p = LargePosition::origin,
                                     const Quat& o = Quat::identity);
    SceneNode<T>* newChild(const Vec3& p = Vec3::zero, const Quat& o = Quat::identity,
                           const Vec3& s = Vec3::one);
    LargeSceneNode<T>* largeChild(int i);
    SceneNode<T>* child(int i);
    int largeChildCount();
    int childCount();

    // Transform.
    LargePosition position;
    Quat orientation;

    // Container data.
    T data;

private:
    bool relative_to_camera_;
    LargeSceneNode<T>* parent_;
    Vector<LargeSceneNode<T>*> large_children_;
    Vector<SceneNode<T>*> children_;
    byte depth_;

    detail::SceneNodePool<T>* pool_;

    void detachFromParent();

    friend class SceneNode<T>;
};

template <typename T> class SceneNode {
public:
    SceneNode(detail::SceneNodePool<T>* pool, const Vec3& p, const Quat& o, const Vec3& s);

    Mat4 calculateModelMatrix() const;

    LargeSceneNode<T>* largeParent() const;
    SceneNode<T>* parent() const;

    SceneNode<T>* addChild(SceneNode<T>* child);
    SceneNode<T>* newChild(const Vec3& p = Vec3::zero, const Quat& o = Quat::identity,
                           const Vec3& s = Vec3::zero);
    SceneNode<T>* child(int i);
    int childCount();

    // Transform.
    detail::Transform& transform();
    const detail::Transform& transform() const;
    bool dirty;

    // Container data.
    T data;

private:
    detail::Transform transform_;
    SceneNode<T>* parent_;
    LargeSceneNode<T>* large_parent_;
    Vector<SceneNode<T>*> children_;
    byte depth_;

    detail::SceneNodePool<T>* pool_;

    void detachFromParent();

    friend class LargeSceneNode<T>;
};
}  // namespace dw

#include "core/scene/SceneNode.i.h"
