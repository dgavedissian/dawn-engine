#version 330 core

uniform float renderTargetFlipping;

in vec4 vertex;

out vec2 oUv;

void main()
{
    vec2 pos = sign(vertex.xy);
    gl_Position = vec4(pos, 0, 1);
    gl_Position.y *= renderTargetFlipping;
    oUv = (vec2(pos.x, -pos.y) + 1.0) * 0.5;
}
