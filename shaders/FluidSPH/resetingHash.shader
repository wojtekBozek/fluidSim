#version 430
layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer HashHeadBuffer {
    int hashHead[];
};

layout(std430, binding = 1) buffer NextNodeBuffer {
    int nextNode[];
};

uniform uint tableSize;
uniform uint numOfParticles;

void main()
{
    uint hash_id = gl_GlobalInvocationID.x;
    if(hash_id > tableSize) return;

    hashHead[hash_id] = -1;
    if(hash_id < numOfParticles)
    {
        nextNode[hash_id] = -1;
    }
}