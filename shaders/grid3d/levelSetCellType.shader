#version 430
//Zmienić na adwekcję pola prędkości meh
layout(local_size_x=16, local_size_y=16);

layout(r32f, binding = 0) uniform sampler2D quantity;
layout(binding = 1) uniform image2D cellType;

uniform gridSize;


const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = gl_GlobalInvocationID.xy;
    
    if(id.x > gridSize.x || id.y > gridSize.y) return;

    uint type = texelFetch(cellType, id, 0).r;
    if(type == SOLID) return;
    float quant = texelFetch(quantity, id, 0).r;
    if(quant > 0.0)
    {
        imageStore(cellType, id, vec4(FLUID));
    } 
    else
    {
        imageStore(cellType, id, vec4(AIR));
    }
}