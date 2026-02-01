#version 430 

layout(local_size_x = 256) in;

layout(binding = 0) uniform usampler2D cellType;


struct Particle {
    vec2 position;
    vec2 velocity;
};

layout(std430, binding = 1) buffer Particles {
    Particle particles[];
};

layout(std430, binding = 2) buffer Particles {
    float velocitiesU[];
};

layout(std430, binding = 3) buffer Particles {
    float velocitiesV[];
};

layout(std430, binding = 4) buffer Particles {
    float weightsU[];
};

layout(std430, binding = 5) buffer Particles {
    float weightsV[];
};

layout(binding = 2) uniform sampler2D uTex;


uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform uint numOfParticles;

int Nx(){gridSize.x;}
int Ny(){gridSize.y;}


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
           checkCellType(ivec2(i,   j)) == SOLID || (checkCellType(ivec2(i-1, j)) != FLUID && checkCellType(ivec2(i, j)) != FLUID);
}

bool vBlocked(int i, int j)
{
    if(0 == j) return true;
    return checkCellType(ivec2(i, j-1)) == SOLID ||
           checkCellType(ivec2(i, j  )) == SOLID || (checkCellType(ivec2(i, j)) != FLUID && checkCellType(ivec2(i, j-1)) != FLUID);
}

void particlesToFacesU(vec2 position, vec2 velocity)
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

    if(!uBlocked(i0,j0))
    {
        float sumW = w00 * w10;
        atomicAdd(velocitiesU[j0*(Nx())+i0], sumW*velocity.x);
        atomicAdd(weightsU[j0*(Nx())+i0], sumW);
    }
    if(!uBlocked(i0,j1))
    {
        float sumW = w00 * w11;
        atomicAdd(velocitiesU[j1*(Nx())+i0], sumW*velocity.x);
        atomicAdd(weightsU[j1*(Nx())+i0], sumW);
    }
    if(!uBlocked(i1,j1))
    {
        float sumW = w01 * w11;
        atomicAdd(velocitiesU[j1*(Nx())+i1], sumW*velocity.x);
        atomicAdd(weightsU[j1*(Nx())+i1], sumW);
    }
    if(!uBlocked(i1,j0))
    {
        float sumW = w01 * w10;
        atomicAdd(velocitiesU[j0*(Nx())+i1], sumW*velocity.x);
        atomicAdd(weightsU[j0*(Nx())+i1], sumW);
    }
}

void particlesToFacesV(vec2 position)
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

    if(!vBlocked(i0,j0))
    {
        float sumW = w00 * w10;
        atomicAdd(velocitiesV[j0*(Nx()-1)+i0], sumW*velocity.y);
        atomicAdd(weightsV[j0*(Nx()-1)+i0], sumW);
    }
    if(!vBlocked(i0,j1))
    {
        float sumW = w00 * w11;
        atomicAdd(velocitiesV[j1*(Nx()-1)+i0], sumW*velocity.y);
        atomicAdd(weightsV[j1*(Nx()-1)+i0], sumW);
    }
    if(!vBlocked(i1,j1))
    {
        float sumW = w01 * w11;
        atomicAdd(velocitiesV[j1*(Nx()-1)+i1], sumW*velocity.y);
        atomicAdd(weightsV[j1*(Nx()-1)+i1], sumW);
    }
    if(!vBlocked(i1,j0))
    {
        float sumW = w01 * w10;
        atomicAdd(velocitiesV[j0*(Nx()-1)+i1], sumW*velocity.y);
        atomicAdd(weightsV[j0*(Nx()-1)+i1], sumW);
    }
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;

    vec2 position = particles[id].position;
    vec2 velocity = particles[id].velocity;
    particlesToFacesU(position,velocity);
    particlesToFacesV(position,velocity);
}