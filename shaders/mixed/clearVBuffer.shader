#version 430 

layout(local_size_x = 256) in;

layout(std430, binding = 1) buffer velocities {
    float velocitiesV[];
};

layout(std430, binding = 2) buffer weights {
    float weightsV[];
};

uniform ivec2 gridSize;

int Nx(){return gridSize.x;}
int Ny(){return gridSize.y;}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= Ny()*(Nx()-1)) return;

    weightsV[id] = 0.0;
    velocitiesV[id] = 0.0;
}