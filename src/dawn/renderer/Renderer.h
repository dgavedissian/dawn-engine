/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Node.h"

namespace dw {
class DW_API Renderer : public Object {
public:
    DW_OBJECT(Renderer);

    Renderer(Context* context);
    ~Renderer();

	Node* GetRootNode() const;

	/// @brief Render a single frame.
    void Frame();

private:
    uint width_, height_;

    // Scene graph.
    SharedPtr<Node> root_node_;

    // O(1) add, O(N) removal. Optimised for iteration speed.
    Vector<Renderable*> render_queue_;
    friend class Renderable;
    void AddToRenderQueue(Renderable* renderable);
    void RemoveFromRenderQueue(Renderable* renderable);
};
}
