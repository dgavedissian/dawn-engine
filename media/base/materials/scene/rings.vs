#version 330 core

uniform mat4 worldViewProjMatrix;
uniform mat4 worldMatrix;
uniform vec3 ringDimensions;

in vec4 vertex;

out float depth;
out vec3 worldPosition;
out vec2 noiseTexCoord;

void main()
{
    vec4 position = worldViewProjMatrix * vertex;
    gl_Position = position;
    depth = position.z;

    worldPosition = vec3(worldMatrix * vertex);
    noiseTexCoord = (vec2(vertex.x, vertex.z) / (2.0 * ringDimensions.y) + vec2(0.5, 0.5)) * 2000.0;
}
