#version 430 

layout(local_size_x = 256) in;

layout(std430, binding = 1) buffer velocities {
    float velocitiesU[];
};

layout(std430, binding = 2) buffer weights {
    float weightsU[];
};

layout(r32f, binding = 0) uniform image2D writeonly uTex;


uniform ivec2 gridSize;

int Nx(){return gridSize.x;}
int Ny(){return gridSize.y;}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= (Ny()-1)*Nx()) return;

    uint j = id/Nx();
    uint i = id%Nx();

    float weight = weightsU[id];
    float vel = velocitiesU[id];
    float value = weight > 0.0 ? vel/weight : 0.0;
    imageStore(uTex, ivec2(i,j), vec4(value));
}