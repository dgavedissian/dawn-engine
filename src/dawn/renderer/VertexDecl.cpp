/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexDecl.h"

namespace dw {
dw::VertexDecl::VertexDecl() {

}

VertexDecl &VertexDecl::begin() {
    return *this;
}

VertexDecl &VertexDecl::add(VertexDecl::Attribute attribute, uint count, VertexDecl::AttributeType type) {
    attributes_.emplace_back(
            static_cast<u32>(attribute) << 17 |
            (static_cast<u32>(count) & 0x3) << 15 |
            (static_cast<u32>(type) & 0x7FFF)
    );
    return *this;
}

VertexDecl &VertexDecl::end() {
    return *this;
}

void VertexDecl::decodeAttributes(u32 encoded_attribute, Attribute& attribute, uint& count, AttributeType& type) {
    // Attribute: 15
    // Count: 2
    // AttributeType: 15
    attribute = static_cast<Attribute>(encoded_attribute >> 17);
    count = static_cast<uint>((encoded_attribute >> 15) & 0x3);
    type = static_cast<AttributeType>(encoded_attribute & 0x7FFF);
}
}