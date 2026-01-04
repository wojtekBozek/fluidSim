//#version 430
//
//layout (local_size_x = 8, local_size_y = 8) in;
//
//layout (binging = 0, rgba32f) uniform writeonly image2D uTex;
//layout (binging = 1, rgba32f) uniform writeonly image2D vTex;
//layout (binging = 2, rgba32f) uniform writeonly image2D pressure;
//layout (binging = 3, rgba32f) uniform writeonly image2D divergence;
//layout (binging = 4, rgba32f) uniform writeonly image2D density;
//
//uniform ivec2 gridSize;

#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform writeonly image2D cellTypeOut;
layout(binging = 1, r32f) uniform writeonly image2D uTex;
layout(binging = 2, r32f) uniform writeonly image2D vTex;
layout(binging = 3, r32f) uniform writeonly image2D pressure;
layout(binging = 4, r32f) uniform writeonly image2D divergence;

uniform ivec2 gridSize;

const uint FLUID = 0u;
const uint AIR   = 1u;

uniform float height = 0.25;
uniform float width = 0.8;

void main() {
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x < gridSize.x+1 && id.y < gridSize.y)
    {
        imageStore(uTex, id, vec4(0.0));
    }

    if(id.x < gridSize.x && id.y < gridSize.y+1)
    {
        imageStore(vTex, id, vec4(0.0));
    }
    if (id.x >= gridSize.x || id.y >= gridSize.y) return;

    imageStore(pressure, id, vec4(0.0));
    imageStore(divergence, id, vec4(0.0));
    bool isFluid = 
        id.x < gridSize.x * width &&
        id.y < gridSize.y * height;

    if (isFluid) {
        imageStore(cellTypeOut, id, uvec4(FLUID));
    } else {
        imageStore(cellTypeOut, id, uvec4(AIR));
    }
}