#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldViewMatrix;

in vec4 vertex;

out vec3 oViewPos;

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    oViewPos = (worldViewMatrix * vertex).xyz;
}
