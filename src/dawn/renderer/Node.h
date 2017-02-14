/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Geometry.h"
#include "renderer/Material.h"

namespace dw {
class DW_API Node : public Object {
public:
    DW_OBJECT(Node);

    Node(Context* context);
    ~Node();

    void setGeometry(SharedPtr<Geometry> geometry);
    void setMaterial(SharedPtr<Material> material);

    Geometry* getGeometry();
    Material* getMaterial();

private:
    SharedPtr<Geometry> mGeometry;
    SharedPtr<Material> mMaterial;
};
}
