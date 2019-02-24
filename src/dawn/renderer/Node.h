/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "core/math/Defs.h"
#include "renderer/SystemPosition.h"

namespace dw {
class Node;
class SystemNode;
class Frame;

class DW_API Renderable;

namespace detail {
class SceneNodePool {
public:
    SystemNode* newSystemNode(const SystemPosition& p, const Quat& o);
    Node* newNode(Frame* frame, const Vec3& p, const Quat& o, const Vec3& s);

    void free(SystemNode* node);
    void free(Node* node);

    template <typename T> class Deleter {
    public:
        Deleter(SceneNodePool* pool) : pool_(pool) {
        }

        void operator()(T* p) const noexcept {
            pool_->free(p);
        }

    private:
        SceneNodePool* pool_;
    };
};

struct Transform {
    Vec3 position;
    Quat orientation;
    Vec3 scale;

    Transform(Vec3 p, Quat o, Vec3 s);

    Mat4 toMat4() const;
    static Transform fromMat4(const Mat4& matrix);
};

struct RendererSceneNodeData {
    SharedPtr<Renderable> renderable;
};
}  // namespace detail

class SystemNode {
public:
    SystemNode(detail::SceneNodePool* pool, const SystemPosition& p, const Quat& o);

    /// Note: Model matrices only make sense if they're relative to a position.
    Mat4 calculateModelMatrix(const SystemPosition& camera_position) const;

    /// Note: View matrices are always based on the origin.
    Mat4 calculateViewMatrix() const;

    SystemNode* parent() const;

    SystemNode* addChild(SystemNode* child);
    SystemNode* newChild(const SystemPosition& p = SystemPosition::origin,
                         const Quat& o = Quat::identity);
    SystemNode* child(int i);
    usize childCount() const;

    // Transform.
    SystemPosition position;
    Quat orientation;

    // Container data.
    detail::RendererSceneNodeData data;

private:
    SystemNode* parent_;
    Vector<SystemNode*> children_;
    byte depth_;

    detail::SceneNodePool* pool_;

    void detachFromParent();

    friend class Node;
    friend class Frame;
};

class Node {
public:
    Node(detail::SceneNodePool* pool, Frame* frame, const Vec3& p, const Quat& o, const Vec3& s);

    Mat4 calculateModelMatrix() const;
    Mat4 deriveWorldModelMatrix() const;

    Frame* frame() const;
    Node* parent() const;

    Node* addChild(Node* child);
    Node* newChild(const Vec3& p = Vec3::zero, const Quat& o = Quat::identity,
                   const Vec3& s = Vec3::one);
    Node* child(int i);
    usize childCount() const;

    // Transform.
    detail::Transform& transform();
    const detail::Transform& transform() const;
    bool dirty;

    // Container data.
    detail::RendererSceneNodeData data;

private:
    detail::Transform transform_;
    Node* parent_;
    Frame* frame_;
    Vector<Node*> children_;
    byte depth_;

    detail::SceneNodePool* pool_;

    void detachFromParent();

    friend class SystemNode;
};

class Frame {
public:
    Frame(SystemNode* system_node);

    Node* addChild(Node* child);
    Node* newChild(const Vec3& p = Vec3::zero, const Quat& o = Quat::identity,
                   const Vec3& s = Vec3::one);
    Node* child(int i);
    int childCount() const;

    void setFollowed(Node* followed);

    const SystemPosition& position() const;

private:
    detail::SceneNodePool* pool_;
    SystemNode* system_node_;
    UniquePtr<Node, detail::SceneNodePool::Deleter<Node>> root_frame_node_;
    Node* followed_;

    friend class SceneGraph;
};

}  // namespace dw
