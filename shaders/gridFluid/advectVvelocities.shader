//This shader advects velocities on the grid faces 

#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D vTex;
layout(binding = 2) uniform usampler2D cellType;

layout(r32f, binding = 3) uniform writeonly image2D vOut;

uniform ivec2 gridSize;
uniform float dt;
uniform float dx;

int Nx() {return gridSize.x;} 
int Ny() {return gridSize.y;}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;


int checkCellType(ivec2 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

bool uBlocked(int i, int j)
{
    if(0 == i) return true;
    return checkCellType(ivec2(i-1, j)) == SOLID ||
           checkCellType(ivec2(i,   j)) == SOLID;
}

bool vBlocked(int i, int j)
{
    if(0 == j) return true;
    return checkCellType(ivec2(i, j-1)) == SOLID ||
           checkCellType(ivec2(i, j  )) == SOLID;
}

float interpolateUonVFace(int i, int j)
{
    int i0 = clamp(i,     0, Nx());
    int i1 = clamp(i + 1, 0, Nx());

    int j0 = clamp(j - 1, 0, Ny() - 1);
    int j1 = clamp(j,     0, Ny() - 1);

    float value = 0.0;
    float div = 0.0;
    if(!uBlocked(i0,j1))
    {
      value += texelFetch(uTex, ivec2(i0,j1),0).r;
      div += 1.0;
    }
    if(!uBlocked(i0,j0))
    {
       value += texelFetch(uTex, ivec2(i0,j0),0).r;
       div += 1.0;
    }
    if(!uBlocked(i1,j1))
    {
      value += texelFetch(uTex, ivec2(i1,j1),0).r;
      div += 1.0;
    }
    if(!uBlocked(i1, j0))
    {
      value += texelFetch(uTex, ivec2(i1,j0),0).r;
      div += 1.0;
    }
    return (div>0.0) ? value/div : 0.0;
}

float interpolateUinGrid(vec2 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int((position.y-0.5*dx)/dx);
    float downDistance = ((position.y-0.5*dx) - j*dx);

    int i0 = clamp(i,     0, Nx());
    int i1 = clamp(i + 1, 0, Nx());

    int j0 = clamp(j, 0, Ny() - 1);
    int j1 = clamp(j+1,     0, Ny() - 1);
    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float value = 0.0;
    float sumW = 0.0;
    if(!uBlocked(i0,j0))
    {
      value += texelFetch(uTex, ivec2(i0,j0),0).r * w00 * w10;
      sumW += w00 * w10;
    }
    if(!uBlocked(i0,j1))
    {
      value += texelFetch(uTex, ivec2(i0,j1),0).r * w00 * w11;
      sumW += w00 * w11;
    }
    if(!uBlocked(i1,j1))
    {
      value += texelFetch(uTex, ivec2(i1,j1),0).r * w01 * w11;
      sumW += w01 * w11;
    }
    if(!uBlocked(i1,j0))
    {
      value += texelFetch(uTex, ivec2(i1,j0),0).r * w01 * w10;
      sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
}

float interpolateVinGrid(vec2 position)
{ 
    int i = int((position.x-0.5*dx)/dx);
    float backDistance = (position.x-0.5*dx) - i*dx;
    
    int j = int(position.y/dx);
    float downDistance = position.y - j*dx;

    int i0 = clamp(i,     0, Nx()-1);
    int i1 = clamp(i + 1, 0, Nx()-1);

    int j0 = clamp(j, 0, Ny());
    int j1 = clamp(j+1,     0, Ny());
    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float value = 0.0;
    float sumW = 0.0;
    if(!vBlocked(i0,j0))
    {
      value += texelFetch(vTex, ivec2(i0,j0),0).r * w00 * w10;
      sumW += w00 * w10;
    }
    if(!vBlocked(i0,j1))
    {
      value += texelFetch(vTex, ivec2(i0,j1),0).r * w00 * w11;
      sumW += w00 * w11;
    }
    if(!vBlocked(i1,j1))
    {
      value += texelFetch(vTex, ivec2(i1,j1),0).r * w01 * w11;
      sumW += w01 * w11;
    }
    if(!vBlocked(i1,j0))
    {
      value += texelFetch(vTex, ivec2(i1,j0),0).r * w01 * w10;
      sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
}

float sampleV(vec2 position)
{
    return interpolateVinGrid(position);
}

float sampleU(vec2 position)
{
    return interpolateUinGrid(position);
}

vec2 clampPosition(vec2 position)
{
    vec2 minPos = vec2(0.0);
    vec2 maxPos = vec2(gridSize.x * dx, gridSize.y* dx);

    position = clamp(position, minPos, maxPos);
    return position;
}

vec2 backTracePositionRK2(vec2 position, vec2 velocity)
{
    vec2 halfPos = position - dt*velocity/2.0;
    halfPos = clampPosition(halfPos);
    vec2 halfVelocity = vec2(sampleU(halfPos), sampleV(halfPos));

    vec2 backtracedPosition = position - dt*halfVelocity;
    backtracedPosition = clampPosition(backtracedPosition);
    return backtracedPosition;
}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y+1) return ;
    int i = id.x;
    int j = id.y;

    uint btype = (j-1 < 0 || j-1 >= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j-1), 0).r;
    uint ttype = (j< 0 || j>= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j), 0).r;
    if(btype == FLUID || ttype == FLUID)
    {
        vec2 positionV = vec2(i*dx+0.5*dx, j*dx);
        vec2 velocityOnV = vec2(interpolateUonVFace(i,j), texelFetch(vTex, ivec2(i,j), 0).r);
        vec2 backtracedPosition = backTracePositionRK2(positionV, velocityOnV);
        float v = sampleV(backtracedPosition);
        imageStore(vOut, id, vec4(v));
    }
    else
        imageStore(vOut, id, vec4(0.0));
}