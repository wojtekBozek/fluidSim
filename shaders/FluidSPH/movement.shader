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
uniform float timeStep;

void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;

    FluidParticle particle= particles[fluidParticle_id];
    if(particle.type == 0)
    {
        particles[fluidParticle_id].position.xyz += particle.velocity.xyz*timeStep;
        particles[fluidParticle_id].velocity.xyz += particle.acceleration.xyz*timeStep;
    }
}