/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Renderable.h"
#include "renderer/Material.h"

namespace dw {
class DW_API Node : public Object {
public:
    DW_OBJECT(Node);

    Node(Context* context);
    ~Node();

    void SetRenderable(SharedPtr<Renderable> renderable);

    Renderable* GetRenderable() const;

private:
    SharedPtr<Renderable> renderable_;
};
}
