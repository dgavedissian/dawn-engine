/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Renderable.h"
#include "renderer/Node.h"

namespace dw {
Node::Node(Context* context) : Object(context) {
}

Node::~Node() {
}

void Node::SetRenderable(SharedPtr<Renderable> geometry) {
    renderable_ = geometry;
}

Renderable* Node::GetRenderable() const {
    return renderable_.get();
}
}
