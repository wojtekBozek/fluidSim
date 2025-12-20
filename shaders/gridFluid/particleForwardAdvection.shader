#version 430 

layout(local_size_x = 256) in;

layout(r8ui, binding = 0) uniform usampler2D cellType;

uniform ivec2 gridSize;

struct Particle {
    vec2 position;
};

layout(std430, binding = 1) buffer Particles {
    Particle particles[];
};


layout (binding = 2) sampler2D uTex;
layout (binding = 3) sampler2D vTex;


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


vec2 clampU(vec2 p)
{
    return clamp(p,
        vec2(0.0, 0.5*dx),
        vec2(gridSize.x*dx, (gridSize.y-0.5)*dx));
}

vec2 clampV(vec2 p)
{
    return clamp(p,
        vec2(0.5*dx, 0.0),
        vec2((gridSize.x-0.5)*dx, gridSize.y*dx));
}

float interpolateUinGrid(vec2 position)
{ 
    int i = position.x/dx;
    float backDistance = position.x - i*dx;
    int j = position.y/dx;
    float downDistance = (position.y - j*dx);

    if(downDistance < dx/2.0)
    {
        j--;
        downDistance += dx/2.0;
    }

    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float value = 0.0;
    float sumW = 0.0;
    if(!uBlocked(i,j))
    {
        value += texelFetch(uTex, ivec2(i,j),0).r * w00 * w10;
        sumW += w00 * w10;
    }
    if(!uBlocked(i,j+1))
    {
        value += texelFetch(uTex, ivec2(i,j+1),0).r * w00 * w11;
        sumW += w00 * w11;
    }
    if(!uBlocked(i+1,j+1))
    {
        value += texelFetch(uTex, ivec2(i+1,j+1),0).r * w01 * w11;
        sumW += w01 * w11;
    }
    if(!uBlocked(i+1,j))
    {
        value += texelFetch(uTex, ivec2(i+1,j),0).r * w01 * w10;
        sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
}

float interpolateVinGrid(vec2 position)
{ 
    int i = position.x/dx;
    float backDistance = position.x - i*dx;
    
    int j = position.y/dx;
    float downDistance = position.y - j*dx;

    if(backDistance < dx/2.0)
    {
        i--;
        backDistance += dx/2.0;
    }

    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float value = 0.0;
    float sumW = 0.0;
    if(!vBlocked(i,j))
    {
        value += texelFetch(vTex, ivec2(i,j),0).r * w00 * w10;
        sumW += w00 * w10;
    }
    if(!vBlocked(i,j+1))
    {
        value += texelFetch(vTex, ivec2(i,j+1),0).r * w00 * w11;
        sumW += w00 * w11;
    }
    if(!vBlocked(i+1,j+1))
    {
        value += texelFetch(vTex, ivec2(i+1,j+1),0).r * w01 * w11;
        sumW += w01 * w11;
    }
    if(!vBlocked(i+1,j))
    {
        value += texelFetch(vTex, ivec2(i+1,j),0).r * w01 * w10;
        sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
}

float sampleV(vec2 position)
{
    return interpolateVinGrid(clampV(position));
}

float sampleU(vec2 position)
{
    return interpolateUinGrid(clampU(position));
}

vec2 clampPosition(vec2 position)
{
    vec2 minPos = vec2(0.5 * dx);
    vec2 maxPos = vec2((gridSize.x - 1.5) * dx, (gridSize.y - 1.5) * dx);

    position = clamp(position, minPos, maxPos);
    return position;
}
vec2 forwardPosition(vec2 position, vec2 velocity)
{
    vec2 newPosition = position + dt*velocity;
    newPosition = clampPosition(newPosition);
    ivec2 newCell = ivec2(newPosition);
    if(texelFetch(cellType, newCell, 0).r == SOLID)
    {
        newPosition = position;
    }
    return newPosition;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id > numOfParticles) return;

    Particle particle = particles[id];
    vec2 position = particle.position;
    vec2 velocity = vec2(sampleU(position), sampleV(position));
    vec2 newPosition = forwardPosition(position, velocity);
    particle[id].position = newPosition;
}