/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#include "Common.h"
#include "renderer/VertexDecl.h"

namespace dw {
dw::VertexDecl::VertexDecl() : stride_{0} {
}

VertexDecl& VertexDecl::begin() {
    return *this;
}

VertexDecl& VertexDecl::add(VertexDecl::Attribute attribute, uint count,
                            VertexDecl::AttributeType type) {
    attributes_.emplace_back(makePair(encodeAttributes(attribute, count, type),
                                      reinterpret_cast<void*>(static_cast<uintptr_t>(stride_))));
    stride_ += count * getAttributeTypeSize(type);
    return *this;
}

VertexDecl& VertexDecl::end() {
    return *this;
}

u16 VertexDecl::encodeAttributes(VertexDecl::Attribute attribute, uint count,
                                 VertexDecl::AttributeType type) {
    // Attribute: 7
    // Count: 3
    // AttributeType: 6
    return static_cast<u16>(static_cast<u16>(attribute) << 9 |
                            (static_cast<u16>(count) & 0x7) << 6 | (static_cast<u16>(type) & 0x7F));
}

void VertexDecl::decodeAttributes(u16 encoded_attribute, Attribute& attribute, uint& count,
                                  AttributeType& type) {
    // Attribute: 7
    // Count: 3
    // AttributeType: 6
    attribute = static_cast<Attribute>(encoded_attribute >> 9);
    count = static_cast<uint>((encoded_attribute >> 6) & 0x7);
    type = static_cast<AttributeType>(encoded_attribute & 0x7F);
}

uint VertexDecl::getAttributeTypeSize(AttributeType type) {
    switch (type) {
        case AttributeType::Uint8:
            return 1;
        case AttributeType::Float:
            return 4;
    }
}
}  // namespace dw
   // namespace dw