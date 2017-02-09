/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "renderer/Geometry.h"
#include "renderer/Material.h"

namespace dw {

class Node : public Object {
public:
    DW_OBJECT(Node);

    Node(Context* context);
    ~Node();

    void setGeometry(Geometry* geometry);
    void setMaterial(Material* material);

    Geometry* getGeometry();
    Material* getMaterial();

private:
    Geometry* mGeometry;
    Material* mMaterial;
};
}
