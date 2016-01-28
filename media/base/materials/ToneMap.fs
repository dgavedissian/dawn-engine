#version 330 core

uniform sampler2D texture;
uniform float exposureLevel;

in vec2 oUv;

out vec4 colour;

void main()
{
    colour = texture2D(texture, oUv);
    vec2 mid = oUv - vec2(0.5);
    float vignette = 1.0 - dot(mid, mid);
    colour *= pow(vignette, 4.0);
    colour *= exposureLevel;
}
