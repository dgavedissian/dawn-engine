#version 330 core

uniform sampler2D galacticPlaneTexture;
uniform sampler2D nebulaTexture;

in vec2 out_texcoord0;

out vec4 colour;

void main()
{
    vec4 galacticPlane = texture(galacticPlaneTexture, out_texcoord0);
    vec4 nebula = texture(nebulaTexture, out_texcoord0);
    colour = vec4(mix(galacticPlane.rgb, nebula.rgb, nebula.a), 1.0);
}
