#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D inputTex;

layout(binding = 1) buffer Result {
    uint maxValue;
};

shared float sharedMax[256];

void main() {
    ivec2 size = textureSize(inputTex, 0);
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    if (coord.x >= size.x || coord.y >= size.y)
        return;

    float v = texelFetch(inputTex, coord, 0).r;

    uint lid = gl_LocalInvocationIndex;
    sharedMax[lid] = v;
    barrier();

    for (uint stride = gl_WorkGroupSize.x * gl_WorkGroupSize.y / 2; stride > 0; stride >>= 1) {
        if (lid < stride) {
            sharedMax[lid] = max(abs(sharedMax[lid]), abs(sharedMax[lid + stride]));
        }
        barrier();
    }

    if (lid == 0) {
        uint u = floatBitsToUint(sharedMax[0]);
        atomicMax(maxValue, u);
    }
}
