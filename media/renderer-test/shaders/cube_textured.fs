#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 Color;

layout(location = 0) out vec4 outColor;

uniform vec3 light_direction;

uniform sampler2D wall_sampler;

void main()
{
    vec4 diffuse = texture(wall_sampler, TexCoord);
    outColor = clamp(dot(Normal, light_direction), 0.0, 1.0) * vec4(Color, 1.0) * diffuse;
}