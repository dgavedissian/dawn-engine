#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;

uniform mat4 model_matrix;
uniform mat4 mvp_matrix;

out vec2 out_texcoord;
out mat3 out_tbn;

void main()
{
    // Compute TBN matrix.
    vec3 t = normalize((model_matrix * vec4(tangent, 0.0)).xyz);
    vec3 n = normalize((model_matrix * vec4(normal, 0.0)).xyz);
    vec3 b = cross(n, t);
    mat3 tbn = mat3(t, b, n);
    out_tbn = tbn;

    // Transform position.
    out_texcoord = texcoord;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}