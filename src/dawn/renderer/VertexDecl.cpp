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
                            VertexDecl::AttributeType type, bool normalised) {
    attributes_.emplace_back(makePair(encodeAttributes(attribute, count, type, normalised),
                                      reinterpret_cast<void*>(static_cast<uintptr_t>(stride_))));
    stride_ += count * attributeTypeSize(type);
    return *this;
}

VertexDecl& VertexDecl::end() {
    return *this;
}

u16 VertexDecl::encodeAttributes(VertexDecl::Attribute attribute, uint count,
                                 VertexDecl::AttributeType type, bool normalised) {
    // Attribute: 7
    // Count: 3
    // AttributeType: 5
    // Normalised: 1
    return static_cast<u16>((static_cast<u16>(attribute) << 9) |
                            ((static_cast<u16>(count) & 0x7) << 6) |
                            ((static_cast<u16>(type) & 0x1F) << 1) | (normalised ? 1 : 0));
}

void VertexDecl::decodeAttributes(u16 encoded_attribute, Attribute& attribute, uint& count,
                                  AttributeType& type, bool& normalised) {
    // Attribute: 7
    // Count: 3
    // AttributeType: 5
    // Normalised: 1
    attribute = static_cast<Attribute>(encoded_attribute >> 9);
    count = static_cast<uint>((encoded_attribute >> 6) & 0x7);
    type = static_cast<AttributeType>((encoded_attribute >> 1) & 0x1F);
    normalised = (encoded_attribute & 0x1) == 1;
}

uint VertexDecl::attributeTypeSize(AttributeType type) {
    switch (type) {
        case AttributeType::Uint8:
            return 1;
        case AttributeType::Float:
            return 4;
        default:
            assert(false);
    }
}
}  // namespace dw
   // namespace dw