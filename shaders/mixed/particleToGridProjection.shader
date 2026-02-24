#version 450 core
#extension GL_NV_shader_atomic_float : enable
layout(local_size_x = 256) in;

layout(binding = 0) uniform usampler2D cellType;

struct Particle {
    vec2 position;
    vec2 velocity;
    mat2 apicMat;
};

layout(std430, binding = 1) buffer Particles {
    Particle particles[];
};

layout(std430, binding = 2) buffer VelocitiesU {
    float velocitiesU[];
};

layout(std430, binding = 3) buffer VelocitiesV {
    float velocitiesV[];
};

layout(std430, binding = 4) buffer WeightsU {
    float weightsU[];
};

layout(std430, binding = 5) buffer WeightsV {
    float weightsV[];
};

uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform uint numOfParticles;

int Nx(){return gridSize.x;}
int Ny(){return gridSize.y;}


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

void particlesToFacesU(vec2 position, vec2 velocity, mat2 Cp)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int((position.y-0.5*dx)/dx);
    float downDistance = ((position.y-0.5*dx) - j*dx);

    int i0 = clamp(i,0, Nx());
    int i1 = clamp(i + 1, 0, Nx());

    int j0 = clamp(j, 0, Ny()-1);
    int j1 = clamp(j+1,     0, Ny()-1);
    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 
    vec2 distance = vec2(0.0);
    vec2 velocityPosition = vec2(0.0);

    if(!uBlocked(i0,j0))
    {
        float sumW = w00 * w10;
        velocityPosition = vec2(i0*dx, j0*dx + 0.5*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesU[j0*(Nx()+1)+i0], sumW*computed.x);
        atomicAdd(weightsU[j0*(Nx()+1)+i0], sumW);
    }
    if(!uBlocked(i0,j1))
    {
        float sumW = w00 * w11;
        velocityPosition = vec2(i0*dx, j1*dx + 0.5*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesU[j1*(Nx()+1)+i0], sumW*computed.x);
        atomicAdd(weightsU[j1*(Nx()+1)+i0], sumW);
    }
    if(!uBlocked(i1,j1))
    {
        float sumW = w01 * w11;
        velocityPosition = vec2(i1*dx, j1*dx + 0.5*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesU[j1*(Nx()+1)+i1], sumW*computed.x);
        atomicAdd(weightsU[j1*(Nx()+1)+i1], sumW);
    }
    if(!uBlocked(i1,j0))
    {
        float sumW = w01 * w10;
        velocityPosition = vec2(i1*dx, j0*dx + 0.5*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesU[j0*(Nx()+1)+i1], sumW*computed.x);
        atomicAdd(weightsU[j0*(Nx()+1)+i1], sumW);
    }
}

void particlesToFacesV(vec2 position, vec2 velocity, mat2 Cp)
{ 
    int i = int((position.x-0.5*dx)/dx);
    float backDistance = (position.x-0.5*dx) - i*dx;
    
    int j = int(position.y/dx);
    float downDistance = position.y - j*dx;

    int i0 = clamp(i,0, Nx()-1);
    int i1 = clamp(i + 1, 0, Nx()-1);

    int j0 = clamp(j, 0, Ny());
    int j1 = clamp(j+1,     0, Ny());
    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 
    vec2 distance = vec2(0.0);
    vec2 velocityPosition = vec2(0.0);

    if(!vBlocked(i0,j0))
    {
        float sumW = w00 * w10;
        velocityPosition = vec2(i0*dx + 0.5*dx, j0*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesV[j0*(Nx())+i0], sumW*computed.y);
        atomicAdd(weightsV[j0*(Nx())+i0], sumW);
    }
    if(!vBlocked(i0,j1))
    {
        float sumW = w00 * w11;
        velocityPosition = vec2(i0*dx + 0.5*dx, j1*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesV[j1*(Nx())+i0], sumW*computed.y);
        atomicAdd(weightsV[j1*(Nx())+i0], sumW);
    }
    if(!vBlocked(i1,j1))
    {
        float sumW = w01 * w11;
        velocityPosition = vec2(i1*dx + 0.5*dx, j1*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesV[j1*(Nx())+i1], sumW*computed.y);
        atomicAdd(weightsV[j1*(Nx())+i1], sumW);
    }
    if(!vBlocked(i1,j0))
    {
        float sumW = w01 * w10;
        velocityPosition = vec2(i1*dx + 0.5*dx, j0*dx);
        distance = velocityPosition - position;
        vec2 computed = velocity + Cp * distance;
        atomicAdd(velocitiesV[j0*(Nx())+i1], sumW*computed.y);
        atomicAdd(weightsV[j0*(Nx())+i1], sumW);
    }
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;

    vec2 position = particles[id].position;
    vec2 velocity = particles[id].velocity;
    mat2 Cp = particles[id].apicMat;
    particlesToFacesU(position,velocity,Cp);
    particlesToFacesV(position,velocity,Cp);
}