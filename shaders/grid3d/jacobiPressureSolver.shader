#version 430 

layout(local_size_x=16, local_size_y=16, local_size_z=16) in;
layout(binding = 0) uniform sampler3D pressureIn;
layout(binding = 1) uniform sampler3D divergence;

layout(binding = 2) uniform usampler3D cellType;

layout(binding = 3, r32f) uniform writeonly image3D pressureOut;

uniform float dx;
uniform float dt;
uniform float dens;
uniform ivec3 gridSize;
uniform float overrelaxation = 1.0;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

float P(int i, int j, int k) {return texelFetch(pressureIn, ivec3(i, j, k), 0).r;}
uint typeAt(int i, int j, int k){if(i<0 || i>= gridSize.x || j <0 || j>= gridSize.y || k<0 || k>=gridSize.z) return SOLID; return texelFetch(cellType, ivec3(i, j, k), 0).r;}

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x || id.y >= gridSize.y || id.z>=gridSize.z) return;

    int i = id.x;
    int j = id.y;
    int k = id.z;

    if(typeAt(i,j,k) != FLUID) {imageStore(pressureOut, id, vec4(0.0)); return;}

    float sum = 0.0;
    int count = 0;
//
    // Left
    if (typeAt(i-1,j,k) == FLUID) { sum += P(i-1,j,k); count++; } else if(typeAt(i-1,j,k) == AIR) { sum += 0.0; count++; }  
    // Right
    if (typeAt(i+1,j,k) == FLUID) { sum += P(i+1,j,k); count++; } else if(typeAt(i+1,j,k) == AIR) { sum += 0.0; count++; }  
    // Bottom
    if (typeAt(i,j-1,k) == FLUID) { sum += P(i,j-1,k); count++; } else if(typeAt(i,j-1,k) == AIR) { sum += 0.0; count++; }  
    // Top
    if (typeAt(i,j+1,k) == FLUID) { sum += P(i,j+1,k); count++; } else if(typeAt(i,j+1,k) == AIR) { sum += 0.0; count++; }  
    if (typeAt(i,j,k-1) == FLUID) { sum += P(i,j,k-1); count++; } else if(typeAt(i,j,k-1) == AIR) { sum += 0.0; count++; }  
    // Top
    if (typeAt(i,j,k+1) == FLUID) { sum += P(i,j,k+1); count++; } else if(typeAt(i,j,k+1) == AIR) { sum += 0.0; count++; }  
    float div = texelFetch(divergence, id, 0).r;
    // Free-surface Poisson update
    float newPressure = (sum - dens*dx*div/dt) / (float(max(count,1)));
    float oldPressure = P(i,j,k);
    //newPressure = mix(oldPressure, newPressure, overrelaxation);
    newPressure = oldPressure + (newPressure-oldPressure)*overrelaxation;
    imageStore(pressureOut, id, vec4(newPressure));
}