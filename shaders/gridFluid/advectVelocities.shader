//This shader advects velocities on the grid faces 

#version 430

layout (binding = 0) sampler2D uTex;
layout (binding = 1) sampler2D vTex;
layout(binding = 3) uniform usampler2D cellType;

layout(rgba32f, binding = 4) uniform writeonly image2D uOut;
layout(rgba32f, binding = 5) uniform writeonly image2D vOut;

uniform vec2 gridSize;
uniform float dt;
uniform float dx;

const int Nx = gridSize.x;
const int Ny = gridSize.y;
// functions for interpolating values of missing velocity component (v for velocity on u face and u for velocity on v face) from neighboring perpendicular faces 
// we assume that i and j check for border faces have been dealt on earlier as update of velocity on those faces is not needed since it will still b zero by bc.
float interpolateUonVFace(int i, int j)
{
    float leftUp = texelFetch(uTex, ivec2(i,j),0).r;
    float leftDown = texelFetch(uTex, ivec2(i,j-1),0).r;
    float rightUp = texelFetch(uTex, ivec2(i+1,j),0).r;
    float rightDown = texelFetch(uTex, ivec2(i+1,j-1),0).r;
    return (leftUp+leftDown+rightUp+rightDown)/4.0;
}

float interpolateVonUFace(int i, int j)
{
    float leftUp = texelFetch(vTex, ivec2(i-1,j+1),0).r;
    float leftDown = texelFetch(vTex, ivec2(i-1,j),0).r;
    float rightUp = texelFetch(vTex, ivec2(i,j+1),0).r;
    float rightDown = texelFetch(vTex, ivec2(i,j),0).r;
    return (leftUp+leftDown+rightUp+rightDown)/4.0;
}

int checkCellType(ivec2 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

bool uBlocked(int i, int j)
{
    if(0 == i) return true;
    return checkCellType(ivec2(i-1, j)) == SOLID ||
           checkCellType(ivec2(i,   j)) == SOLID;
}

bool vBlocked(int i, int j)
{
    if(0 == j) return true;
    return checkCellType(ivec2(i, j-1)) == SOLID ||
           checkCellType(ivec2(i, j  )) == SOLID;
}


vec2 clampU(vec2 p)
{
    return clamp(p,
        vec2(0.0, 0.5*dx),
        vec2(gridSize.x*dx, (gridSize.y-0.5)*dx));
}

vec2 clampV(vec2 p)
{
    return clamp(p,
        vec2(0.5*dx, 0.0),
        vec2((gridSize.x-0.5)*dx, gridSize.y*dx));
}


float interpolateUinGrid(vec2 position)
{ 
    int i = position.x/dx;
    float backDistance = position.x - i*dx;
    int j = position.y/dx;
    float downDistance = (position.y - j*dx);

    if(downDistance < dx/2.0)
    {
        j--;
        downDistance += dx/2.0;
    }

    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float leftDown = uBlocked(i,j) ? 0.0 : texelFetch(uTex, ivec2(i,j),0).r * w00 * w10;
    float leftUp = uBlocked(i,j+1) ? 0.0 : texelFetch(uTex, ivec2(i,j+1),0).r * w00 * w11;
    float rightUp = uBlocked(i+1,j+1) ? 0.0 : texelFetch(uTex, ivec2(i+1,j+1),0).r * w01 * w11;
    float rightDown = uBlocked(i+1,j) ? 0.0 : texelFetch(uTex, ivec2(i+1,j),0).r * w01 * w10;

    return (leftUp + leftDown + rightUp + rightDown);
}

float interpolateVinGrid(vec2 position)
{ 
    int i = position.x/dx;
    float backDistance = position.x - i*dx;
    
    int j = position.y/dx;
    float downDistance = position.y - j*dx;

    if(backDistance < dx/2.0)
    {
        i--;
        backDistance += dx/2.0;
    }

    float w00 = 1.0 - backDistance/dx; 
    float w01 = backDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float leftDown = vBlocked(i,j) ? 0.0 : texelFetch(vTex, ivec2(i,j),0).r * w00 * w10;
    float leftUp = vBlocked(i,j+1) ? 0.0 : texelFetch(vTex, ivec2(i,j+1),0).r * w00 * w11;
    float rightUp = vBlocked(i+1,j+1) ? 0.0 : texelFetch(vTex, ivec2(i+1,j+1),0).r * w01 * w11;
    float rightDown = vBlocked(i+1,j) ? 0.0 : texelFetch(vTex, ivec2(i+1,j),0).r * w01 * w10;

    return (leftUp + leftDown + rightUp + rightDown);
}

float sampleV(vec2 position)
{
    return interpolateVinGrid(clampV(position));
}

float sampleU(vec2 position)
{
    return interpolateUinGrid(clampU(position));
}

vec2 clampPosition(vec2 position)
{
    vec2 minPos = vec2(0.5 * dx);
    vec2 maxPos = vec2((gridSize.x - 1.5) * dx, (gridSize.y - 1.5) * dx);

    position = clamp(position, minPos, maxPos);
    return position;
}

vec2 backTracePositionRK2(vec2 position, vec2 velocity)
{
    vec2 halfPos = position - dt*velocity/2.0;
    halfPos = clampPosition(halfPos);
    vec2 halfVelocity = vec2(sampleU(halfPos), sampleV(halfPos));

    vec2 backtracedPosition = position - dt*halfVelocity;
    backtracedPosition = clampPosition(backtracedPosition);
    return backtracedPosition;
}

vec2 clampU(vec2 p)
{
    return clamp(p,
        vec2(0.0,        0.5*dx),
        vec2(Nx*dx, (Ny-0.5)*dx));
}

vec2 clampV(vec2 p)
{
    return clamp(p,
        vec2(0.5*dx, 0.0),
        vec2((Nx-0.5)*dx, Ny*dx));
}

void main()
{
    ivec2 id = gl_GlobalInvocationID.xy;

    int i = id.x;
    int j = id.y;
    if(i > 0 && i < gridSize.x && j < gridSize.y)
    {
        vec2 positionU = vec2(i*dx+0.5*dx, j*dx);
        vec2 velocityOnU = vec2(texelFetch(uTex, ivec2(i,j), 0).r, interpolateVonUFace(i,j));
        vec2 backtracedPosition = backTracePositionRK2(position, velocityOnU);
        float u = sampleU(backtracedPosition);
        imageStore(uOut, id, vec4(u));
    }
    if(i < gridSize.x && j < gridSize.y && j > 0)
    {
        vec2 positionV = vec2(i*dx, j*dx+0.5*dx);
        vec2 velocityOnV = vec2(interpolateUonUFace(i,j), texelFetch(vTex, ivec2(i,j), 0).r);
        vec2 backtracedPosition = backTracePositionRK2(position, velocityOnV);
        float v = sampleV(backtracedPosition);
        imageStore(vOut, id, vec4(v));
    }
}