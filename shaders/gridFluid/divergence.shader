#version 430

layout (local_size_x=16, local_size_y=16);

layout (binding = 0) uniform sampler2D uTex;
layout (binding = 1) uniform sampler2D vTex;

layout (binding = 2) uniform usampler2D cellType;

layout (binding = 3, r32f) uniform writeonly image2D divergance;

uniform float dx; 
uniform ivec2 size;
uniform float dt;

float U(int i, int j) {return texelFetch(uTex, ivec2(i, j), 0).r;}
float V(int i, int j) {return texelFetch(vTex, ivec2(i, j), 0).r;}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= size.x || id.y >= size.y) return;


    uint type = texelFetch(cellType, id, 0).r;
    if(type == SOLID)
    {
        imageStore(divergance, id, vec4(0.0));
        return;
    }
    int i = id.x;
    int j = id.y;

    float uR, uL, vT, vB;

    bool rightSolid = (i+1 < size.x && texelFetch(cellType, ivec2(i+1,j),0).r == SOLID);
    uR = rightSolid ? 0.0 : U(i+1,j);

    bool leftSolid = (i-1 >= 0 && texelFetch(cellType, ivec2(i-1,j),0).r == SOLID);
    uL = leftSolid ? 0.0 : U(i,j);

    bool topSolid = (j+1 < size.y && texelFetch(cellType, ivec2(i,j+1),0).r == SOLID);
    vT = topSolid ? 0.0 : U(i,j);

    bool bottomSolid = (j-1 >= 0 && texelFetch(cellType, ivec2(i,j-1),0).r == SOLID);
    vB = bottomSolid ? 0.0 : U(i,j);

    float div = (uR - uL  + vT - vB) / dx;
    imageStore(divergance, div, vec4(div));
}

