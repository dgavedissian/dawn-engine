#version 330 core

uniform mat4 worldViewProjMatrix;

in vec4 vertex;
in vec2 uv0;

out vec2 oUv;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    oUv = uv0;
}
