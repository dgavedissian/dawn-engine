#version 330 core

uniform mat4 worldViewProjMatrix;
uniform float znear;
uniform float zfar;

in vec4 vertex;
in vec4 colour;
in vec2 uv0;

out vec4 oColour;
out vec2 oUv0;

void main()
{
	gl_Position = worldViewProjMatrix * vertex;
	gl_Position.z = 2.0 * log(gl_Position.w / znear) / log(zfar / znear) - 1.0;
	gl_Position.z *= gl_Position.w;

    oColour = colour;
    oUv0 = uv0;
}
