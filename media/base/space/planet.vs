#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord0;

uniform mat4 model_matrix;
uniform mat4 mvp_matrix;

out vec3 out_normal;
out vec2 out_texcoord0;

void main()
{
    out_normal = (model_matrix * vec4(normal, 0.0)).xyz;
    out_texcoord0 = texcoord0;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}