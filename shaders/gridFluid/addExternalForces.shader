#version 430
layout(local_size_x=16, local_size_y=16) in;
layout(r32f, binding=0) uniform image2D vTex;
layout(binding=1) uniform usampler2D cellType;
//layout(r32f, binding = 1) uniform writeonly image2D vOut;
uniform float vAccelerations;
uniform float dt;
uniform ivec2 gridSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    if(id.x >= gridSize.x  || id.y >= gridSize.y + 1) return;
    
    if(texelFetch(cellType, id, 0).r != FLUID && texelFetch(cellType, ivec2(id.x, id.y-1), 0).r != FLUID) {return;}

    float val = imageLoad(vTex, id).r;

    val += vAccelerations*dt;
    imageStore(vTex, id, vec4(val));
}