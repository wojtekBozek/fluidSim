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
uniform vec3 externalForces;

uniform vec3 domainRefPos;
uniform vec3 domainDimennsions;

uniform float timeStep;

float CubicSplineKernel(uint dimension, float kernelRadius, float distance);
vec3 KernelGradient(uint dimension, float kernelRadius, vec3 distanceVector);
vec3 checkDomainConstrains(vec3 position, vec3 velocity);
float PressureByDensity2(FluidParticle part);
vec3 reflectFromSurface(vec3 velVector, vec3 normalVector, float energyReflected); // istnieje reflect w GLSL, można skorzystać, jeśli nie będzie potrzeby utraty energi przy zderzeniu
void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID;
    if(fluidParticle_id > numOfParticles) return;

    FluidParticle particle= particles[fluidParticle_id];
    vec3 temp = {0.0,0.0,0.0};
    float localPBD2 = PressureByDensity2(particle);
    for (int i=0; i< numOfParticles; ++i)
    {
        vec3 kernelGradient = KernelGradient(DIMENSION, sphKernelRadius, particle.position - particles[i].position);
        temp += particles[i].mass*(localPBD2+PressureByDensity2(particles[i]))*kernelGradient;
    }
    particle.acceleration.xyz = -temp + externalForces;
    particle.velocity += particle.acceleration*timeStep;
   
} 

float PressureByDensity2(FluidParticle part)
{
    return part.pressure/(part.density*part.density);
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

vec3 reflectFromSurface(vec3 velVector, vec3 normalVector, float energyReflected)
{
    return velVector - (1+energyReflected)*(dot(velVector, normalVector))*normalize(normalVector);
}