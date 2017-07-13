#version 330

in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D in_sampler;

void main()
{
    vec4 rt_in = texture(in_sampler, TexCoord);
    rt_in *= vec4(-1, -1, -1, 1);
    outColor = rt_in;
}