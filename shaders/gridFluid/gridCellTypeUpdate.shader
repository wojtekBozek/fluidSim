#version 430

layout(local_size_x = 256) in;

layout(r8ui, binding = 0) uniform uimage2D grid;

uniform ivec2 gridSize;
uniform vec2 gridMin;
uniform vec2 gridMax;

struct Particle {
    vec2 position;
};

layout(std430, binding = 1) buffer Particles {
    Particle particles[];
};

void main()
{
    uint id = gl_GlobalInvocationID.x;
    vec2 pos = particles[id].position;

    vec2 uv = (pos - gridMin) / (gridMax - gridMin);
    ivec2 cell = ivec2(uv * vec2(gridSize));

    if (any(lessThan(cell, ivec2(0))) ||
        any(greaterThanEqual(cell, gridSize)))
        return;

    imageStore(grid, cell, uvec4(1)); 
}