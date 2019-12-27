#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;

uniform mat4 model_matrix;
uniform mat4 mvp_matrix;

// Atmosphere parameters.
uniform vec3 inv_wavelength;
uniform vec4 radius;
uniform vec4 kr_km;
uniform vec3 atmos_scale;
uniform vec3 sun_direction;
uniform vec3 camera_position;
uniform vec2 camera_height;

out vec2 out_texcoord;
out mat3 out_tbn;
out vec3 out_atmosphere_colour;

float funcScale(float atmos_scale_depth, float cosine)
{
    float x = 1.0 - cosine;
    return atmos_scale_depth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main()
{
    // Compute TBN matrix.
    vec3 t = normalize((model_matrix * vec4(tangent, 0.0)).xyz);
    vec3 n = normalize((model_matrix * vec4(normal, 0.0)).xyz);
    vec3 b = cross(n, t);
    mat3 tbn = mat3(t, b, n);
    out_tbn = tbn;

    // Transform position.
    out_texcoord = texcoord;
    gl_Position = mvp_matrix * vec4(position, 1.0);

    /////////////////////////////
    // ATMOSPHERE CODE
    /////////////////////////////
#ifdef ENABLE_ATMOSPHERE
    // Get the ray from the camera to the vertex, and its length.
    vec3 pos = position.xyz;
    vec3 ray = pos - camera_position;
    float far = length(ray);
    ray /= far;
    vec3 start;
    float depth;
    if (camera_height.x < radius.z)
    {
        start = camera_position;
        depth = exp((radius.x - camera_height.x) / atmos_scale.y);
    }
    else
    {
        // Calculate the closest intersection of the ray with the outer atmosphere
        float B = 2.0 * dot(camera_position, ray);
        float C = camera_height.y - radius.w;
        float det = max(0.0, B * B - 4.0 * C);
        float near = 0.5 * (-B - sqrt(det));

        // Calculate the ray's starting position, then calculate it's scattering offset
        start = camera_position + ray * near;
        far -= near;
        depth = exp((radius.x - radius.z) / atmos_scale.y);
    }

    float camera_angle = dot(-ray, pos) / length(pos);
    float light_angle = dot(sun_direction, pos) / length(pos);
    float camera_scale = funcScale(atmos_scale.y, camera_angle);
    float light_scale = funcScale(atmos_scale.y, light_angle);
    float camera_offset = depth * camera_scale;
    float temp = light_scale + camera_scale;

    // Initialise the scattering loop variables
    int num_samples = 4;
    float samples = float(num_samples);
    float sample_length = far / samples;
    float atmos_scaled_length = sample_length * atmos_scale.x;
    vec3 sample_ray = ray * sample_length;
    vec3 sample_point = start + sample_ray * 0.5;

    // Loop through the sample rays
    vec3 front_clour = vec3(0.0);
    vec3 attenuate;
    for (int i = 0; i < num_samples; i++)
    {
        float height = length(sample_point);
        float depth = exp(atmos_scale.z * (radius.x - height));
        float scatter = depth * temp - camera_offset;
        attenuate = exp(-scatter * (inv_wavelength * kr_km.y + kr_km.w));
        front_clour += attenuate * (depth * atmos_scaled_length);
        sample_point += sample_ray;
    }

    // Finally, scale the Mie and Rayleigh colours
    out_atmosphere_colour = front_clour * (inv_wavelength * kr_km.x + kr_km.z);
    out_atmosphere_colour.r = pow(out_atmosphere_colour.r, 2.2f);
    out_atmosphere_colour.g = pow(out_atmosphere_colour.g, 2.2f);
    out_atmosphere_colour.b = pow(out_atmosphere_colour.b, 2.2f);
#else
    out_atmosphere_colour = vec3(0.0);
#endif
}