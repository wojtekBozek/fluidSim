#version 430
layout(local_size_x = 1000) in;
struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    float pad0;
};

struct Fluid
{
    float fluidDensity; // kg
    float volume; // m^3
    float soundSpeed;// nazewnictwo luźno powiązane z prędkością dźwięku 
    float pad0;
};

layout(std430, binding = 0) buffer PartBuffer {
    FluidParticle particles[];
};

layout(std430, binding = 1) buffer FluidBuffer {
    Fluid fluid;
};

uniform uint numOfParticles;
uniform float sphKernelRadius;
uniform uint DIMENSION;
uniform vec3 externalAccelerations;

uniform vec3 domainRefPos;
uniform vec3 domainDimennsions;
uniform float boundaryMaxDist;

//uniform float timeStep;
uniform uint toonerP;
uniform float stiffnessK = 100.0;//n/m

uniform float epsilon;

const float PI = 3.14159265359;
const uint DIMENSION_1 = 0u;
const uint DIMENSION_2 = 1u;
const uint DIMENSION_3 = 2u;

float getAlfa(float kernelRadius, uint dimension)
{
    float alfa;

    switch (dimension)
    {
        case DIMENSION_1: alfa = 1.0/kernelRadius;break;
        case DIMENSION_2: alfa = 15.0/(7.0*PI*kernelRadius*kernelRadius);break;
        case DIMENSION_3: alfa = 3.0/(2.0*PI*pow(kernelRadius,3));break;
        default: alfa = 3.0/(2.0*PI*pow(kernelRadius,3));break;
    }
    return alfa;
}

float CubicSplineKernel(float kernelRadius, float distance, float alfa);
vec3 KernelGradient(float kernel, vec3 distanceVector, float alfa);
vec3 applyDomainForces(vec3 position, FluidParticle particle);
float PressureByDensity2(FluidParticle part);


void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;
    float alfa = getAlfa(sphKernelRadius, DIMENSION);
    FluidParticle particle= particles[fluidParticle_id];
    vec3 temp = vec3(0.0,0.0,0.0);
    float localPBD2 = PressureByDensity2(particle);
    float kernel = 0.0;
    for (uint i=0; i< numOfParticles; i++)
    {
        if(distance(particle.position.xyz,particles[i].position.xyz) <= 2*sphKernelRadius)
        {
            kernel = CubicSplineKernel(sphKernelRadius, distance(particle.position.xyz, particles[i].position.xyz), alfa);
            vec3 kernelGradient = KernelGradient(sphKernelRadius, particle.position.xyz - particles[i].position.xyz, alfa);
            temp += particles[i].mass*(localPBD2+PressureByDensity2(particles[i]))*kernelGradient;
            particle.velocity.xyz += epsilon*particles[i].mass/particles[i].density*(particles[i].velocity.xyz-particle.velocity.xyz)*kernel;
        }
    }
    particle.acceleration.xyz = externalAccelerations - temp;// + applyDomainForces(particle.position.xyz, particle);
    particles[fluidParticle_id].acceleration.xyz = particle.acceleration.xyz;
    particles[fluidParticle_id].velocity.xyz = particle.velocity.xyz;
} 

float PressureByDensity2(FluidParticle part)
{
    return part.pressure/(part.density*part.density);
}

float CubicSplineKernel(float kernelRadius, float distance, float alfa)
{
    float q = distance/kernelRadius;
    float retVal = 0.0;

    if (q >= 0.0 && q < 1.0)
    {
        retVal = 1.5 - q*q + 0.5*(q*q*q);
    }
    else if(q >= 1.0 && q < 2.0)
    {
        retVal = (1.0/6.0)*((2.0-q)*(2.0-q)*(2.0-q));
    }
    return alfa*retVal;
}

vec3 KernelGradient(float kernelRadius, vec3 distanceVector, float alfa)
{
    float r = length(distanceVector);
    if (r < 0.0001) return vec3(0.0);

    float q = r / kernelRadius;
    float grad_q = 0.0;

    if (q >= 0.0 && q < 1.0)
        grad_q = (-3.0*q + 2.25*q*q);
    else if (q >= 1.0 && q < 2.0)
        grad_q = -0.75 * pow(2.0 - q, 2.0);

    float gradW = alfa * grad_q / kernelRadius;
    return gradW * (distanceVector / r);
}

float WallForce(float dist,float mass)
{
    if(dist < boundaryMaxDist)
    {
        return stiffnessK *pow((boundaryMaxDist - dist)/boundaryMaxDist, toonerP)/mass;
    } else
    {
        return 0.0;
    }
}

vec3 applyDomainForces(vec3 position, FluidParticle particle)
{
    float backSurface = domainRefPos.z;
    vec3 backNormal = vec3(0.0,0.0,1.0);
    float frontSurface = domainRefPos.z + domainDimennsions.z;
    vec3 frontNormal = vec3(0.0,0.0,-1.0);
    float up = domainRefPos.y + domainDimennsions.y;
    vec3 upNormal = vec3(0.0,-1.0,0.0);
    float down = domainRefPos.y;
    vec3 downNormal = vec3(0.0,1.0,0.0);
    float right = domainRefPos.x + domainDimennsions.x;
    vec3 rightNormal = vec3(-1.0,0.0,0.0);
    float left = domainRefPos.x;
    vec3 leftNormal = vec3(1.0,0.0,0.0);

    vec3 domainForces = vec3(0.0,0.0,0.0);
    domainForces += WallForce(abs(position.z-backSurface),particle.mass)*backNormal;
    domainForces += WallForce(abs(position.z-frontSurface),particle.mass)*frontNormal;
    domainForces += WallForce(abs(position.y-up),particle.mass)*upNormal;
    domainForces += WallForce(abs(position.y-down),particle.mass)*downNormal;
    domainForces += WallForce(abs(position.x-right),particle.mass)*rightNormal;
    domainForces += WallForce(abs(position.x-left),particle.mass)*leftNormal;
    return domainForces;
}
