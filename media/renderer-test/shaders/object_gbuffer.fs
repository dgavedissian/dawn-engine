#version 330 core

in vec3 WorldPosition;
in vec2 TexCoord;
in vec3 Normal;
in vec3 Color;

/*
GBUFFER LAYOUT:
gb0: |diffuse.rgb|X|
gb1: |position.xyz|X|
gb2: |normal.xyz|X|
*/
layout(location = 0) out vec4 gb0;
layout(location = 1) out vec4 gb1;
layout(location = 2) out vec4 gb2;

uniform vec3 light_direction;
uniform vec2 texcoord_scale;

uniform sampler2D wall_sampler;

void main()
{
    vec4 diffuse = texture(wall_sampler, TexCoord * texcoord_scale);
    gb0.rgb = diffuse.rgb;
    gb1.rgb = WorldPosition;
    gb2.rgb = normalize(Normal);
}