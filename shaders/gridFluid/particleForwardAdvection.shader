#version 430 

layout(local_size_x = 256) in;

layout(binding = 0) uniform usampler2D cellType;


struct Particle {
    vec2 position;
};

layout(std430, binding = 1) buffer Particles {
    vec2 particles[];
};


layout(binding = 2) uniform sampler2D uTex;
layout(binding = 3) uniform sampler2D vTex;


uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform uint numOfParticles;

const int Nx = gridSize.x;
const int Ny = gridSize.y;


int checkCellType(ivec2 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

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

float interpolateUinGrid(vec2 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int(position.y/dx);
    float downDistance = (position.y - j*dx);

    if(downDistance < dx/2.0)
    {
        j--;
        downDistance += dx/2.0;
    }

    int i0 = clamp(i,0, Nx-1);
    int i1 = clamp(i + 1, 0, Nx-1);

    int j0 = clamp(j, 0, Ny);
    int j1 = clamp(j+1,     0, Ny);
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
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    
    int j = int(position.y/dx);
    float downDistance = position.y - j*dx;

    if(backDistance < dx/2.0)
    {
        i--;
        backDistance += dx/2.0;
    }

    int i0 = clamp(i,0, Nx-1);
    int i1 = clamp(i + 1, 0, Nx-1);

    int j0 = clamp(j, 0, Ny);
    int j1 = clamp(j+1,     0, Ny);
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
    vec2 maxPos = vec2(gridSize.x * dx, gridSize.y * dx);

    position = clamp(position, minPos, maxPos);
    return position;
}
vec2 forwardPosition(vec2 position, vec2 velocity)
{
    vec2 newPosition = position + dt*velocity;
    newPosition = clampPosition(newPosition);
    ivec2 newCell = ivec2(newPosition.x/dx, newPosition.y/dx);
    if(newCell.x < 0 || newCell.x >= gridSize.x 
        || newCell.y < 0 || newCell.y >= gridSize.y 
        ||texelFetch(cellType, newCell, 0).r == SOLID)
    {
        newPosition = position;
    }
    return newPosition;
}

vec2 forwardRK2Position(vec2 position, vec2 velocity)
{
    vec2 halfPosition = position + dt*0.5*velocity;
    halfPosition = clampPosition(halfPosition);
    ivec2 halfCell = ivec2(halfPosition.x/dx, halfPosition.y/dx);
    if(halfCell.x < 0 || halfCell.x >= gridSize.x 
        || halfCell.y < 0 || halfCell.y >= gridSize.y 
        ||texelFetch(cellType, halfCell, 0).r == SOLID)
    {
        halfPosition = position;
    }

    vec2 halfVelocity = vec2(sampleU(halfPosition), sampleV(halfPosition));
    vec2 newPosition = position + dt*halfVelocity;
    ivec2 newCell = ivec2(newPosition.x/dx, newPosition.y/dx);
    if(newCell.x < 0 || newCell.x >= gridSize.x 
        || newCell.y < 0 || newCell.y >= gridSize.y 
        ||texelFetch(cellType, newCell, 0).r == SOLID)
    {
        newPosition = position;
    }
    return newPosition;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;

    vec2 position = particles[id];
    vec2 velocity = vec2(sampleU(position), sampleV(position));
    vec2 newPosition = forwardRK2Position(position, velocity);
    particles[id] = newPosition;
}