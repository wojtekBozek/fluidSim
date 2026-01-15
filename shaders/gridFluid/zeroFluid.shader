#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, r32ui) uniform uimage2D cellType;
layout(binding = 1, r32f) uniform image2D divergence;
layout(binding = 2, r32f) uniform image2D pressure;

uniform ivec2 gridSize;

const uint FLUID = 0u;
const uint AIR   = 1u;
const uint SOLID = 2u;

void main() {
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if (id.x >= gridSize.x || id.y >= gridSize.y) return;

    bool isSolid = imageLoad(cellType, id).r == SOLID; 

    if (!isSolid) {
        imageStore(cellType, id, uvec4(AIR));
    }
    
    imageStore(divergence, id, uvec4(0.0));
    //imageStore(pressure, id, uvec4(0.0));
}