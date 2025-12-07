#version 430 

layout (binding = 0) uniform sampler2D pressureIn;
layout (binding = 1) uniform sampler2D divergence;

layout (binding = 2) uniform usampler2D cellType;

layout (binding = 3) uniform writeonly image2D pressureOut;

uniform float dx;
uniform ivec2 gridSize;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

float P(int i, int j) {return texelFetch(pressureIn, ivec2(i, j), 0).r;}
uint typeAt(int i, int j){return texelFetch(cellType, ivec2(i, j), 0).r;}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return;

    int i = id.x;
    int j = id.y;

    float pressureL, pressureR, pressureT, pressureB;

    float pressureC = texelFetch(pressureIn, id, 0).r;

    pressureL = (i-1 < 0 || typeAt(i-1,j) == SOLID) ? pressureC : P(i-1, j);  
    pressureR = (i+1 >= gridSize.x || typeAt(i+1,j) == SOLID) ? pressureC : P(i+1, j);  
    pressureB = (j-1 < 0 || typeAt(i,j-1) == SOLID) ? pressureC : P(i, j-1);  
    pressureT = (j+1 >= gridSize.y || typeAt(i,j+1) == SOLID) ? pressureC : P(i, j+1);  

    float div = texelFetch(divergence, ivec2(i,j),0).r;
    float newPressure = (pressureL + pressureR + pressureB + pressureT - dx*dx*div) * 0.25;

    imageStore(pressureOut, id, vec4(newPressure));
}