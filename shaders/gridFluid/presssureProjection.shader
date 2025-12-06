#version 430

layout (binding = 0, rgba32f) uniform image2D uTex;
layout (binding = 1, rgba32f) uniform image2D vTex;

layout (binding = 2) uniform sampler2D pressure;
layout (binding = 3) uniform usampler2D cellType;

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
    if(id.x <= gridSize.x && id.y < gridSize.y)
    {
        int i = id.x;
        int j = id.y;

        bool leftFluid = (i-1 >= 0 && texelFetch(cellType, ivec2(i-1,j), 0).r == FLUID);
        bool rightFluid = (i+1 < gridSize.x && texelFetch(cellType, ivec2(i+1,j), 0).r == FLUID);

        float u = imageLoad(uTex, id).r;

        if (leftFluid || rightFluid)
        {
            float pressureL = (i-1 >= 0) ? texelFetch(pressure, ivec2(i-1, j), 0).r : texelFetch(pressure, ivec2(i,j), 0).r;
            float pressureR = (i+1 < gridSize.x) ? texelFetch(pressure, ivec2(i+1, j), 0).r : texelFetch(pressure, ivec2(i,j), 0).r;

            float gradPressure = (pressureR - pressureL)/ dx;
            u -= dt * gradPressure;
        }

        if(!leftFluid || !rightFluid)
        {
            u = 0.0;
        }

        imageStore(uTex, id, vec4(u));
    }

    //bottom-top projection
    if(id.y <= gridSize.y && id.x < gridSize.x)
    {
        int i = id.x;
        int j = id.y;

        bool bottomFluid = (j-1 >= 0 && texelFetch(cellType, ivec2(i,j-1), 0).r == FLUID);
        bool topFluid = (j+1 < gridSize.y && texelFetch(cellType, ivec2(i,j+1), 0).r == FLUID);

        float v = imageLoad(vTex, id).r;

        if (bottomFluid || topFluid)
        {
            float pressureB = (j-1 >= 0) ? texelFetch(pressure, ivec2(i, j-1), 0).r : texelFetch(pressure, ivec2(i,j), 0).r;
            float pressureT = (j+1 < gridSize.y) ? texelFetch(pressure, ivec2(i, j+1), 0).r : texelFetch(pressure, ivec2(i,j), 0).r;

            float gradPressure = (pressureT - pressureB)/ dx;
            v -= dt * gradPressure;
        }

        if(!bottomFluid || !topFluid)
        {
            v = 0.0;
        }
        
        imageStore(vTex, id, vec4(v));
    }
}