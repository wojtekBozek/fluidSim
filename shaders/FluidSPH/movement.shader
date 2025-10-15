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
//uniform float sphKernelRadius;
//uniform uint DIMENSION;
//uniform vec3 externalAccelerations;
//
//uniform vec3 domainRefPos;
//uniform vec3 domainDimennsions;
//uniform float boundaryMaxDist;
//
uniform float timeStep;
//uniform uint toonerP;
//uniform float stiffnessK = 100;//n/m


//const uint PI = 3.14159265359;
//
//float CubicSplineKernel(uint dimension, float kernelRadius, float distance);
//vec3 KernelGradient(uint dimension, float kernelRadius, vec3 distanceVector);
//vec3 applyDomainForces(vec3 position);
//float PressureByDensity2(FluidParticle part);
//vec3 reflectFromSurface(vec3 velVector, vec3 normalVector, float energyReflected); // istnieje reflect w GLSL, można skorzystać, jeśli nie będzie potrzeby utraty energi przy zderzeniu
void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;

    FluidParticle particle= particles[fluidParticle_id];
    //vec3 temp = {0.0,0.0,0.0};
    //float localPBD2 = PressureByDensity2(particle);
    //for (int i=0; i< numOfParticles; ++i)
    //{
    //    vec3 kernelGradient = KernelGradient(DIMENSION, sphKernelRadius, particle.position.xyz - particles[i].position.xyz);
    //    temp += particles[i].mass*(localPBD2+PressureByDensity2(particles[i]))*kernelGradient;
    //}
    //particle.acceleration.xyz = -temp + externalAccelerations;// + applyDomainForces(particle.position.xyz);
    //particle.position.xyz = particle.position.xyz + timeStep*particle.velocity.xyz;
    //particle.velocity.xyz = particle.velocity.xyz + timeStep*particle.acceleration.xyz;
    particles[fluidParticle_id].position.xyz += particle.velocity.xyz*timeStep;
    particles[fluidParticle_id].velocity.xyz += particle.acceleration.xyz*timeStep;
    //particles[fluidParticle_id].position.xyz = particles[fluidParticle_id].position.xyz + vec3(1.0, 1.0, 1.0);
} 

//float PressureByDensity2(FluidParticle part)
//{
//    return part.pressure/(part.density*part.density);
//}
//
//float CubicSplineKernel(uint dimension, float kernelRadius, float distance)
//{
//    float alfa;
//
//    switch (dimension)
//    {
//        case DIMENSION_1: alfa = 1.0/kernelRadius;
//        case DIMENSION_2: alfa = 15.0/(7.0*PI*kernelRadius*kernelRadius);
//        case DIMENSION_3: alfa = return 3.0/(2.0*PI*(kernelRadius*kernelRadius*kernelRadius));
//        default: alfa = return 3.0/(2.0*PI*(kernelRadius*kernelRadius*kernelRadius));
//    }
//
//    float q = distance/kernelRadius;
//    float retVal = 0.0;
//
//    if (q >= 0.0 && q <1.0)
//    {
//        retVal = 1.5 - q*q + 0.5*(q*q*q);
//    }
//    else if(q >= 1.0 && q<2.0)
//    {
//        retVal = (1.0/6.0)*((2-q)*(2-q)*(2-q));
//    } // else 0, obsłużone z defaultu
//    return alfa*retVal;
//}
//
//vec3 KernelGradient(uint dimension, float kernelRadius, vec3 distanceVector)
//{
//    return CubicSplineKernel(DIMENSION, kernelRadius, length(distanceVector)) * (distanceVector/length(distanceVector));
//}
//
//vec3 reflectFromSurface(vec3 velVector, vec3 normalVector, float energyReflected)
//{
//    return velVector - (1+energyReflected)*(dot(velVector, normalVector))*normalize(normalVector);
//}
//
//vec3 applyDomainForces(vec3 position)
//{
//    float back = domainRefPos.z;
//    vec3 backNormal = {0,0,1};
//    float front = domainRefPos + domainDimennsions.z;
//    vec3 frontNormal = {0,0,-1};
//    float up = domainRefPos.y + domainDimennsions.y;
//    vec3 upNormal = {0,-1,0};
//    float down = domainRefPos.y;
//    vec3 downNormal = {0,-1,0};
//    float right = domainRefPos.x + domainDimennsions.x;
//    vec3 rightNormal = {-1,0,0};
//    float left = domainRefPos.x;
//    vec3 leftNormal = {1,0,0};
//
//    vec3 domainForces = {0,0,0};
//    float localForce;
//    if(abs(position.z-back)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.z-back))/boundaryMaxDist;
//        }
//        domainForces += (localForce/particle.mass+externalAccelerations) * backNormal;
//    } 
//    else if(abs(position.z-front)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.z-front))/boundaryMaxDist;
//        }
//        domainForces +=  (localForce/particle.mass+externalAccelerations) * frontNormal;
//    }
//
//    if(abs(position.y-up)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.y-up))/boundaryMaxDist;
//        }
//        domainForces +=  (localForce/particle.mass+externalAccelerations) * upNormal;
//    } else if(abs(position.y-down)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.y-down))/boundaryMaxDist;
//        }
//        domainForces +=  (localForce/particle.mass+externalAccelerations) * downNormal;
//    }
//
//    if(abs(position.x-right)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.x-right))/boundaryMaxDist;
//        }
//        domainForces +=  (localForce/particle.mass+externalAccelerations) * rightNormal;
//    } else if(abs(position.x-left)<boundaryMaxDist)
//    {
//        localForce = stiffnessK;
//        for(int i = 0; i<toonerP; i++)
//        {
//            localForce *= (boundaryMaxDist - abs(position.x-left))/boundaryMaxDist;
//        }
//        domainForces +=  (localForce/particle.mass+externalAccelerations) * leftNormal;
//    }
//    return domainForces;
//}
//