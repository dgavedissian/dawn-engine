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

void Node::setGeometry(SharedPtr<Geometry> geometry) {
    mGeometry = geometry;
}

void Node::setMaterial(SharedPtr<Material> material) {
    mMaterial = material;
}

Geometry* Node::getGeometry() {
    return mGeometry.get();
}

Material* Node::getMaterial() {
    return mMaterial.get();
}
}
