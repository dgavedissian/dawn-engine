#version 330 core

uniform vec3 farCorner;
uniform float renderTargetFlipping;

in vec4 vertex;

out vec2 oUv;
out vec3 oRay;

void main()
{
    vec2 pos = sign(vertex.xy);
    gl_Position = vec4(pos, 0, 1);
    gl_Position.y *= renderTargetFlipping;
    oUv = (vec2(pos.x, -pos.y) + 1.0) * 0.5;

    // This ray will be interpolated and will be the ray from the camera to the far clip plane
    oRay = farCorner * vec3(pos, 1);
}
