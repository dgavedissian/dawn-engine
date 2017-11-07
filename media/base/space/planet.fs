#version 330 core

in vec3 out_normal;
in vec2 out_texcoord0;

layout(location = 0) out vec4 out_colour;

uniform vec3 light_direction;
uniform sampler2D surface_sampler;

void main()
{
    vec4 surface = texture(surface_sampler, out_texcoord0);
    out_colour = clamp(dot(out_normal, light_direction), 0.0, 1.0) * surface;
}