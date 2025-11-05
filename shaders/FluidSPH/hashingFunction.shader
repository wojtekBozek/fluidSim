#version 430
layout(local_size_x = 256) in;

struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    float pad0;
};

layout(std430, binding = 0) buffer PartBuffer {
    FluidParticle particles[];
};

layout(std430, binding = 1) buffer HashHeadBuffer {
    int hashHead[];
};

layout(std430, binding = 2) buffer NextNodeBuffer {
    int nextNode[];
};

uint hash(ivec3 cell) {
    const uint p1 = 73856093u;
    const uint p2 = 19349663u;
    const uint p3 = 83492791u;
    return (cell.x * p1 ^ cell.y * p2 ^ cell.z * p3);
}

uniform uint numOfParticles;
uniform uint tableSize;

uniform float cellSize;

void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;

    vec3 position = particles[fluidParticle_id].position.xyz;
    ivec3 cellPosition = ivec3(floor(position/cellSize));

    uint hashValue = hash(cellPosition) % tableSize;
    int oldHead = atomicExchange(hashHead[hashValue], int(fluidParticle_id));
    nextNode[fluidParticle_id] = oldHead;
}
