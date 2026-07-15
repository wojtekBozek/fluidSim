#version 430
layout(local_size_x=8, local_size_y=8, local_size_z=8) in;
layout(binding = 0, r32f) uniform image3D uTex;
layout(binding = 1, r32f) uniform image3D vTex;
layout(binding = 2, r32f) uniform image3D wTex;
layout(binding=3) uniform usampler3D cellType;


uniform vec3 accelerations;
uniform float dt;
uniform ivec3 gridSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x < gridSize.x + 1  && id.y < gridSize.y && id.z < gridSize.z){
    
        uint ttype = (id.x < gridSize.x) ? texelFetch(cellType, ivec3(id.x, id.y, id.z), 0).r : SOLID;
        uint btype = (id.x > 0)          ? texelFetch(cellType, ivec3(id.x-1, id.y, id.z), 0).r : SOLID;

        if (ttype != FLUID && btype != FLUID)
        {
            imageStore(uTex, id, vec4(0.0));
        }
        else if (ttype == SOLID || btype == SOLID)
        {
            imageStore(uTex, id, vec4(0.0));
        } 
        else 
        {
            float val = imageLoad(uTex, id).r;    
            val += accelerations.x*dt;
            imageStore(uTex, id, vec4(val));
        }
    }
    if(id.x < gridSize.x  && id.y < gridSize.y + 1 && id.z < gridSize.z){
    
        uint ttype = (id.y < gridSize.y) ? texelFetch(cellType, ivec3(id.x, id.y, id.z), 0).r : SOLID;
        uint btype = (id.y > 0)          ? texelFetch(cellType, ivec3(id.x, id.y-1, id.z), 0).r : SOLID;

        if(ttype != FLUID && btype != FLUID)
        {
            imageStore(vTex, id, vec4(0.0));
        }
        else if (ttype == SOLID || btype == SOLID)
        {
            imageStore(vTex, id, vec4(0.0));
        }
        else
        {
            float val = imageLoad(vTex, id).r;
            val += accelerations.y*dt;
            imageStore(vTex, id, vec4(val));
        }
    }
    if(id.x < gridSize.x  && id.y < gridSize.y && id.z < gridSize.z+1){
    
        uint ttype = (id.z < gridSize.z) ? texelFetch(cellType, ivec3(id.x, id.y, id.z), 0).r : SOLID;
        uint btype = (id.z > 0)          ? texelFetch(cellType, ivec3(id.x, id.y, id.z-1), 0).r : SOLID;

        if(ttype != FLUID && btype != FLUID)
        {
            imageStore(wTex, id, vec4(0.0));
        }
        else if (ttype == SOLID || btype == SOLID)
        {
            imageStore(wTex, id, vec4(0.0));
        }
        else
        {
            float val = imageLoad(wTex, id).r;
            val += accelerations.z*dt;
            imageStore(wTex, id, vec4(val));
        }
    }
}