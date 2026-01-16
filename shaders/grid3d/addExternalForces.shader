#version 430
layout(local_size_x=16, local_size_y=16, local_size_z=16) in;
layout(r32f, binding=0) uniform image3D vTex;
layout(binding=1) uniform usampler3D cellType;
uniform float vAccelerations;
uniform float dt;
uniform ivec3 gridSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec3(gl_GlobalInvocationID.xyz);

    if(id.x >= gridSize.x  || id.y >= gridSize.y + 1 || id.z > gridSize.z) return;
    
    uint ttype = (id.y < gridSize.y) ? texelFetch(cellType, ivec3(id.x, id.y, id.z), 0).r : SOLID;
    uint btype = (id.y > 0)          ? texelFetch(cellType, ivec3(id.x, id.y-1, id.z), 0).r : SOLID;

    if(ttype != FLUID && btype != FLUID)
    {
        imageStore(vTex, id, vec4(0.0));
        return;
    }

    if (ttype == SOLID || btype == SOLID)
    {
        imageStore(vTex, id, vec4(0.0));
        return;
    }
    float val = imageLoad(vTex, id).r;

    val += vAccelerations*dt;
    imageStore(vTex, id, vec4(val));
}