/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

// Vertex Declaration.
class VertexDecl {
public:
    enum class Attribute {
        Position,
        Normal
    };

    enum class AttributeType {
        Float,
        Uint8
    };

    VertexDecl();
    ~VertexDecl() = default;

    VertexDecl& begin();
    VertexDecl& add(Attribute attribute, uint count, AttributeType type);
    VertexDecl & end();

private:
    void decodeAttributes(u32 encoded_attribute, Attribute& attribute, uint& count, AttributeType& type);
    // Attribute: 15
    // Count: 2
    // AttributeType: 15
    Vector<u32> attributes_;

    friend class Renderer;
};

} // namespace dw
