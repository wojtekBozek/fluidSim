#version 430
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
};

layout(std430, binding = 0) buffer PosBuffer {
    FluidParticle particles[];
};

uniform uint numOfParticles;
uniform float sphKernelRadius;
uniform uint DIMENSION;

uniform Fluid fluid;

float CubicSplineKernel(uint dimension, float kernelRadius, float distance);


void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID;
    if(fluidParticle_id > numOfParticles) return;

    FluidParticle particle= particles[fluidParticle_id];
    for (int i=0; i< numOfParticles; ++i)
    {
        particle.density += particles[i].mass * CubicSplineKernel(DIMENSION_3, sphKernelRadius, distance(particle.position, particles[i].position));       
    }
    particle.pressure = fluid.soundSpeed*fluid.soundSpeed*(particle.density-fluid.fluidDensity);
}

float CubicSplineKernel(uint dimension, float kernelRadius, float distance)
{
    float alfa;

    switch (dimension)
    {
        case DIMENSION_1: alfa = 1.0f/kernelRadius;
        case DIMENSION_2: alfa = 15.0f/(7*M_PI*kernelRadius*kernelRadius);
        case DIMENSION_3: alfa = return 3.0f/(2*M_PI*std::pow(kernelRadius,3));
        default: alfa = return 3.0f/(2*M_PI*std::pow(kernelRadius,3));
    }

    float q = distance/kernelRadius;
    float retVal = 0.0f;

    if (q >= 0.0f && q <1.0f)
    {
        retVal = 3/2.0f - q*q + 0.5f*std::pow(q,3);
    }
    else if(q >= 1.0f && q<2.0f)
    {
        retVal = 1/6.0f*std::pow((2-q),3);
    } // else 0, obsłużone z defaultu
    return alfa*retVal;
}
