#version 330 core

#define LIGHT_POINT         1
#define LIGHT_SPOT          2
#define LIGHT_DIRECTIONAL   3

//////////////////////////////////////////////////////////////////////////////
// Helper function section
//////////////////////////////////////////////////////////////////////////////

float shadowTexturePixelOffset = 1.0 / 2048.0;

float lookup(sampler2D shadowMap, vec2 shadowSampleTexCoord, float shadowFarClip, float distanceFromLight)
{
    // Shadows only affect the scene if the projected coordinate
    // lies between the bounds of the texture
    if (shadowSampleTexCoord.s >= 0.0 && shadowSampleTexCoord.s <= 1.0
        && shadowSampleTexCoord.t >= 0.0 && shadowSampleTexCoord.t <= 1.0)
    {
        float shadowDepth = texture2D(shadowMap, shadowSampleTexCoord).r;
        float shadowDistance = shadowDepth * shadowFarClip;
        if (distanceFromLight > shadowDistance && distanceFromLight < shadowFarClip)
            return 0.0;
    }

    return 1.0;
}

float getShadow(
    sampler2D shadowMap,
    vec3 viewPos,
    mat4 invView,
    mat4 shadowViewProj,
    float shadowFarClip,
#if LIGHT_TYPE == LIGHT_DIRECTIONAL
    vec3 shadowCamPos
#else
    float distanceFromLight
#endif
    )
{
    vec3 worldPos = (invView * vec4(viewPos, 1.0)).xyz;
#if LIGHT_TYPE == LIGHT_DIRECTIONAL
    float distanceFromLight = length(shadowCamPos - worldPos);
#endif
    vec4 shadowProjPos = shadowViewProj * vec4(worldPos, 1.0);
    shadowProjPos /= shadowProjPos.w;

    float shadow = 0.0;

    // Simple shadow mapping
    //shadow = lookup(shadowMap, shadowProjPos.xy, shadowFarClip, distanceFromLight);

    // PCM 4x4
    float x, y;
    for (y = -1.5; y <= 1.5 ; y += 1.0)
        for (x = -1.5 ; x <= 1.5 ; x += 1.0)
            shadow += lookup(shadowMap, shadowProjPos.xy + vec2(x, y) * shadowTexturePixelOffset, shadowFarClip, distanceFromLight);
    shadow /= 16.0;

    // PCM 8x8
    /*
    float x, y;
    for (y = -3.5; y <= 3.5 ; y += 1.0)
        for (x = -3.5 ; x <= 3.5 ; x += 1.0)
            shadow += lookup(shadowMap, shadowProjPos.xy + vec2(x, y) * shadowTexturePixelOffset, shadowFarClip, distanceFromLight);
    shadow /= 64.0;
    */

    return shadow;
}

//////////////////////////////////////////////////////////////////////////////
// Main shader section
//////////////////////////////////////////////////////////////////////////////

uniform sampler2D gb0;
uniform sampler2D gb1;

#if LIGHT_TYPE != LIGHT_POINT
uniform vec3 lightDir;
#endif

#if LIGHT_TYPE == LIGHT_SPOT
uniform vec4 spotParams;
#endif

#if LIGHT_TYPE != LIGHT_DIRECTIONAL
uniform float vpWidth;
uniform float vpHeight;
uniform vec3 farCorner;
uniform float flip;
#endif

#ifdef IS_SHADOW_CASTER
uniform mat4 invView;
uniform mat4 shadowViewProjMat;
uniform sampler2D ShadowTex;
uniform vec3 shadowCamPos;
uniform float shadowFarClip;
#endif

uniform float farClipDistance;
// Attributes of light
uniform vec4 lightDiffuseColor;
uniform vec4 lightSpecularColor;
uniform vec4 lightFalloff;
uniform vec3 lightPos;

#if LIGHT_TYPE == LIGHT_DIRECTIONAL
in vec2 oUv;
in vec3 oRay;
#else
in vec4 oPos;
#endif

out vec4 fragColour;

vec3 decodeNormal(vec2 enc)
{
    float scale = 1.7777;
    vec3 nn = vec3(enc * vec2(2.0 * scale, 2.0 * scale), 0.0) + vec3(-scale, -scale, 1.0);
    float g = 2.0 / dot(nn.xyz, nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g - 1.0;
    return n;
}

float decodeDepth(vec2 encodedDepth)
{
    return dot(encodedDepth, vec2(1.0, 1.0 / 2048.0));
}

void main()
{
    // Non-directional lights have some calculations to do in the beginning of the pixel shader
#if LIGHT_TYPE != LIGHT_DIRECTIONAL
    vec4 normProjPos = oPos / oPos.w;
    // -1 is because generally +Y is down for textures but up for the screen
    vec2 oUv = vec2(normProjPos.x, normProjPos.y * -1.0 * flip) * 0.5 + 0.5;
    vec3 oRay = vec3(normProjPos.x, normProjPos.y * flip, 1.0) * farCorner;
#endif

    vec4 a0 = texture2D(gb0, oUv);
    vec4 a1 = texture2D(gb1, oUv);

    // Attributes
    vec3 colour = a0.rgb;
    float specularity = a0.a;
    vec3 normal = decodeNormal(a1.rg);
    float distance = decodeDepth(a1.ba);  // Distance from viewer

    // Calculate position of texel in view space
    vec3 viewPos = normalize(oRay) * distance * farClipDistance;

    // Calculate light direction and distance
#if LIGHT_TYPE == LIGHT_DIRECTIONAL
    vec3 objToLightDir = -lightDir.xyz;
#else
    vec3 objToLightVec = lightPos - viewPos;
    float len_sq = dot(objToLightVec, objToLightVec);
    float len = sqrt(len_sq);
    vec3 objToLightDir = objToLightVec / len;
#endif

    // Calculate diffuse colour
    vec3 totalLight;
    totalLight = max(0.0, dot(objToLightDir, normal)) * lightDiffuseColor.rgb;

#if IS_SPECULAR
    // Calculate specular component
    vec3 viewDir = -normalize(viewPos);
    vec3 h = normalize(viewDir + objToLightDir);
    vec3 light_specular = pow(dot(normal, h), 32.0) * lightSpecularColor.rgb;

    totalLight += specularity * light_specular;
#endif

#if IS_ATTENUATED
    if(lightFalloff.x - len < 0.0)
        discard;

    // Calculate attenuation
    float attenuation = dot(lightFalloff.yzw, vec3(1.0, len, len_sq));
    totalLight /= attenuation;
#endif

#if LIGHT_TYPE == LIGHT_SPOT
    float spotlightAngle = clamp(dot(lightDir.xyz, -objToLightDir), 0.0, 1.0);
    float spotFalloff = clamp((spotlightAngle - spotParams.x) / (spotParams.y - spotParams.x), 0.0, 1.0);
    totalLight *= (1.0 - spotFalloff);
#endif

#ifdef IS_SHADOW_CASTER
    #if LIGHT_TYPE == LIGHT_DIRECTIONAL
    totalLight *= getShadow(ShadowTex, viewPos, invView, shadowViewProjMat, shadowFarClip, shadowCamPos);
    #else
    totalLight *= getShadow(ShadowTex, viewPos, invView, shadowViewProjMat, shadowFarClip, len);
    #endif
#endif

    fragColour = vec4(totalLight * colour, 0.0);
}
