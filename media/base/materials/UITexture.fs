#version 330 core

uniform sampler2D uiTexture;

in vec4 oColour;
in vec2 oUv0;

out vec4 colour;

void main()
{
    colour = texture(uiTexture, oUv0) * oColour;
}
