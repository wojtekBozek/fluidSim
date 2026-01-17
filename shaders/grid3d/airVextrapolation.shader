//This shader extrapolate velocities to the air faces 

#version 430

layout(local_size_x=16, local_size_y=16, local_size_z = 16) in;
layout(binding = 0) uniform sampler3D vTex;
layout(binding = 1) uniform usampler3D cellType;

layout(r32f, binding = 2) uniform writeonly image3D vOut;

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

bool isAirOnlyVFace(int i, int j, int k)
{
    if(j <= 0 || j >= gridSize.y) return false;
    return checkCellType(ivec2(i, j-1,k)) == AIR &&
           checkCellType(ivec2(i,   j,k)) == AIR;
}

bool isFluidAirVFace(int i, int j, int k)
{
    if(j <= 0 || j >= gridSize.y || i <= 0 || i >=gridSize.x) return false;
    return ((checkCellType(ivec2(i, j-1, k)) == FLUID && checkCellType(ivec2(i, j, k)) == AIR) 
    || (checkCellType(ivec2(i, j-1, k)) == AIR && checkCellType(ivec2(i, j, k)) == FLUID));
}

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x || id.y >= gridSize.y + 1 || id.z > gridSize.z) return;
    int i = id.x;
    int j = id.y;
    int k = id.k;
    if(!isAirOnlyVFace(i,j, k))
    {
        float extV = texelFetch(vTex, ivec3(i, j, k),0).r;
        imageStore(vOut, ivec3(i,j,k),vec4(extV));
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
                if(isFluidAirVFace(i + ii, j + jj, k+kk))
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

    if(abs(real_jj) > borderSize) { imageStore(vOut, ivec2(i,j),vec4(0.0));return;}

    float extV = texelFetch(vTex, ivec3(i+real_ii, j+real_jj, k+real_kk),0).r;
    imageStore(vOut, ivec2(i,j),vec4(extV));
}