#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec3 Color;

void main()
{
    Color = color.rgb;
    gl_Position = vec4(position, 0.0, 1.0);
}