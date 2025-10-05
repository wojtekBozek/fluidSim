#version 430

layout(std430, binding = 0) buffer PosBuffer {
    vec4 positions[];
};

layout(location = 0) in vec2 quadVertex; // unit quad corner

out vec4 vertColor;

uniform mat4 viewProj;  
uniform mat4 viewMatrix;
uniform vec2 halfSize = vec2(0.01, 0.01);

void main() {
    uint particleID = gl_InstanceID;
    vec3 worldPos = positions[particleID].xyz;

    vec3 camRight = normalize(inverse(viewMatrix)[0].xyz);
    vec3 camUp = normalize(inverse(viewMatrix)[1].xyz);

    vec3 offset = (quadVertex.x * halfSize.x) * camRight +
                  (quadVertex.y * halfSize.y) * camUp;
    vec4 pos = vec4(worldPos + offset, 1.0);

    gl_Position = viewProj * pos;
    vertColor = vec4(1.0, 0.3, 0.3, 1.0); // fixed red-ish color
}