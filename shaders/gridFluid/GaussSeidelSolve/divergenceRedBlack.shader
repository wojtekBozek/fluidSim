#version 430

layout(local_size_x=16, local_size_y=16) in;

layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D vTex;

layout(binding = 2) uniform usampler2D cellType;

layout(binding = 3, r32f) uniform writeonly image2D divergence;

uniform float dx; 
uniform ivec2 gridSize;
uniform float dt;
uniform float overrelaxation;
float U(int i, int j) {return texelFetch(uTex, ivec2(i, j), 0).r;}
float V(int i, int j) {return texelFetch(vTex, ivec2(i, j), 0).r;}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

int checkCellType(ivec2 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

bool uBlocked(int i, int j)
{
    if(0 == i || i >= gridSize.x) return true;
    return checkCellType(ivec2(i-1, j)) == SOLID ||
           checkCellType(ivec2(i,   j)) == SOLID;
}

bool vBlocked(int i, int j)
{
    if(0 == j || j >= gridSize.y) return true;
    return checkCellType(ivec2(i, j-1)) == SOLID ||
           checkCellType(ivec2(i, j  )) == SOLID;
}

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

    float uR = uBlocked(i+1, j) ? 0.0 : U(i+1, j);
    float uL = uBlocked(i,   j) ? 0.0 : U(i,   j);
    float vT = vBlocked(i, j+1) ? 0.0 : V(i, j+1);
    float vB = vBlocked(i, j  ) ? 0.0 : V(i, j  );

    float div = overrelaxation*(uR - uL + vT - vB);
    imageStore(divergence, id, vec4(div));
}

