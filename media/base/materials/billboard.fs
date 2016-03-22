#version 330 core

uniform sampler2D billboardTex;

in vec4 oColour;
in vec2 oUv0;

out vec4 colour;

void main()
{
    colour = texture(billboardTex, oUv0) * oColour;
    colour.a = pow(colour.a, 2.2);
}
