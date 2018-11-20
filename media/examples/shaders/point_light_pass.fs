#version 330 core

layout(location = 0) out vec4 out_colour;

/*
GBUFFER LAYOUT:
gb0: |diffuse.rgb|X|
gb1: |position.xyz|X|
gb2: |normal.xyz|X|
*/
uniform sampler2D gb0_sampler;
uniform sampler2D gb1_sampler;
uniform sampler2D gb2_sampler;
uniform vec2 screen_size;

uniform vec3 light_position;

uniform float radius;

void main()
{
    vec2 screen_coord = gl_FragCoord.xy / screen_size;

    vec3 diffuse_colour = texture(gb0_sampler, screen_coord).rgb;
    vec3 pixel_position = texture(gb1_sampler, screen_coord).rgb;
    vec3 pixel_normal = normalize(texture(gb2_sampler, screen_coord).rgb);

    // Calculate shading. Source: https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
    vec3 light_direction = light_position - pixel_position;
    float distance = length(light_direction);
    light_direction /= distance;

    // Calculate attenuation.
    float denom = distance / radius + 1.0f;
    float attenuation = 1.0f / (denom * denom);
    float dot = max(dot(light_direction, pixel_normal), 0.0);

    vec3 shaded_result = diffuse_colour * dot * attenuation;
    out_colour = vec4(shaded_result, 1.0);
}