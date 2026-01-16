#version 430

layout(local_size_x=16, local_size_y=16, local_size_z=16) in;
layout(r32f ,binding = 0) uniform image3D vTex;

layout(binding = 1) uniform sampler3D pressure;
layout(binding = 2) uniform usampler3D cellType;

uniform float dt;
uniform float dx;
uniform ivec3 gridSize;
uniform float density = 997.0;


const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec3 id = ivec2(gl_GlobalInvocationID.xyz);

    //bottom-top projection
    if(id.y < gridSize.y + 1 && id.x < gridSize.x && id.z < gridSize.z)
    {
        int i = id.x;
        int j = id.y;
        int k = id.z;

        uint bottomType = (j-1 < 0) ? SOLID : texelFetch(cellType, ivec2(i,j-1,k), 0).r;
        uint topType = (j >= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j,k), 0).r;

        float v = imageLoad(vTex, id).r;

        if (bottomType==SOLID || topType==SOLID)
        {
            v = 0.0;
        }
        else if(bottomType == FLUID || topType == FLUID)
        {
            float pressureB = (bottomType == FLUID) ? texelFetch(pressure, ivec2(i, j-1,k), 0).r : 0.0;
            float pressureT = (topType == FLUID) ? texelFetch(pressure, ivec2(i,j,k), 0).r : 0.0;

            float gradPressure = (pressureT - pressureB)/ dx;
            v -= dt * gradPressure;
        }  
        else
        {
            v = 0.0;
        }
        imageStore(vTex, id, vec4(v));
    }
}