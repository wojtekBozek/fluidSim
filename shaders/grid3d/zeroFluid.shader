#version 430
layout(local_size_x = 16, local_size_y = 16, local_size_z = 16) in;

layout(binding = 0, r32ui) uniform uimage3D cellType;
layout(binding = 1, r32f) uniform image3D divergence;
layout(binding = 2, r32f) uniform image3D pressure;

uniform ivec3 gridSize;

const uint FLUID = 0u;
const uint AIR   = 1u;
const uint SOLID = 2u;
uniform bool zeroPressure;
void main() {
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if (id.x >= gridSize.x || id.y >= gridSize.y || id.z >= gridSize.z) return;

    bool isSolid = imageLoad(cellType, id).r == SOLID; 

    if (!isSolid) {
        imageStore(cellType, id, uvec4(AIR));
    }
    
    imageStore(divergence, id, uvec4(0.0));
    if(zeroPressure) imageStore(pressure, id, uvec4(0.0));
}