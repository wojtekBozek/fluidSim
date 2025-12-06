#version 430 

layout (binding = 0, rgba32f) uniform image2D uTex;
layout (binding = 0, rgba32f) unfiorm image2D vTex;

layout (binding = 0, rgba32f) uniform usampler2D celltype;


const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

uniform ivec2 gridSize;

bool solid (int i, int j)
{
    if(i < 0 || i > gridSize.x || j < 0 || j > gridSize.y)
    {
        return true;
    }

    return texelFetch(cellType, ivec2(i,j), 0).r == SOLID;
}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    if(id.x <= size.x && id.y < size.y )
    {
        int i = id.x;
        int j = id.y;

        bool leftSolid = solid(i-1, j);
        bool rightSolid = solid(i+1, j);

        float u = imageLoad(uTex, id).r;

        if(leftSolid || rightSolid)
        {
            u = 0.0;
        }

        imageStore(uTex, id, vec4(u,0,0,0));
    }

    if(id.y <= size.y && id.x < size.x )
    {
        int i = id.x;
        int j = id.y;

        bool bottomSolid = solid(i, j-1);
        bool topSolid = solid(i, j+1);

        float v = imageLoad(vTex, id).r;

        if(bottomSolid || topSolid)
        {
            v = 0.0;
        }

        imageStore(vTex, id, vec4(v,0,0,0));
    }
}