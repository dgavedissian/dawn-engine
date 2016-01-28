#version 330 core

uniform sampler2D starfieldTexture;

in vec2 oUv;

out vec4 colour;

void main()
{
    colour = texture2D(starfieldTexture, oUv * 4);
}
