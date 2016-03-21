#version 330 core

uniform sampler2D gb1;

in vec2 oUv0;

out vec4 colour;

float decodeDepth(vec2 encodedDepth)
{
    return dot(encodedDepth, vec2(1.0, 1.0 / 2048.0));
}

void main()
{
    // Get depth
    vec4 depthTex = texture(gb1, oUv0);
    float depth = decodeDepth(depthTex.ba);
    if (depth < 1e-7)
        depth = 1.0;

    // Generate fog
    float fogStart = 0.0;
    float fogEnd = 0.002;
    float fogFactor = 1.0 - clamp(exp(-pow(depth * 1000.0, 2.0)), 0.0, 1.0);
    vec4 fogColour = vec4(0.08, 0.065, 0.03, 0.58);
    fogColour.a *= fogFactor;
    colour = fogColour;
}
