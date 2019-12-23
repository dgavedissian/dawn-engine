#version 330 core
#define ENABLE_NORMAL_MAP
in vec2 out_texcoord;
in mat3 out_tbn;
in vec3 out_normal;

layout(location = 0) out vec4 out_colour;

uniform sampler2D surface_map;
#ifdef ENABLE_NORMAL_MAP
uniform sampler2D normal_map;
#endif
uniform vec3 light_dir;

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
    float lighting = clamp(dot(normal, light_dir), 0.0, 1.0);

    // Set output colour.
    vec4 surface = texture(surface_map, out_texcoord);
    // out_colour = vec4((normal + 1.0) / 2.0, 1.0);
    out_colour = lighting * surface;
}
