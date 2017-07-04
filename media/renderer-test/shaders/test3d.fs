#version 330

in vec3 Normal;
in vec3 Color;

layout(location = 0) out vec4 outColor;

uniform vec3 lightDirection;

void main()
{
    outColor = vec4(clamp(dot(Normal, lightDirection), 0.0, 1.0) * Color, 1.0);
}