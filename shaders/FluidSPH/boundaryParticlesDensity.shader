#version 430
layout(local_size_x = 512) in;

struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    uint type;
};

layout(std430, binding = 0) buffer PartBuffer {
    FluidParticle particles[];
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
    vec3 position = particle.position.xyz;
    float kernelSum = 0.0;
    float dist = 0.0;
    for (uint i=0; i < numOfParticles; i++)
    {
        dist = distance(particle.position.xyz, particles[i].position.xyz);
        if(dist <= 2*sphKernelRadius)
        {
           kernelSum += CubicSplineKernel(sphKernelRadius, dist, alfa);       
        }
    }
    particle.mass = particle.density/kernelSum;
    particles[fluidParticle_id].mass = particle.mass;
}

float CubicSplineKernel(float kernelRadius, float distance, float alfa)
{
    float q = distance/kernelRadius;
    float retVal = 0.0;
    if (q >= 0.0 && q < 1.0)
        retVal = 1.0 - 1.5*q*q + 0.75*q*q*q;
    else if (q >= 1.0 && q < 2.0)
        retVal = 0.25 * pow(2.0 - q, 3.0);
    return alfa*retVal;
}
