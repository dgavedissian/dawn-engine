#version 330 core

uniform vec3 config;
uniform vec3 sunDirection;
uniform vec3 planetPosition;
uniform vec3 localSunDirection;
uniform vec3 ringDimensions;

uniform sampler2D surfaceTexture;
uniform sampler2D nightTexture;
uniform sampler2D ringsTexture;

in vec3 worldPosition;
in vec3 atmosphereColour;
in vec3 oNormal;
in vec2 oUv0;

out vec4 colour;

void main()
{
    // Calculate lighting factor the the surface
    float lighting = clamp(dot(sunDirection, oNormal), 0.0, 1.0);
    vec4 surfaceSample = texture(surfaceTexture, oUv0);

    // Set up config parameters (very hacky - must be a better way to do this!)
    bool hasNightSide = config.x > 0.5;
    bool hasAtmosphere = config.y > 0.5;
    bool hasRings = config.z > 0.5;

    // Combine with night lighting
    vec4 nightFactor = vec4(0.0);
    if (hasNightSide)
    {
        // night = (1 - day)^4
        float nightLighting = pow(1.0 - lighting, 4.0);
        vec4 nightSample = texture(nightTexture, oUv0);
        nightFactor = nightSample * nightLighting;
    }

    // Add atmosphere
    vec4 atmosphereFactor = vec4(0.0);
    if (hasAtmosphere)
        atmosphereFactor.rgb = atmosphereColour;

    // Calculate ring shadows
    if (hasRings)
    {
        float ringShadow = 1.0;

        // Perform a ray cast in the sun direction and get the point of
        // intersection with the ring plane
        float raycastScalar = -(worldPosition.y - planetPosition.y) / localSunDirection.y;
        if (raycastScalar < 0.0)
            raycastScalar = 0.0;
        vec3 intersectionPoint = worldPosition + raycastScalar * localSunDirection;

        // Get the distance from the centre of the planet
        float d = length(intersectionPoint - planetPosition);

        // If the distance lies between the boundaries, get the texture
        // coordinate and sample it at this point
        if (d > ringDimensions.x && d < ringDimensions.y)
        {
            float s = (d - ringDimensions.x) / ringDimensions.z;
            ringShadow = 1.0 - texture(ringsTexture, vec2(s, 0.5)).a;
        }

        // Apply the ring shadowing to the lighting
        lighting *= ringShadow;
    }

    colour = surfaceSample * lighting + nightFactor + atmosphereFactor;
}
