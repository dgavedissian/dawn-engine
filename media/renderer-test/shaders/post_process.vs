#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord0;

out vec2 TexCoord;

void main()
{
    TexCoord = texcoord0;
    gl_Position = vec4(position, 0.0, 1.0);
}