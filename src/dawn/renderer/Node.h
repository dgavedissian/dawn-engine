/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class Node : public Object {
public:
    DW_OBJECT(Node);

    Node(Context* context);
    ~Node();

    void setMesh(Mesh* mesh);
    void setMaterial(Material* material);

    Mesh* getMesh();
    Material* getMaterial();

private:
    Mesh* mMesh;
    Material* mMaterial;
};
}