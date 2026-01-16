#version 430

layout(local_size_x=16, local_size_y=16, local_size_z=16) in;
layout(r32f ,binding = 0) uniform image3D wTex;

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

    //left-right projection
    if(id.x < gridSize.x && id.y < gridSize.y && id.z < gridSize.z+1)
    {
        int i = id.x;
        int j = id.y;
        int k = id.z;

        uint leftType= (k-1 < 0) ? SOLID : texelFetch(cellType, ivec2(i,j,k-1), 0).r;
        uint rightType = (k >= gridSize.z) ? SOLID : texelFetch(cellType, ivec2(i,j,k), 0).r;

        float w = imageLoad(wTex, id).r;

        if (leftType == SOLID || rightType == SOLID)
        {
            w = 0.0;
        }
        else if(leftType == FLUID || rightType == FLUID)
        {
            float pressureL = (leftType == FLUID) ? texelFetch(pressure, ivec2(i,j,k-1), 0).r : 0.0;
            float pressureR = (rightType == FLUID) ? texelFetch(pressure, ivec2(i,j,k), 0).r : 0.0;

            float gradPressure = (pressureR - pressureL)/ dx;
            w -= dt * gradPressure;
        }
        else
        {
            w = 0.0;
        }
        imageStore(wTex, id, vec4(u));
    }
}