/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

// Vertex Declaration.
class VertexDecl {
public:
    enum class Attribute { Position, Normal, Colour, TexCoord0 };

    enum class AttributeType { Float, Uint8 };

    VertexDecl();
    ~VertexDecl() = default;

    VertexDecl& begin();
    VertexDecl& add(Attribute attribute, uint count, AttributeType type);
    VertexDecl& end();

private:
    static u16 encodeAttributes(Attribute attribute, uint count, AttributeType type);
    static void decodeAttributes(u16 encoded_attribute, Attribute& attribute, uint& count,
                                 AttributeType& type);
    static uint getAttributeTypeSize(AttributeType type);
    // Attribute: 7
    // Count: 3
    // AttributeType: 6
    Vector<Pair<u16, void*>> attributes_;
    u16 stride_;

    friend class Renderer;
};

}  // namespace dw
