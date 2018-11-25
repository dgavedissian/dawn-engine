#version 330 core

in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D gb0_sampler;
uniform sampler2D gb1_sampler;
uniform sampler2D gb2_sampler;

void main()
{
    vec4 gb0 = texture(gb0_sampler, TexCoord);
    vec4 gb1 = texture(gb1_sampler, TexCoord);
    vec4 gb2 = texture(gb2_sampler, TexCoord);

    if (TexCoord.x < 0.5 && TexCoord.y >= 0.5) {
        outColor = texture(gb0_sampler, (TexCoord - vec2(0.0, 0.5)) * 2.0);
    } else if (TexCoord.x >= 0.5 && TexCoord.y >= 0.5) {
        outColor = texture(gb1_sampler, (TexCoord - vec2(0.5, 0.5)) * 2.0);
    } else if (TexCoord.x < 0.5 && TexCoord.y < 0.5) {
        outColor = texture(gb2_sampler, (TexCoord - vec2(0.0, 0.0)) * 2.0);
    } else {
        outColor = vec4(0, 0, 0, 0);
    }
}
