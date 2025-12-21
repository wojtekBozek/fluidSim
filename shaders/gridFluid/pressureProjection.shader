#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(r32f ,binding = 0) uniform image2D uTex;
layout(r32f ,binding = 1) uniform image2D vTex;

layout(binding = 2) uniform sampler2D pressure;
layout(binding = 3) uniform usampler2D cellType;

uniform float dt;
uniform float dx;
uniform ivec2 gridSize;


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
        else if(leftType == FLUID && rightType == FLUID)
        {
            float pressureL = texelFetch(pressure, ivec2(i-1,j), 0).r;
            float pressureR = texelFetch(pressure, ivec2(i,j), 0).r;

            float gradPressure = (pressureR - pressureL)/ dx;
            u -= dt * gradPressure;
        }
        imageStore(uTex, id, vec4(u));
    }

    //bottom-top projection
    if(id.y < gridSize.y + 1 && id.x < gridSize.x)
    {
        int i = id.x;
        int j = id.y;

        uint bottomType = (j-1 < 0) ? SOLID : texelFetch(cellType, ivec2(i,j-1), 0).r;
        uint topType = (j >= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j), 0).r;

        float v = imageLoad(vTex, id).r;

        if (bottomType==SOLID || topType==SOLID)
        {
            v = 0.0;
        }
        else if(bottomType == FLUID && topType == FLUID)
        {
            float pressureB = texelFetch(pressure, ivec2(i, j-1), 0).r;
            float pressureT = texelFetch(pressure, ivec2(i,j), 0).r;

            float gradPressure = (pressureT - pressureB)/ dx;
            v -= dt * gradPressure;
        }        
        imageStore(vTex, id, vec4(v));
    }
}