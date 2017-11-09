/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "net/NetData.h"

namespace dw {
NetData::NetData(const Vector<ReplicatedPropertyPtr> properties) : properties_{properties} {
}

void NetData::onAddToEntity(Entity *parent) {
    for (auto &prop : properties_) {
        prop->onAddToEntity(*parent);
    }
}

void NetData::serialise(OutputStream &out) {
    for (auto &prop : properties_) {
        prop->serialise(out);
    }
}

void NetData::deserialise(InputStream &in) {
    for (auto &prop : properties_) {
        prop->deserialise(in);
    }
}
}