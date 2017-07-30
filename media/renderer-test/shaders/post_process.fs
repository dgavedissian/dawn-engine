#version 330 core

in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D in_sampler;

void main()
{
    vec4 rt_in = texture(in_sampler, TexCoord);
    rt_in.xyz = vec3(1.0) - rt_in.xyz;
    outColor = rt_in;
}
