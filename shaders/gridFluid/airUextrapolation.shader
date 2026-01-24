//This shader extrapolate velocities to the air faces 

#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform usampler2D cellType;

layout(r32f, binding = 2) uniform writeonly image2D uOut;

uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform int borderSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

int checkCellType(ivec2 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

uint typeAt(int i, int j)
{
    if(i < 0 || i >= gridSize.x || j<0 || j>=gridSize.y) return SOLID;
    return texelFetch(cellType, ivec2(i,j), 0).r;
}

bool isAirOnlyUFace(int i, int j)
{
    if(i <= 0 || i >= gridSize.x) return false;
    return checkCellType(ivec2(i-1, j)) == AIR &&
           checkCellType(ivec2(i,   j)) == AIR;
}

bool isFluidAirUFace(int i, int j)
{
    if(j <= 0 || j >= gridSize.y || i <= 0 || i >=gridSize.x) return false;
    return ((checkCellType(ivec2(i-1, j)) == FLUID && checkCellType(ivec2(i, j)) == AIR) 
    || (checkCellType(ivec2(i-1, j)) == AIR && checkCellType(ivec2(i, j)) == FLUID));
}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x + 1 || id.y >= gridSize.y) return;
    int i = id.x;
    int j = id.y;
    if(!isAirOnlyUFace(i,j))
    {
        float extU = texelFetch(uTex, ivec2(i, j),0).r;
        imageStore(uOut, ivec2(i,j),vec4(extU));
        return;
    }
    int real_ii = borderSize+1;
    int real_jj = borderSize+1;

    for(int ii = -borderSize; ii <= borderSize; ii++)
    {
        for(int jj = -borderSize; jj <= borderSize; jj++)
        {
            if(isFluidAirUFace(i + ii, j + jj))
            {
                if(ii*ii + jj*jj < real_ii*real_ii + real_jj*real_jj)
                {
                    real_ii = ii;
                    real_jj = jj;
                }
                //else if(pow(ii,2) + pow(jj,2) == pow(real_ii,2) + pow(real_jj,2))
                //{
                //    if(abs(ii) < abs(real_ii))
                //    {
                //        real_ii = ii;
                //        real_jj = jj;
                //    }
                //}
            }
        }
    }

    if(abs(real_ii) > borderSize) { imageStore(uOut, ivec2(i,j),vec4(0.0));return;}

    float extU = texelFetch(uTex, ivec2(i+real_ii, j+real_jj),0).r;
    imageStore(uOut, ivec2(i,j),vec4(extU));
}