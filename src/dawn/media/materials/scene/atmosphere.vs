#version 330 core

uniform mat4 worldViewProjMatrix;

uniform vec3 invWavelength;
uniform vec4 radius;
uniform vec4 KrKm;
uniform vec3 atmosScale;
uniform vec3 sunDirection;
uniform vec3 cameraPosition;
uniform vec2 cameraHeight;

in vec4 vertex;

out vec3 mie;
out vec3 rayleigh;
out vec3 direction;

float funcScale( float atmosScaleDepth, float cosine )
{
    float x = 1.0 - cosine;
    return atmosScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main()
{
    // Transform the vertex
    gl_Position = worldViewProjMatrix * vertex;

    // Get the ray from the camera to the vertex, and its length
    vec3 pos = vertex.xyz;
    vec3 ray = pos - cameraPosition;
    float far = length(ray);
    ray /= far;

    vec3 start;
    float startAngle;
    float startOffset;
    if (cameraHeight.x < radius.z)
    {
        // Calculate the ray's starting position, then calculate it's scattering offset
        start = cameraPosition;
        float height = length(start);
        float depth = exp(atmosScale.z * (radius.x - cameraHeight.x));
        startAngle = dot(ray, start) / height;
        startOffset = depth * funcScale(atmosScale.y, startAngle);
    }
    else
    {
        // Calculate the closest intersection of the ray with the outer atmosphere
        float B = 2.0 * dot(cameraPosition, ray);
        float C = cameraHeight.y - radius.w;
        float det = max(0.0, B * B - 4.0 * C);
        float near = 0.5 * (-B - sqrt(det));

        // Calculate the ray's starting position, then calculate it's scattering offset
        start = cameraPosition + ray * near;
        far -= near;
        float startDepth = exp(-1.0 / atmosScale.y);
        startAngle = dot(ray, start) / radius.z;
        startOffset = startDepth * funcScale(atmosScale.y, startAngle);
    }

    // Initialise the scattering loop variables
    int nSamples = 4;
    float fSamples = float(nSamples);
    float sampleLength = far / fSamples;
    float atmosScaledLength = sampleLength * atmosScale.x;
    vec3 sampleRay = ray * sampleLength;
    vec3 samplePoint = start + sampleRay * 0.5;

    // Loop through the sample rays
    vec3 frontColour = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < nSamples; i++)
    {
        float height = length(samplePoint);
        float depth = exp(atmosScale.z * (radius.x - height));
        float lightAngle = dot(sunDirection, samplePoint) / height;
        float cameraAngle = dot(ray, samplePoint) / height;
        float scatter = (startOffset + depth * (funcScale(atmosScale.y, lightAngle) - funcScale(atmosScale.y, cameraAngle)));
        vec3 attenuate = exp(-scatter * (invWavelength * KrKm.y + KrKm.w));
        frontColour += attenuate * (depth * atmosScaledLength);
        samplePoint += sampleRay;
    }

    // Finally, scale the Mie and Rayleigh colours
    mie = frontColour * KrKm.z;
    rayleigh = frontColour * (invWavelength * KrKm.x);
    direction = cameraPosition - pos;
}
