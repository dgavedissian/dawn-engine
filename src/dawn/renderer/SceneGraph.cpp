/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/SceneGraph.h"
#include "renderer/SystemPosition.h"

namespace dw {
SceneGraph::SceneGraph()
    : root_(&pool_, SystemPosition::origin, Quat::identity),
      background_root_(&pool_, nullptr, Vec3::zero, Quat::identity, Vec3::one) {
}

SceneGraph::~SceneGraph() {
}

Frame* SceneGraph::addFrame(SystemNode* frame_node) {
    frames_.emplace_back(makeUnique<Frame>(frame_node));
    return frames_.back().get();
}

void SceneGraph::removeFrame(Frame* frame) {
    std::remove_if(frames_.begin(), frames_.end(),
                   [frame](const UniquePtr<Frame>& element) { return frame == element.get(); });
}

Frame* SceneGraph::frame(int i) {
    return frames_[i].get();
}

int SceneGraph::frameCount() const {
    return frames_.size();
}

SystemNode& SceneGraph::root() {
    return root_;
}

Node& SceneGraph::backgroundNode() {
    return background_root_;
}

}  // namespace dw
