#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

out vec2 o_texcoord;

void main()
{
    o_texcoord = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}