//This shader extrapolate velocities to the air faces 

#version 430

layout(local_size_x=16, local_size_y=16, local_size_z=16) in;
layout(binding = 0) uniform sampler3D uTex;
layout(binding = 1) uniform usampler3D cellType;

layout(r32f, binding = 2) uniform writeonly image3D uOut;

uniform ivec3 gridSize;
uniform float dt;
uniform float dx;
uniform int borderSize = 4;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

int checkCellType(ivec3 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

uint typeAt(int i, int j, int k)
{
    if(i < 0 || i >= gridSize.x || j<0 || j>=gridSize.y, k<0 || k>=gridSize.z) return SOLID;
    return texelFetch(cellType, ivec3(i,j, k), 0).r;
}

bool isAirOnlyUFace(int i, int j, int k)
{
    if(i <= 0 || i >= gridSize.x) return false;
    return checkCellType(ivec2(i-1, j, k)) == AIR &&
           checkCellType(ivec2(i,   j, k)) == AIR;
}

bool isFluidAirUFace(int i, int j, int k)
{
    if(j <= 0 || j >= gridSize.y || i <= 0 || i >=gridSize.x) return false;
    return ((checkCellType(ivec2(i-1, j, k)) == FLUID && checkCellType(ivec2(i, j, k)) == AIR) 
    || (checkCellType(ivec2(i-1, j, k)) == AIR && checkCellType(ivec2(i, j, k)) == FLUID));
}

void main()
{
    ivec3 id = ivec2(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x + 1 || id.y >= gridSize.y) return;
    int i = id.x;
    int j = id.y;
    int k = id.z;
    if(!isAirOnlyUFace(i,j,k))
    {
        float extU = texelFetch(uTex, ivec3(i, j,k),0).r;
        imageStore(uOut, ivec3(i,j,j),vec4(extU));
        return;
    }
    int real_ii = borderSize+1;
    int real_jj = borderSize+1;
    int real_kk = borderSize+1;

    for(int ii = -borderSize; ii <= borderSize; ii++)
    {
        for(int jj = -borderSize; jj <= borderSize; jj++)
        {
            for(int kk = -borderSize; kk <= borderSize; kk++)
            {
                if(isFluidAirUFace(i + ii, j + jj, k+kk))
                {
                    if(ii*ii + jj*jj + kk*kk < real_ii*real_ii + real_jj*real_jj + real_kk*real_kk)
                    {
                        real_ii = ii;
                        real_jj = jj;
                        real_kk = kk;
                    }
                }
            }
        }
    }

    if(abs(real_ii) > borderSize) { imageStore(uOut, ivec3(i,j, k),vec4(0.0));return;}

    float extU = texelFetch(uTex, ivec2(i+real_ii, j+real_jj, k ++real_kk),0).r;
    imageStore(uOut, ivec2(i,j),vec4(extU));
}