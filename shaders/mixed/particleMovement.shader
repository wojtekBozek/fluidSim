#version 430 

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

layout(binding = 2) uniform sampler2D uNewTex;
layout(binding = 3) uniform sampler2D vNewTex;

layout(binding = 4) uniform sampler2D uOldTex;
layout(binding = 5) uniform sampler2D vOldTex; 


uniform ivec2 gridSize;
uniform float dt;
uniform float dx;
uniform float picFlipAlpha = 1.0;
uniform uint numOfParticles;

int Nx(){ return gridSize.x;}
int Ny(){ return gridSize.y;}


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

float interpolateUinGrid(vec2 position, sampler2D uTex)
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

float interpolateVinGrid(vec2 position, sampler2D vTex)
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

float sampleV(vec2 position, sampler2D vTex)
{
    return interpolateVinGrid(position, vTex);
}

float sampleU(vec2 position, sampler2D uTex)
{
    return interpolateUinGrid(position, uTex);
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

vec2 computeFlipVelocity(vec2 position, vec2 oldParticleVelocity)
{
    vec2 flipVelocity = oldParticleVelocity + (vec2(sampleU(position, uNewTex), sampleV(position, vNewTex)) - vec2(sampleU(position, uOldTex), sampleV(position, vOldTex)));
    return flipVelocity;
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

    vec2 halfVelocity = vec2(sampleU(halfPosition, uNewTex), sampleV(halfPosition, vNewTex));
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

struct ApicData
{
    vec2 velocity;
    mat2 B;
    float sumW;
};

ApicData computeAPICFromUFaces(vec2 position)
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

    vec2 velocityPosition = vec2(0.0); 
    vec2 totalVelocity = vec2(0.0);
    mat2 B = mat2(0.0);
    float sumW = 0.0;
    float uVelocity = 0.0;
    float vVelocity = 0.0;
    vec2 localVelocity = vec2(0.0);
    vec2 distance = vec2(0.0);
    //if(!uBlocked(i0,j0))
    //{
        velocityPosition = vec2(i0*dx, j0*dx+0.5*dx);
        uVelocity = texelFetch(uNewTex, ivec2(i0,j0),0).r;
        vVelocity = interpolateVinGrid(velocityPosition, vNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w00 * w10;
        B += w00 * w10 * outerProduct(localVelocity, distance);
        sumW += w00 * w10;
    //}
    //if(!uBlocked(i0,j1))
    //{
        velocityPosition = vec2(i0*dx, j1*dx+0.5*dx);
        uVelocity = texelFetch(uNewTex, ivec2(i0,j1),0).r;
        vVelocity = interpolateVinGrid(velocityPosition, vNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w00 * w11;
        B += w00 * w11 * outerProduct(localVelocity, distance);
        sumW += w00 * w11;
    //}
    //if(!uBlocked(i1,j1))
    //{
        velocityPosition = vec2(i1*dx, j1*dx+0.5*dx);
        uVelocity = texelFetch(uNewTex, ivec2(i1,j1),0).r;
        vVelocity = interpolateVinGrid(velocityPosition, vNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w01 * w11;
        B += w01 * w11 * outerProduct(localVelocity, distance);
        sumW += w01 * w11;
    //}
    //if(!uBlocked(i1,j0))
    //{
        velocityPosition = vec2(i1*dx, j0*dx+0.5*dx);
        uVelocity = texelFetch(uNewTex, ivec2(i1,j0),0).r;
        vVelocity = interpolateVinGrid(velocityPosition, vNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w01 * w10;
        B += w01 * w10 * outerProduct(localVelocity, distance);
        sumW += w01 * w10;
    //}
    return ApicData(totalVelocity, B, sumW);
}

ApicData computeAPICFromVFaces(vec2 position)
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

    vec2 velocityPosition = vec2(0.0); 
    vec2 totalVelocity = vec2(0.0);
    mat2 B = mat2(0.0);
    float sumW = 0.0;
    float uVelocity = 0.0;
    float vVelocity = 0.0;
    vec2 localVelocity = vec2(0.0);
    vec2 distance = vec2(0.0);
    //if(!vBlocked(i0,j0))
    //{
        velocityPosition = vec2(i0*dx + 0.5*dx, j0*dx);
        vVelocity = texelFetch(vNewTex, ivec2(i0,j0),0).r;
        uVelocity = interpolateUinGrid(velocityPosition, uNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w00 * w10;
        B += w00 * w10 * outerProduct(localVelocity, distance);
        sumW += w00 * w10;
    //}
    //if(!vBlocked(i0,j1))
    //{
        velocityPosition = vec2(i0*dx+0.5*dx, j1*dx);
        vVelocity = texelFetch(vNewTex, ivec2(i0,j1),0).r;
        uVelocity = interpolateUinGrid(velocityPosition, uNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w00 * w11;
        B += w00 * w11 * outerProduct(localVelocity, distance);
        sumW += w00 * w11;
    //}
    //if(!vBlocked(i1,j1))
    //{
        velocityPosition = vec2(i1*dx+0.5*dx, j1*dx);
        vVelocity = texelFetch(vNewTex, ivec2(i1,j1),0).r;
        uVelocity = interpolateUinGrid(velocityPosition, uNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w01 * w11;
        B += w01 * w11 * outerProduct(localVelocity, distance);
        sumW += w01 * w11;
    //}
    //if(!vBlocked(i1,j0))
    //{
        velocityPosition = vec2(i1*dx+0.5*dx, j0*dx);
        vVelocity = texelFetch(vNewTex, ivec2(i1,j0),0).r;
        uVelocity = interpolateUinGrid(velocityPosition, uNewTex);
        localVelocity = vec2(uVelocity, vVelocity);
        distance = velocityPosition - position;
        totalVelocity += localVelocity * w01 * w10;
        B += w01 * w10 * outerProduct(localVelocity, distance);
        sumW += w01 * w10;
    //}
    return ApicData(totalVelocity, B, sumW);
}

Particle computeAPIC_N_PIC(vec2 position)
{
    ApicData uData = computeAPICFromUFaces(position);
    ApicData vData = computeAPICFromVFaces(position);

    uData.B = uData.sumW > 0.0 ? uData.B : mat2(0.0);
    vData.B = vData.sumW > 0.0 ? vData.B : mat2(0.0);
    mat2 B =  uData.B + vData.B;
    vec2 uVelocityComponent = uData.sumW > 0.0 ? uData.velocity/uData.sumW : vec2(0.0);
    vec2 vVelocityComponent = vData.sumW > 0.0 ? vData.velocity/vData.sumW : vec2(0.0);
    vec2 velocity = vec2(uVelocityComponent.x, vVelocityComponent.y);
    vec2 picVelocity = vec2(sampleU(position, uNewTex), sampleV(position, vNewTex));
    float alpha = clamp(picFlipAlpha, 0.0, 1.0);
    mat2 Cp = alpha*B*2/(dx*dx);
    velocity = alpha*velocity + (1.0-alpha)*picVelocity;
    vec2 newPosition = forwardRK2Position(position, velocity);
    return Particle(newPosition, velocity, Cp);
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;

    vec2 position = particles[id].position;
    //vec2 oldVelocity = particles[id].velocity;
    //vec2 flipVelocity = computeFlipVelocity(position, oldVelocity);
    //vec2 picVelocity = vec2(sampleU(position, uNewTex), sampleV(position, vNewTex));
    //float alpha = clamp(picFlipAlpha, 0.0, 1.0);
//
    //vec2 velocity = alpha * picVelocity + (1.0-alpha) * flipVelocity;
    //vec2 newPosition = forwardRK2Position(position, picVelocity);
    //particles[id] = Particle(newPosition, velocity, mat2(0.0));
    particles[id] = computeAPIC_N_PIC(position);
}