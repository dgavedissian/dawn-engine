#version 330 core

uniform mat4 worldViewProj;

in vec4 vertex;
in vec4 colour;
in vec2 uv0;

out vec4 oColour;
out vec2 oUv0;

void main()
{
    gl_Position = worldViewProj * vertex;
    oColour = colour;
    oUv0 = uv0;
}
