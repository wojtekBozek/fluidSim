//This shader extrapolate velocities to the air faces 

#version 430

layout(local_size_x=8, local_size_y=8, local_size_z = 8) in;
layout(binding = 0) uniform sampler3D wTex;
layout(binding = 1) uniform usampler3D cellType;

layout(r32f, binding = 2) uniform writeonly image3D wOut;

uniform ivec3 gridSize;
uniform float dt;
uniform float dx;
uniform int borderSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

int checkCellType(ivec3 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

uint typeAt(int i, int j, int k)
{
    if(i < 0 || i >= gridSize.x || j<0 || j>=gridSize.y || k<0 || k>=gridSize.z) return SOLID;
    return texelFetch(cellType, ivec3(i,j, k), 0).r;
}

bool isAirOnlyWFace(int i, int j, int k)
{
    if(k <= 0 || k >= gridSize.y) return false;
    return checkCellType(ivec3(i, j,k-1)) == AIR &&
           checkCellType(ivec3(i, j,k)) == AIR;
}

bool isFluidAirWFace(int i, int j, int k)
{
    if(j <= 0 || j >= gridSize.y || i <= 0 || i >=gridSize.x) return false;
    return ((checkCellType(ivec3(i, j, k-1)) == FLUID && checkCellType(ivec3(i, j, k)) == AIR) 
    || (checkCellType(ivec3(i, j, k-1)) == AIR && checkCellType(ivec3(i, j, k)) == FLUID));
}

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x || id.y >= gridSize.y || id.z > gridSize.z + 1) return;
    int i = id.x;
    int j = id.y;
    int k = id.z;
    if(!isAirOnlyWFace(i,j, k))
    {
        float extW = texelFetch(wTex, ivec3(i, j, k),0).r;
        imageStore(wOut, ivec3(i,j,k),vec4(extW));
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
                if(isFluidAirWFace(i + ii, j + jj, k+kk))
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

    if(abs(real_kk) > borderSize) { imageStore(wOut, ivec3(i,j,k),vec4(0.0));return;}

    float extW = texelFetch(wTex, ivec3(i+real_ii, j+real_jj, k+real_kk),0).r;
    imageStore(wOut, ivec3(i,j,k),vec4(extW));
}