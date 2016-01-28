#version 330 core

uniform float farDistance;

in vec3 oViewPos;
out vec4 colour;

void main()
{
    float depth = length(oViewPos) / farDistance;
    colour = vec4(depth, depth, depth, 1.0);
}
