#version 330 core

uniform vec3 ringDimensions;
uniform float planetRadius;
uniform vec3 planetPosition;
uniform float planetPenumbraRecip;
uniform vec3 atmosphereColour;
uniform vec4 lightToPlanet;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec2 distanceThreshold;
uniform float znear;
uniform float zfar;

uniform sampler2D ringsTexture;
uniform sampler2D noiseTexture;

in float depth;
in vec3 worldPosition;
in vec2 noiseTexCoord;

out vec4 colour;

void main()
{
    // Ensure that the circular shape of the ring is perfect
    float distance = length(worldPosition - planetPosition);
    if (distance < ringDimensions.x || distance > ringDimensions.y)
        discard;

    // Apply fading out of the ring bands based on distance of the fragment
    // from the camera
    float distToPx = length(worldPosition - cameraPosition);
    if (distToPx < distanceThreshold.x)
        discard;

    // Fade to the noise texture
    float alpha = 1.0;
    if (distToPx < distanceThreshold.y)
    {
        float power = 3.0;
        float noiseFactor = pow(2.0, power) * clamp((distToPx - distanceThreshold.x) / (distanceThreshold.y - distanceThreshold.x), 0.0, 1.0);
        float noiseSample = texture(noiseTexture, noiseTexCoord).r;
        alpha = clamp(noiseSample + pow(noiseFactor, 1.0 / power) - 1.0, 0.0, 1.0);
    }

    // Calculate shadow factor
    vec3 shadowFactor = vec3(1.0);
    vec3 pxToLight = worldPosition - lightPosition;
    float lenPxToLight = length(pxToLight);

    // Only check pen/umbra if this pixel is on the planet's far side
    if (lenPxToLight > lightToPlanet.w)
    {
        float lengthRatio = lenPxToLight / lightToPlanet.w;
        vec3 newPlanetPos = lightPosition + lightToPlanet.xyz * lengthRatio;
        vec3 newPlanetToPx = newPlanetPos - worldPosition;
        float lenNewPlanetToPx = length(newPlanetToPx);

        float planetOver = clamp((planetRadius - lenNewPlanetToPx) * planetPenumbraRecip, 0.0, 1.0);
        float atmoOver = clamp((planetRadius - lenNewPlanetToPx) * planetPenumbraRecip, 0.0, 1.0);
        shadowFactor *= mix(vec3(1.0), vec3(0.0), planetOver);
        shadowFactor *= mix(vec3(1.0), atmosphereColour, atmoOver);
    }

    // Calculate texture coordinates and sample the texture then apply shadowing
    vec4 ringsSample = texture(ringsTexture, vec2((distance - ringDimensions.x) / ringDimensions.z, 0.5));
    ringsSample.rgb *= shadowFactor;
    ringsSample.a *= alpha;
    colour = ringsSample;

    // Logarithmic Z-depth
    gl_FragDepth = log(depth / znear) / log(zfar / znear);
}
