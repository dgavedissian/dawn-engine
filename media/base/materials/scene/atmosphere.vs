#version 330 core

layout(location = 0) in vec4 vertex;

out vec3 out_mie;
out vec3 out_rayleigh;
out vec3 out_direction;

uniform mat4 mvp_matrix;

uniform vec3 inv_wavelength;
uniform vec4 radius;
uniform vec4 kr_km;
uniform vec3 atmos_scale;
uniform vec3 sun_direction;
uniform vec3 camera_position;
uniform vec2 camera_height;

float funcScale(float atmos_scale_depth, float cosine)
{
    float x = 1.0 - cosine;
    return atmos_scale_depth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main()
{
    // Transform the vertex
    gl_Position = mvp_matrix * vertex;

    // Get the ray from the camera to the vertex, and its length
    vec3 pos = vertex.xyz;
    vec3 ray = pos - camera_position;
    float far = length(ray);
    ray /= far;

    vec3 start;
    float startAngle;
    float startOffset;
    if (camera_height.x < radius.z)
    {
        // Calculate the ray's starting position, then calculate it's scattering offset
        start = camera_position;
        float height = length(start);
        float depth = exp(atmos_scale.z * (radius.x - camera_height.x));
        startAngle = dot(ray, start) / height;
        startOffset = depth * funcScale(atmos_scale.y, startAngle);
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
        float startDepth = exp(-1.0 / atmos_scale.y);
        startAngle = dot(ray, start) / radius.z;
        startOffset = startDepth * funcScale(atmos_scale.y, startAngle);
    }

    // Initialise the scattering loop variables
    int num_samples = 4;
    float samples = float(num_samples);
    float sample_length = far / samples;
    float atmos_scaled_length = sample_length * atmos_scale.x;
    vec3 sample_ray = ray * sample_length;
    vec3 sample_point = start + sample_ray * 0.5;

    // Loop through the sample rays
    vec3 front_colour = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < num_samples; i++)
    {
        float height = length(sample_point);
        float depth = exp(atmos_scale.z * (radius.x - height));
        float light_angle = dot(sun_direction, sample_point) / height;
        float camera_angle = dot(ray, sample_point) / height;
        float scatter = (startOffset + depth * (funcScale(atmos_scale.y, light_angle) - funcScale(atmos_scale.y, camera_angle)));
        vec3 attenuate = exp(-scatter * (inv_wavelength * kr_km.y + kr_km.w));
        front_colour += attenuate * (depth * atmos_scaled_length);
        sample_point += sample_ray;
    }

    // Finally, scale the Mie and Rayleigh colours
    out_mie = front_colour * kr_km.z;
    out_rayleigh = front_colour * (inv_wavelength * kr_km.x);
    out_direction = camera_position - pos;
}
