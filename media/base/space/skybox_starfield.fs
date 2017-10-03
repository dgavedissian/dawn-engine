#version 330 core

in vec2 out_texcoord0;

layout(location = 0) out vec4 out_colour;

uniform sampler2D starfield_sampler;

void main()
{
    out_colour = texture(starfield_sampler, out_texcoord0);
}
