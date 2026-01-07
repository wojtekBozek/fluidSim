#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(r32f ,binding = 0) uniform image2D uTex;

layout(binding = 1) uniform sampler2D pressure;
layout(binding = 2) uniform usampler2D cellType;

uniform float dt;
uniform float dx;
uniform ivec2 gridSize;
uniform float density = 997.0;


const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    //left-right projection
    if(id.x < gridSize.x + 1 && id.y < gridSize.y)
    {
        int i = id.x;
        int j = id.y;

        uint leftType= (i-1 < 0) ? SOLID : texelFetch(cellType, ivec2(i-1,j), 0).r;
        uint rightType = (i >= gridSize.x) ? SOLID : texelFetch(cellType, ivec2(i,j), 0).r;

        float u = imageLoad(uTex, id).r;

        if (leftType == SOLID || rightType == SOLID)
        {
            u = 0.0;
        }
        else if(leftType == FLUID || rightType == FLUID)
        {
            float pressureL = (leftType == FLUID) ? texelFetch(pressure, ivec2(i-1,j), 0).r : 0.0;
            float pressureR = (rightType == FLUID) ? texelFetch(pressure, ivec2(i,j), 0).r : 0.0;

            float gradPressure = (pressureR - pressureL)/ dx;
            u -= dt * gradPressure;
        }
        else
        {
            u = 0.0;
        }
        imageStore(uTex, id, vec4(u));
    }
}