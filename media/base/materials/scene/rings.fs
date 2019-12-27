#version 330 core

uniform vec3 ring_dimensions;
uniform float planet_radius;
uniform vec3 planet_position;
uniform float planet_penumbra_recip;
uniform vec3 atmosphere_colour;
uniform vec4 light_to_planet;
uniform vec3 light_position;
uniform vec3 camera_position;
uniform vec2 distance_threshold;
uniform float znear;
uniform float zfar;

uniform sampler2D rings_texture;
uniform sampler2D noise_texture;

in float depth;
in vec3 world_position;
in vec2 noise_tex_coord;

out vec4 colour;


// x = percentage between a and c
// x == 0.4
// a       b        c
// |----x--|--------|------------------
// 0       0.5      1

// mix(a, b, x)

// if x < 0.5: mix(a, b, x * 2)
// if x > 0.5: mix(b, c, x * 2 - 1)

float mix3(float a, float b, float c, float x) {
    if (x < 0.5) {
        return mix(a, b, x * 2);
    } else {
        return mix(b, c, x * 2 - 1);
    }
}

void main()
{
    // Ensure that the circular shape of the ring is perfect
    float distance = length(world_position - planet_position);
    if (distance < ring_dimensions.x || distance > ring_dimensions.y) {
        discard;
    }

    // Apply fading out of the ring bands based on distance of the fragment
    // from the camera
    float dist_to_px = length(world_position - camera_position);
    if (dist_to_px < distance_threshold.x) {
        discard;
    }

    // Fade to the noise texture
    float alpha = 1.0;
    float noise_sample = texture(noise_texture, noise_tex_coord).r;
    float power = 4.0;
    float noise_factor = pow(2.0, power) * clamp((dist_to_px - distance_threshold.x) / (distance_threshold.y - distance_threshold.x), 0.0, 1.0);
    alpha = clamp(noise_sample + pow(noise_factor, 1.0 / power) - 1.0, 0.0, 1.0);

    // Calculate shadow factor
    vec3 shadow_factor = vec3(1.0);
    vec3 px_to_light = world_position - light_position;
    float len_px_to_light = length(px_to_light);

    // Only check pen/umbra if this pixel is on the planet's far side
    if (len_px_to_light > light_to_planet.w)
    {
        float length_ratio = len_px_to_light / light_to_planet.w;
        vec3 new_planet_pos = light_position + light_to_planet.xyz * length_ratio;
        vec3 new_planet_to_px = new_planet_pos - world_position;
        float len_new_planet_to_px = length(new_planet_to_px);

        float planet_over = clamp((planet_radius - len_new_planet_to_px) * planet_penumbra_recip, 0.0, 1.0);
        float atmosphere_over = clamp((planet_radius - len_new_planet_to_px) * planet_penumbra_recip, 0.0, 1.0);
        shadow_factor *= mix(vec3(1.0), vec3(0.0), planet_over);
        shadow_factor *= mix(vec3(1.0), atmosphere_colour, atmosphere_over);
    }

    // Calculate texture coordinates and sample the texture then apply shadowing
    vec4 rings_sample = texture(rings_texture, vec2((distance - ring_dimensions.x) / ring_dimensions.z, 0.5));
    rings_sample.rgb *= shadow_factor;
    rings_sample.a *= alpha;
    colour = rings_sample;
    //colour = vec4(alpha, 0.0, 0.0, 1.0);

    // Logarithmic Z-depth
    //gl_FragDepth = log(depth / znear) / log(zfar / znear);
}
