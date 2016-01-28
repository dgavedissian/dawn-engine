#version 330 core

uniform sampler2D galacticPlaneTexture;
uniform sampler2D nebulaTexture;

in vec2 oUv;

out vec4 colour;

void main()
{
    vec4 galacticPlane = texture2D(galacticPlaneTexture, oUv);
    vec4 nebula = texture2D(nebulaTexture, oUv);
    colour = vec4(mix(galacticPlane.rgb, nebula.rgb, nebula.a), 1.0);
}
