#version 330 core

uniform vec3 sun_direction;
uniform vec2 g;

in vec3 out_mie;
in vec3 out_rayleigh;
in vec3 out_direction;

out vec4 colour;

void main()
{
    float cosine = dot(sun_direction, out_direction) / length(out_direction);
    float mie_phase = 1.5 * ((1.0 - g.y) / (2.0 + g.y)) * (1.0 + cosine * cosine) / pow(1.0 + g.y - 2.0 * g.x * cosine, 1.5);
    float rayleigh_phase = 0.75 * (1.0 + cosine * cosine);

    colour.rgb = rayleigh_phase * out_rayleigh + mie_phase * out_mie;
    colour.a = max(colour.b, colour.r);
}
