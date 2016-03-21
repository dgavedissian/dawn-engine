#version 330 core

uniform sampler2D gb0;
uniform sampler2D gb1;

in vec2 oUv;
out vec4 colour;

vec3 decodeNormal(vec2 enc)
{
    float scale = 1.7777;
    vec3 nn = vec3(enc * vec2(2.0 * scale, 2.0 * scale), 0.0) + vec3(-scale, -scale, 1.0);
    float g = 2.0 / dot(nn.xyz, nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g - 1.0;
    return n;
}

float decodeDepth(vec2 encodedDepth)
{
    return dot(encodedDepth, vec2(1.0, 1.0 / 2048.0));
}

void main()
{
    if (oUv.x < 0.5 && oUv.y < 0.5)
    {
        vec4 a0 = texture(gb0, oUv * 2.0);
        colour = vec4(a0.rgb, 1.0);
    }
    else if (oUv.x >= 0.5 && oUv.y < 0.5)
    {
        vec4 a1 = texture(gb1, (oUv - vec2(0.5, 0.0)) * 2.0);
        colour = vec4((decodeNormal(a1.rg) + 1.0) * 0.5, 1.0);
    }
    else if (oUv.x < 0.5 && oUv.y >= 0.5)
    {
        vec4 a1 = texture(gb1, (oUv - vec2(0.0, 0.5)) * 2.0);
        float depth = pow(decodeDepth(a1.ba), 0.2);
        colour = vec4(depth, depth, depth, 1.0);
    }
    else
    {
        colour = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
