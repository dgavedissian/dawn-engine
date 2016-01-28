#version 330 core

uniform mat4 worldViewProjMatrix;

in vec4 vertex;

out vec4 oPos;
out vec2 oUv;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    oPos = gl_Position;
    oUv = gl_Position.xy;
}
