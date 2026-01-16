#version 430

layout(local_size_x = 256) in;

layout(r32ui, binding = 0) uniform uimage3D grid;

uniform ivec3 gridSize;
uniform vec3 gridMin;
uniform vec3 gridMax;
uniform uint numOfParticles;
//struct Particle {
//    vec2 position;
//};

layout(std430, binding = 1) buffer Particles {
    vec3 particles[];
};

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;


void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;
    vec3 pos = particles[id];

    vec3 uv = (pos - gridMin) / (gridMax - gridMin);
    //ivec2 cell = ivec2(uv * vec2(gridSize));

    ivec3 cell = clamp(
        ivec3(floor(uv * vec3(gridSize))),
        ivec3(0),
        gridSize - 1
    );
    imageAtomicExchange(grid, cell, FLUID); 
}