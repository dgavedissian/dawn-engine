#version 330 core

layout(location = 0) out vec4 outColor;

/*
GBUFFER LAYOUT:
gb0: |diffuse.rgb|X|
gb1: |position.xyz|X|
gb2: |normal.xyz|X|
*/
uniform sampler2D gb0;
uniform sampler2D gb1;
uniform sampler2D gb2;
uniform vec2 screen_size;

uniform vec3 light_position;

uniform float radius;

vec2 calcScreenCoord()
{
    return gl_FragCoord.xy / screen_size;
}

void main()
{
    vec2 screenCoord = calcScreenCoord();

    vec3 colour = texture(gb0, screenCoord).rgb;
    vec3 position = texture(gb1, screenCoord).rgb;
    vec3 normal = normalize(texture(gb2, screenCoord).rgb);

    // Calculate shading. Source: https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
    vec3 light_direction = light_position - position;
    float distance = length(light_direction);
    light_direction /= distance;
    float denom = distance / radius + 1.0f;
    float attenuation = 1.0f / (denom * denom);
    float dot = max(dot(light_direction, normal), 0.0);

    vec3 shaded_result = colour * dot * attenuation;
    outColor = vec4(shaded_result, 1.0);
}