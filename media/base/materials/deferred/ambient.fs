#version 330 core

uniform sampler2D gb0;
uniform sampler2D gb1;

uniform mat4 proj;
uniform vec4 ambientColor;
uniform float znear;
uniform float zfar;

in vec3 oRay;
in vec2 oUv;

out vec4 colour;

float finalDepth(vec4 p)
{
    // Logarithmic Z-buffer
    return log(p.w / znear) / log(zfar / znear);
}

float decodeDepth(vec2 encodedDepth)
{
    return dot(encodedDepth, vec2(1.0, 1.0 / 2048.0));
}

void main()
{
    vec4 a0 = texture2D(gb0, oUv);
    vec4 a1 = texture2D(gb1, oUv);

    float depth = decodeDepth(a1.ba);

    // Discard fragment if depth is too close, so forward rendering can be used for transparent and
    // background objects
    if (depth == 0.0)
        discard;

    // Calculate ambient colour of fragment
    colour = vec4(ambientColor * vec4(a0.rgb, 0.0));

    // Calculate depth of fragment
    vec3 viewPos = normalize(oRay) * zfar * depth;
    vec4 projPos = proj * vec4(viewPos, 1.0);
    gl_FragDepth = finalDepth(projPos);
}
