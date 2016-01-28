#version 330 core

uniform sampler2D texture;

in vec4 oColour;
in vec2 oUv0;

out vec4 colour;

void main()
{
    colour = texture2D(texture, oUv0) * oColour;
}
