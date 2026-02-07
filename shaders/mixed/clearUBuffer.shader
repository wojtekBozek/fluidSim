#version 430 

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer velocities {
    float velocitiesU[];
};

layout(std430, binding = 1) buffer weights {
    float weightsU[];
};

uniform ivec2 gridSize;

int Nx(){return gridSize.x;}
int Ny(){return gridSize.y;}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= (Ny())*(Nx()+1)) return;

    weightsU[id] = 0.0;
    velocitiesU[id] = 0.0;
}