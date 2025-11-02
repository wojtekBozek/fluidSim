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
    float stiffnesK;// nazewnictwo luźno powiązane z prędkością dźwięku 
    float stiffnesY;
};

layout(std430, binding = 0) buffer PartBuffer {
    FluidParticle particles[];
};

layout(std430, binding = 1) buffer FluidBuffer {
    Fluid fluid;
};

uniform uint numOfParticles;
uniform uint DIMENSION;
uniform float sphKernelRadius;

float CubicSplineKernel(float kernelRadius, float distance, float alfa);

const uint DIMENSION_1 = 0u;
const uint DIMENSION_2 = 1u;
const uint DIMENSION_3 = 2u;

const float PI = 3.14159265359;

float getAlfa(float kernelRadius, uint dimension)
{
    float alfa;

    switch (dimension)
    {
         case DIMENSION_1:
            alfa = 2.0 / (3.0 * kernelRadius);
            break;
        case DIMENSION_2:
            alfa = 10.0 / (7.0 * PI * kernelRadius * kernelRadius);
            break;
        case DIMENSION_3:
        default:
            alfa = 1.0 / (PI * pow(kernelRadius, 3.0));
            break;
    }
    return alfa;
}

void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;

    float alfa = getAlfa(sphKernelRadius, DIMENSION);
    FluidParticle particle = particles[fluidParticle_id];
    particle.density=0.0;
    for (uint i=0; i < numOfParticles; i++)
    {
        if(distance(particle.position.xyz, particles[i].position.xyz) <= 2*sphKernelRadius)
        {
            particle.density += particles[i].mass * CubicSplineKernel(sphKernelRadius, distance(particle.position.xyz, particles[i].position.xyz), alfa);       
        }
    }
    particle.pressure = (fluid.stiffnesK*fluid.fluidDensity)/fluid.stiffnesY*(pow((particle.density/fluid.fluidDensity),fluid.stiffnesY)-1.0);// -;
    particles[fluidParticle_id].pressure = particle.pressure;
    particles[fluidParticle_id].density = particle.density;
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
