#version 430

layout(local_size_x=16, local_size_y=16) in;

layout(binding = 0) uniform sampler3D uTex;
layout(binding = 1) uniform sampler3D vTex;
layout(binding = 2) uniform sampler3D wTex;

layout(binding = 3) uniform usampler3D cellType;

layout(binding = 4, r32f) uniform writeonly image3D divergence;

uniform float dx; 
uniform ivec3 gridSize;
uniform float dt;
float U(int i, int j, int k) {return texelFetch(uTex, ivec3(i, j, k), 0).r;}
float V(int i, int j, int k) {return texelFetch(vTex, ivec3(i, j, k), 0).r;}
float W(int i, int j, int k) {return texelFetch(wTex, ivec3(i, j, k), 0).r;}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

int checkCellType(ivec3 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

bool uBlocked(int i, int j, int k)
{
    if(0 == i || i >= gridSize.x) return true;
    return checkCellType(ivec2(i-1, j, k)) == SOLID ||
           checkCellType(ivec2(i,   j, k)) == SOLID;
}

bool vBlocked(int i, int j, int k)
{
    if(0 == j || j >= gridSize.y) return true;
    return checkCellType(ivec2(i, j-1, k)) == SOLID ||
           checkCellType(ivec2(i, j  , k)) == SOLID;
}

bool wBlocked(int i, int j, int k)
{
    if(0 == k || k >= gridSize.x) return true;
    return checkCellType(ivec2(i, j, k-1)) == SOLID ||
           checkCellType(ivec2(i,   j, k)) == SOLID;
}

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x || id.y >= gridSize.y || id.z >= gridSize.z) return;


    uint type = texelFetch(cellType, id, 0).r;
    if(type != FLUID)
    {
        imageStore(divergence, id, vec4(0.0));
        return;
    }
    int i = id.x;
    int j = id.y;
    int k = id.z;

    float uR = uBlocked(i+1, j,k) ? 0.0 : U(i+1, j,k);
    float uL = uBlocked(i,   j,k) ? 0.0 : U(i,   j,k);
    float vT = vBlocked(i, j+1,k) ? 0.0 : V(i, j+1,k);
    float vB = vBlocked(i, j  ,k) ? 0.0 : V(i, j  ,k);
    float wN = wBlocked(i, j,k+1) ? 0.0 : W(i, j,k+1);
    float wS = wBlocked(i, j ,k) ? 0.0 : W(i, j  ,k);

    float div = (uR - uL + vT - vB + wN - wS);
    imageStore(divergence, id, vec4(div));
}

