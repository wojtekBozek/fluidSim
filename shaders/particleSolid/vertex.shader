#version 430

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

layout(location = 0) in vec2 quadVertex; // unit quad corner

out vec4 vertColor;

uniform mat4 viewProj;  
uniform vec2 halfSize = vec2(0.01, 0.01);

void main() {
    uint particleID = gl_InstanceID;
    vec3 worldPos = positions[particleID].xyz;

    vec2 offset = quadVertex * halfSize;
    vec4 pos = vec4(worldPos.xy + offset, worldPos.z, 1.0);

    gl_Position = viewProj * pos;
    vertColor = vec4(1.0, 0.3, 0.3, 1.0); // fixed red-ish color
}