/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/Material.h"
#include "renderer/Mesh.h"
#include "renderer/Node.h"

namespace dw {

Node::Node(Context* context) : Object(context) {
}

Node::~Node() {
}

void Node::setMesh(Mesh* mesh) {
    mMesh = mesh;
}

void Node::setMaterial(Material* material) {
    mMaterial = material;
}

Mesh* Node::getMesh() {
    return mMesh;
}

Material* Node::getMaterial() {
    return mMaterial;
}
}
