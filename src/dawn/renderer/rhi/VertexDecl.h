/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2018 (git@dga.me.uk)
 */
#pragma once

namespace dw {
namespace rhi {
// Vertex Declaration.
class DW_API VertexDecl {
public:
    enum class Attribute { Position, Normal, Colour, TexCoord0 };

    enum class AttributeType { Float, Uint8 };

    VertexDecl();
    ~VertexDecl() = default;

    VertexDecl& begin();
    VertexDecl& add(Attribute attribute, uint count, AttributeType type, bool normalised = false);
    VertexDecl& end();

    u16 stride() const;

    bool empty() const;

private:
    static u16 encodeAttributes(Attribute attribute, uint count, AttributeType type,
                                bool normalised);
    static void decodeAttributes(u16 encoded_attribute, Attribute& attribute, uint& count,
                                 AttributeType& type, bool& normalised);
    static u16 attributeTypeSize(AttributeType type);

    // Attribute: 7
    // Count: 3
    // AttributeType: 5
    // Normalised: 1
    Vector<Pair<u16, byte*>> attributes_;
    u16 stride_;

    friend class RHIRenderer;
    friend class GLRenderContext;
};
}  // namespace rhi
}  // namespace dw
