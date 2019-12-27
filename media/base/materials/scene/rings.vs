#version 330 core

uniform mat4 mvp_matrix;
uniform mat4 model_matrix;
uniform vec3 ring_dimensions;

layout(location = 0) in vec4 vertex;

out float depth;
out vec3 world_position;
out vec2 noise_tex_coord;

void main()
{
    vec4 position = mvp_matrix * vertex;
    gl_Position = position;
    depth = position.z;

    world_position = vec3(model_matrix * vertex);
    noise_tex_coord = (vec2(vertex.x, vertex.z) / (2.0 * ring_dimensions.y) + vec2(0.5, 0.5)) * 16000.0;
}
