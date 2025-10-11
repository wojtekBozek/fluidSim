#version 430
layout(std430, binding = 0) buffer Pos {
    vec4 positions[];
};


void main()
{
    uint idx = gl_GlobalInvocationID.x;
    positions[idx] = positions[idx] + vec4(0.1, 0.0, 0.0, 0.0);
}