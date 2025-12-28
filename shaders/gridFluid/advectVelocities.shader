//This shader advects velocities on the grid faces 

#version 430

layout(local_size_x=16, local_size_y=16) in;
layout(binding = 0) uniform sampler2D uTex;
layout(binding = 1) uniform sampler2D vTex;
layout(binding = 2) uniform usampler2D cellType;

layout(r32f, binding = 3) uniform writeonly image2D uOut;
layout(r32f, binding = 4) uniform writeonly image2D vOut;

uniform ivec2 gridSize;
uniform float dt;
uniform float dx;

const int Nx = gridSize.x; 
const int Ny = gridSize.y;
// functions for interpolating values of missing velocity component (v for velocity on u face and u for velocity on v face) from neighboring perpendicular faces 
// we assume that i and j check for border faces have been dealt on earlier as update of velocity on those faces is not needed since it will still b zero by bc.
float interpolateUonVFace(int i, int j)
{
    int i0 = clamp(i,     0, Nx);
    int i1 = clamp(i + 1, 0, Nx);

    int j0 = clamp(j - 1, 0, Ny - 1);
    int j1 = clamp(j,     0, Ny - 1);

    float leftUp = texelFetch(uTex, ivec2(i0,j1),0).r;
    float leftDown = texelFetch(uTex, ivec2(i0,j0),0).r;
    float rightUp = texelFetch(uTex, ivec2(i1,j1),0).r;
    float rightDown = texelFetch(uTex, ivec2(i1,j0),0).r;
    return (leftUp+leftDown+rightUp+rightDown)/4.0;
}

float interpolateVonUFace(int i, int j)
{
    int i0 = clamp(i-1,     0, Nx-1);
    int i1 = clamp(i + 1, 0, Nx-1);

    int j0 = clamp(j, 0, Ny);
    int j1 = clamp(j+1,     0, Ny);
    float leftUp = texelFetch(vTex, ivec2(i0,j1),0).r;
    float leftDown = texelFetch(vTex, ivec2(i0,j0),0).r;
    float rightUp = texelFetch(vTex, ivec2(i1,j1),0).r;
    float rightDown = texelFetch(vTex, ivec2(i1,j0),0).r;
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
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int(position.y/dx);
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

    float value = 0.0;
    float sumW = 0.0;
    if(!uBlocked(i,j))
    {
        value += texelFetch(uTex, ivec2(i,j),0).r * w00 * w10;
        sumW += w00 * w10;
    }
    if(!uBlocked(i,j+1))
    {
        value += texelFetch(uTex, ivec2(i,j+1),0).r * w00 * w11;
        sumW += w00 * w11;
    }
    if(!uBlocked(i+1,j+1))
    {
        value += texelFetch(uTex, ivec2(i+1,j+1),0).r * w01 * w11;
        sumW += w01 * w11;
    }
    if(!uBlocked(i+1,j))
    {
        value += texelFetch(uTex, ivec2(i+1,j),0).r * w01 * w10;
        sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
}

float interpolateVinGrid(vec2 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    
    int j = int(position.y/dx);
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

    float value = 0.0;
    float sumW = 0.0;
    if(!vBlocked(i,j))
    {
        value += texelFetch(vTex, ivec2(i,j),0).r * w00 * w10;
        sumW += w00 * w10;
    }
    if(!vBlocked(i,j+1))
    {
        value += texelFetch(vTex, ivec2(i,j+1),0).r * w00 * w11;
        sumW += w00 * w11;
    }
    if(!vBlocked(i+1,j+1))
    {
        value += texelFetch(vTex, ivec2(i+1,j+1),0).r * w01 * w11;
        sumW += w01 * w11;
    }
    if(!vBlocked(i+1,j))
    {
        value += texelFetch(vTex, ivec2(i+1,j),0).r * w01 * w10;
        sumW += w01 * w10;
    }

    return (sumW > 0.0) ? value/sumW : 0.0;
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

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return ;
    uint type = texelFetch(cellType, id, 0).r;

    //if(type == SOLID)
    //{
    //    return;
    //}
    int i = id.x;
    int j = id.y;

    if(i < gridSize.x + 1 && j < gridSize.y)
    {
        uint ltype = (i - 1 < 0 || i-1>= gridSize.x) ? SOLID : texelFetch(cellType, ivec2(i-1,j), 0).r;
        uint rtype = (i  < 0 || i >= gridSize.x) ? SOLID : texelFetch(cellType, ivec2(i,j), 0).r;
        //if(!(ltype == AIR && rtype == AIR) && !(ltype == AIR && rtype == SOLID) && !(ltype == SOLID && rtype == AIR) )
        if(ltype == FLUID || rtype == FLUID)// && !(ltype == AIR && rtype == SOLID) && !(ltype == SOLID && rtype == AIR) )
        {
            vec2 positionU = vec2(i*dx+0.5*dx, j*dx);
            vec2 velocityOnU = vec2(texelFetch(uTex, ivec2(i,j), 0).r, interpolateVonUFace(i,j));
            vec2 backtracedPosition = backTracePositionRK2(positionU, velocityOnU);
            float u = sampleU(backtracedPosition);
            imageStore(uOut, id, vec4(u));
        }
    }
    if(i < gridSize.x && j < gridSize.y + 1)
    {
        uint btype = (j-1 < 0 || j-1 >= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j-1), 0).r;
        uint ttype = (j< 0 || j>= gridSize.y) ? SOLID : texelFetch(cellType, ivec2(i,j), 0).r;
        //if(!(btype == AIR && ttype == AIR) && !(btype == AIR && ttype == SOLID) && !(btype == SOLID && ttype == AIR) )
        if(btype == FLUID || ttype == FLUID)
        {
            vec2 positionV = vec2(i*dx, j*dx+0.5*dx);
            vec2 velocityOnV = vec2(interpolateUonVFace(i,j), texelFetch(vTex, ivec2(i,j), 0).r);
            vec2 backtracedPosition = backTracePositionRK2(positionV, velocityOnV);
            float v = sampleV(backtracedPosition);
            imageStore(vOut, id, vec4(v));
        }
    }
}