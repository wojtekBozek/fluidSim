#version 430

layout(local_size_x=16, local_size_y=16) in;

layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D vTex;

layout(binding = 2) uniform usampler2D cellType;

layout(binding = 3, r32f) uniform writeonly image2D divergence;

uniform float dx; 
uniform ivec2 gridSize;
uniform float dt;
uniform float overrelaxation = 1.9;
float U(int i, int j) {return texelFetch(uTex, ivec2(i, j), 0).r;}
float V(int i, int j) {return texelFetch(vTex, ivec2(i, j), 0).r;}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return;


    uint type = texelFetch(cellType, id, 0).r;
    if(type != FLUID)
    {
        imageStore(divergence, id, vec4(0.0));
        return;
    }
    int i = id.x;
    int j = id.y;

    float uR, uL, vT, vB;

    bool rightSolid = ( i+1 > gridSize.x || (i+1 < gridSize.x && texelFetch(cellType, ivec2(i+1,j),0).r == SOLID));
    uR = rightSolid ? 0.0 : U(i+1,j);

    bool leftSolid = (i-1 < 0 || (i-1 >= 0 && texelFetch(cellType, ivec2(i-1,j),0).r == SOLID));
    uL = leftSolid ? 0.0 : U(i,j);

    bool topSolid = (j+1 >= gridSize.y || (j+1 < gridSize.y && texelFetch(cellType, ivec2(i,j+1),0).r == SOLID));
    vT = topSolid ? 0.0 : V(i,j+1);

    bool bottomSolid = (j-1 < 0 || (j-1 >= 0 && texelFetch(cellType, ivec2(i,j-1),0).r == SOLID));
    vB = bottomSolid ? 0.0 : V(i,j);

    float div = overrelaxation*(uR - uL  + vT - vB) / dx;
    imageStore(divergence, id, vec4(div));
}

