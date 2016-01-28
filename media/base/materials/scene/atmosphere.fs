#version 330 core

uniform vec3 sunDirection;
uniform vec2 g;

in vec3 mie;
in vec3 rayleigh;
in vec3 direction;

out vec4 colour;

void main()
{
    float cosine = dot(sunDirection, direction) / length(direction);
    float miePhase = 1.5 * ((1.0 - g.y) / (2.0 + g.y)) * (1.0 + cosine * cosine) / pow(1.0 + g.y - 2.0 * g.x * cosine, 1.5);
    float rayleighPhase = 0.75 * (1.0 + cosine * cosine);

    colour.rgb = rayleighPhase * rayleigh + miePhase * mie;
    colour.a = max(colour.b, colour.r);
}
