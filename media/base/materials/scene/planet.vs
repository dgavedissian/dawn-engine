#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;
uniform mat4 normalMatrix;
uniform float znear;
uniform float zfar;

uniform vec3 config;
uniform vec3 invWavelength;
uniform vec4 radius;
uniform vec4 KrKm;
uniform vec3 atmosScale;
uniform vec3 sunDirection;
uniform vec3 cameraPosition;
uniform vec2 cameraHeight;

in vec4 vertex;
in vec3 normal;
in vec2 uv0;

out vec3 worldPosition;
out vec3 atmosphereColour;
out vec3 oNormal;
out vec2 oUv0;

float funcScale(float atmosScaleDepth, float cosine)
{
    float x = 1.0 - cosine;
    return atmosScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main()
{
    gl_Position = worldViewProjMatrix * vertex;
    gl_Position.z = 2.0 * log(gl_Position.w / znear) / log(zfar / znear) - 1.0;
    gl_Position.z *= gl_Position.w;

    oUv0 = uv0;
    oNormal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
    worldPosition = vec3(worldMatrix * vertex);

    /////////////////////////////
    // ATMOSPHERE CODE
    /////////////////////////////
    bool hasAtmosphere = config.y > 0.5;
    if (hasAtmosphere)
    {
        // Get the ray from the camera to the vertex, and its length.
        vec3 pos = vertex.xyz;
        vec3 ray = pos - cameraPosition;
        float far = length(ray);
        ray /= far;
        vec3 start;
        float depth;
        if (cameraHeight.x < radius.z)
        {
            start = cameraPosition;
            depth = exp((radius.x - cameraHeight.x) / atmosScale.y);
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
            depth = exp((radius.x - radius.z) / atmosScale.y);
        }

        float cameraAngle = dot(-ray, pos) / length(pos);
        float lightAngle = dot(sunDirection, pos) / length(pos);
        float cameraScale = funcScale(atmosScale.y, cameraAngle);
        float lightScale = funcScale(atmosScale.y, lightAngle);
        float cameraOffset = depth * cameraScale;
        float temp = lightScale + cameraScale;

        // Initialise the scattering loop variables
        int nSamples = 4;
        float fSamples = float(nSamples);
        float sampleLength = far / fSamples;
        float atmosScaledLength = sampleLength * atmosScale.x;
        vec3 sampleRay = ray * sampleLength;
        vec3 samplePoint = start + sampleRay * 0.5;

        // Loop through the sample rays
        vec3 frontColour = vec3(0.0);
        vec3 attenuate;
        for (int i = 0; i < nSamples; i++)
        {
            float height = length(samplePoint);
            float depth = exp(atmosScale.z * (radius.x - height));
            float scatter = depth * temp - cameraOffset;
            attenuate = exp(-scatter * (invWavelength * KrKm.y + KrKm.w));
            frontColour += attenuate * (depth * atmosScaledLength);
            samplePoint += sampleRay;
        }

        // Finally, scale the Mie and Rayleigh colours
        atmosphereColour = frontColour * (invWavelength * KrKm.x + KrKm.z);
        atmosphereColour.r = pow(atmosphereColour.r, 2.2f);
        atmosphereColour.g = pow(atmosphereColour.g, 2.2f);
        atmosphereColour.b = pow(atmosphereColour.b, 2.2f);
    }
    else
    {
        atmosphereColour = vec3(0.0);
    }
}
