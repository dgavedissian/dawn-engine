#version 330 core

in vec2 out_texcoord;
in mat3 out_tbn;
in vec3 out_normal;
in vec3 out_atmosphere_colour;

layout(location = 0) out vec4 out_colour;

uniform sampler2D surface_map;
#ifdef ENABLE_NORMAL_MAP
uniform sampler2D normal_map;
#endif
uniform vec3 sun_direction;

void main()
{
#ifdef ENABLE_NORMAL_MAP
    // Obtain normal (in tangent space).
    vec3 normal = texture(normal_map, out_texcoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(out_tbn * normal);
#else
   vec3 normal = out_tbn[2];
#endif

    // Compute lighting.
    float n_dot_l = dot(out_tbn[2], sun_direction);
    float lighting = clamp(dot(normal, sun_direction), 0.0, 1.0);
    // If n_dot_l is negative, this means we're on the night side. Due to the shadow of the planetary sphere, normal maps
    // should not cause features to be lit on the night side. We emulate this shadow by creating a smooth terminator
    // line then mixing the calculated lighting with 0 by that factor.
    float terminator_factor = pow(clamp(-n_dot_l * 4.0, 0.0, 1.0), 1.0 / 2.0);
    lighting = mix(lighting, 0.0, terminator_factor);

#ifdef ENABLE_ATMOSPHERE
    vec4 atmosphere_factor = vec4(out_atmosphere_colour, 0.0);
#else
    vec4 atmosphere_factor = vec4(0.0);
#endif

    // Set output colour.
    vec4 surface = texture(surface_map, out_texcoord);
    // out_colour = vec4((normal + 1.0) / 2.0, 1.0);
    out_colour = lighting * surface + atmosphere_factor;
}
