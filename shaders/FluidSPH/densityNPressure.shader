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

float CubicSplineKernel(uint dimension, float kernelRadius, float distance);

const uint DIMENSION_1 = 0u;
const uint DIMENSION_2 = 1u;
const uint DIMENSION_3 = 2u;

const float PI = 3.14159265359;
void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;
//
    FluidParticle particle = particles[fluidParticle_id];
    particle.density=0.0;
    for (uint i=0; i < numOfParticles; i++)
    {
        particle.density += particles[i].mass * CubicSplineKernel(DIMENSION, sphKernelRadius, distance(particle.position.xyz, particles[i].position.xyz));       
    }
    particle.pressure = fluid.soundSpeed*fluid.soundSpeed*(particle.density-fluid.fluidDensity);
    particles[fluidParticle_id] = particle;
}

float CubicSplineKernel(uint dimension, float kernelRadius, float distance)
{
    float alfa;

    switch (dimension)
    {
        case DIMENSION_1: alfa = 1.0/kernelRadius;break;
        case DIMENSION_2: alfa = 15.0/(7.0*PI*kernelRadius*kernelRadius);break;
        case DIMENSION_3: alfa = 3.0/(2.0*PI*pow(kernelRadius,3));break;
        default: alfa = 3.0/(2.0*PI*pow(kernelRadius,3));break;
    }

    float q = distance/kernelRadius;
    float retVal = 0.0;

    if (q >= 0.0 && q <1.0)
    {
        retVal = 1.5 - q*q + 0.5*pow(q,3);
    }
    else if(q >= 1.0 && q<2.0)
    {
        retVal = (1.0/6.0)*pow((2.0-q),3);
    } // else 0, obsłużone z defaultu
    return alfa*retVal;
}
