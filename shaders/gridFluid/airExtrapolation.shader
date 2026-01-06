//This shader advects velocities on the grid faces 

#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D vTex;
layout(binding = 2) uniform usampler2D cellType;

layout(r32f, binding = 3) uniform writeonly image2D uOut;
layout(r32f, binding = 4) uniform writeonly image2D vOut;

uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform int borderSize = 3;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

uint typeAt(int i, int j)
{
    if(i < 0 || i >= gridSize.x || j<0 || j>=gridSize.y) return SOLID;
    return texelFetch(cellType, ivec2(i,j), 0).r;
}


void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return;
    int i = id.x;
    int j = id.y;
    uint type = typeAt(i,j);
    uint leftType = typeAt(i-1,j);
    uint bottomType = typeAt(i,j-1);
    if(type != AIR)
    {
        float extV = texelFetch(vTex, ivec2(i,j),0).r;
        float extU = texelFetch(uTex, ivec2(i, j),0).r;
        imageStore(vOut, ivec2(i,j),vec4(extV));
        imageStore(uOut, ivec2(i,j),vec4(extU));
        return;
    }
    int real_ii = borderSize+1;
    int real_jj = borderSize+1;

    for(int ii = -borderSize; ii <= borderSize; ii++)
    {
        for(int jj = -borderSize; jj <= borderSize; jj++)
        {
            if(typeAt(ii, jj) == FLUID)
            {
                if(pow(ii,2) + pow(jj,2) < pow(real_ii,2) + pow(real_jj,2))
                {
                    real_ii = ii;
                    real_jj = jj;
                }
            }
        }
    }

    if(abs(real_ii) > borderSize) return;
    if(real_ii < 0) real_ii++; 
    if(real_jj < 0) real_jj++; 
    float extV = texelFetch(vTex, ivec2(i+real_ii,j+real_jj),0).r;
    float extU = texelFetch(uTex, ivec2(i+real_ii, j+real_jj),0).r;
    if(bottomType != AIR)
    {
        extV = texelFetch(vTex, ivec2(i,j),0).r;
    }

    if(leftType != AIR)
    {
        extU = texelFetch(uTex, ivec2(i, j),0).r;
    }

    imageStore(vOut, ivec2(i,j),vec4(extV));
    imageStore(uOut, ivec2(i,j),vec4(extU));
}