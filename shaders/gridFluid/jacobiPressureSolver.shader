#version 430 

layout(local_size_x=16, local_size_y=16) in;
layout(binding = 0) uniform sampler2D pressureIn;
layout(binding = 1) uniform sampler2D divergence;

layout(binding = 2) uniform usampler2D cellType;

layout(binding = 3, r32f) uniform writeonly image2D pressureOut;

uniform float dx;
uniform float dt;
uniform ivec2 gridSize;
//uniform float density; 
const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

float P(int i, int j) {return texelFetch(pressureIn, ivec2(i, j), 0).r;}
uint typeAt(int i, int j){if(i<0 || i>= gridSize.x || j <0 || j>= gridSize.y) return SOLID; return texelFetch(cellType, ivec2(i, j), 0).r;}

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return;

    int i = id.x;
    int j = id.y;

    //float pressureL, pressureR, pressureT, pressureB;
    //float pressureC = texelFetch(pressureIn, id, 0).r;
    if(typeAt(i,j) != FLUID) {imageStore(pressureOut, id, vec4(0.0)); return;}
    //pressureL = (i-1 < 0 || typeAt(i-1,j) == SOLID) ? pressureC : P(i-1, j);  
    //pressureR = (i+1 >= gridSize.x || typeAt(i+1,j) == SOLID) ? pressureC : P(i+1, j);  
    //pressureB = (j-1 < 0 || typeAt(i,j-1) == SOLID) ? pressureC : P(i, j-1);  
    //pressureT = (j+1 >= gridSize.y || typeAt(i,j+1) == SOLID) ? pressureC : P(i, j+1);  
//
    //float div = texelFetch(divergence, ivec2(i,j),0).r;
    //float newPressure = (pressureL + pressureR + pressureB + pressureT - dx*dx*div) * 0.25;

    float sum = 0.0;
    int count = 0;
//
    // Left
    if (typeAt(i-1,j) != SOLID) { sum += P(i-1,j); count++; }  
    // Right
    if (typeAt(i+1,j) != SOLID) { sum += P(i+1,j); count++; }
    // Bottom
    if (typeAt(i,j-1) != SOLID) { sum += P(i,j-1); count++; }
    // Top
    if (typeAt(i,j+1) != SOLID) { sum += P(i,j+1); count++; }
//
    float div = texelFetch(divergence, id, 0).r;
//
    // Free-surface Poisson update
    float newPressure = (sum - dx*dx*div) / float(max(count,1));

    newPressure = max(newPressure, 0.0);

    imageStore(pressureOut, id, vec4(newPressure));
}