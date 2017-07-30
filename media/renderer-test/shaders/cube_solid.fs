#version 330 core

in vec3 Normal;
in vec3 Color;

layout(location = 0) out vec4 outColor;

uniform vec3 light_direction;

void main()
{
    outColor = clamp(dot(Normal, light_direction), 0.0, 1.0) * vec4(Color, 1.0);
}