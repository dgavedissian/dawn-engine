/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Node.h"

namespace dw {
class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph();

    Frame* addFrame(SystemNode* frame_node);
    void removeFrame(Frame* frame);
    Frame* frame(int i);
    int frameCount() const;

    // Root system node.
    SystemNode& root();

    // Root scene node for objects attached to _all_ cameras.
    Node& backgroundNode();

private:
    detail::SceneNodePool pool_;
    SystemNode root_;
    Node background_root_;

    Vector<UniquePtr<Frame>> frames_;
};
}  // namespace dw
