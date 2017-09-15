/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"

namespace dw {
class DW_API Node : public Object {
public:
    DW_OBJECT(Node);

    Node(Context* context);
    ~Node();

    void setRenderable(SharedPtr<Renderable> renderable);

    Renderable* renderable() const;

private:
    SharedPtr<Renderable> renderable_;
};
}  // namespace dw
