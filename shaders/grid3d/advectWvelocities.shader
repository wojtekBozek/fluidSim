//This shader advects U velocities to the grid U faces 

#version 430

layout(local_size_x=8, local_size_y=8, local_size_z=8) in;
layout(binding = 0) uniform sampler3D uTex;
layout(binding = 1) uniform sampler3D vTex;
layout(binding = 2) uniform sampler3D wTex;
layout(binding = 3) uniform usampler3D cellType;

layout(r32f, binding = 4) uniform writeonly image3D wOut;

uniform ivec3 gridSize;
uniform float dt;
uniform float dx;

const int Nx = gridSize.x; 
const int Ny = gridSize.y;
const int Nz = gridSize.z;

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;


int checkCellType(ivec3 c)
{
    return int(texelFetch(cellType, c, 0).r);
}

bool uBlocked(int i, int j, int k)
{
    if(0 == i) return true;
    return checkCellType(ivec3(i-1, j, k)) == SOLID ||
           checkCellType(ivec3(i,   j, k)) == SOLID;
}

bool vBlocked(int i, int j, int k)
{
    if(0 == j) return true;
    return checkCellType(ivec3(i, j-1, k)) == SOLID ||
           checkCellType(ivec3(i, j  , k)) == SOLID;
}

bool wBlocked(int i, int j, int k)
{
    if(0 == k) return true;
    return checkCellType(ivec3(i, j, k)) == SOLID ||
           checkCellType(ivec3(i, j, k-1)) == SOLID;
}

float interpolateVonWFace(int i, int j, int k)
{
    int i0 = clamp(i, 0, Nx-1);

    int j0 = clamp(j, 0, Ny);
    int j1 = clamp(j+1, 0, Ny);

    int k0 = clamp(k-1, 0, Nz);
    int k1 = clamp(k, 0, Nz);

    float value = 0.0;
    float div = 0.0;
    if(!vBlocked(i0,j1,k0))
    {
      value += texelFetch(vTex, ivec3(i0,j1,k0),0).r;
      div += 1.0;
    }
    if(!vBlocked(i0,j0,k0))
    {
       value += texelFetch(vTex, ivec3(i0,j0,k0),0).r;
       div += 1.0;
    }
    if(!vBlocked(i0,j1,k1))
    {
      value += texelFetch(vTex, ivec3(i0,j1,k1),0).r;
      div += 1.0;
    }
    if(!vBlocked(i0, j0,k1))
    {
      value += texelFetch(vTex, ivec3(i0,j0,k1),0).r;
      div += 1.0;
    }
    return (div>0.0) ? value/div : 0.0;
}

float interpolateUonWFace(int i, int j, int k)
{
    int i0 = clamp(i, 0, Nx-1);
    int i1 = clamp(i + 1, 0, Nx-1);

    int j0 = clamp(j, 0, Ny);

    int k0 = clamp(k-1, 0, Nz);
    int k1 = clamp(k, 0, Nz);

    float value = 0.0;
    float div = 0.0;
    if(!vBlocked(i0,j0,k0))
    {
      value += texelFetch(wTex, ivec3(i0,j0,k0),0).r;
      div += 1.0;
    }
    if(!vBlocked(i0,j0,k0))
    {
       value += texelFetch(wTex, ivec3(i0,j0,k1),0).r;
       div += 1.0;
    }
    if(!vBlocked(i1,j0,k0))
    {
      value += texelFetch(wTex, ivec3(i1,j0,k0),0).r;
      div += 1.0;
    }
    if(!vBlocked(i1, j0,k0))
    {
      value += texelFetch(wTex, ivec3(i1,j0,k1),0).r;
      div += 1.0;
    }
    return (div>0.0) ? value/div : 0.0;
}

float interpolateWinGrid(vec3 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int(position.y/dx);
    float downDistance = (position.y - j*dx);
    int k = int(position.z/dx);
    float depthDistance = (position.z - k*dx);

    if(backDistance < dx/2.0)
    {
        i--;
        backDistance += dx/2.0;
    }

    if(downDistance < dx/2.0)
    {
        j--;
        downDistance += dx/2.0;
    }

    int i0 = clamp(i,0, Nx);
    int i1 = clamp(i + 1, 0, Nx);

    int k0 = clamp(k,0, Nz-1);
    int k1 = clamp(k + 1, 0, Nz-1);

    int j0 = clamp(j, 0, Ny-1);
    int j1 = clamp(j + 1, 0, Ny-1);

    float w00 = 1.0 - downDistance/dx; 
    float w01 = downDistance/dx; 
    float w10 = 1.0 - backDistance/dx;
    float w11 = backDistance/dx; 
    float wX0 = 1.0 - depthDistance/dx; 
    float wX1 = depthDistance/dx; 
          
          
    float value0 = 0.0;
    float sumW0 = 0.0;
    //k0 face
    if(!wBlocked(i0,j0,k0))
    {
      value0 += texelFetch(wTex, ivec3(i0,j0,k0),0).r * w00 * w10;
      sumW0 += w00 * w10;
    }
    if(!wBlocked(i0,j1,k0))
    {
      value0 += texelFetch(wTex, ivec3(i0,j1,k0),0).r * w00 * w11;
      sumW0 += w00 * w11;
    }

    if(!wBlocked(i1,j1,k0))
    {
      value0 += texelFetch(wTex, ivec3(i1,j1,k0),0).r * w01 * w11;
      sumW0 += w01 * w11;
    }
    if(!wBlocked(i1,j0,k0))
    {
      value0 += texelFetch(wTex, ivec3(i1,j0,k0),0).r * w01 * w10;
      sumW0 += w01 * w10;
    }

    float value1 = 0.0;
    float sumW1 = 0.0;
    //k1 face
    if(!wBlocked(i0,j0,k1))
    {
      value1 += texelFetch(wTex, ivec3(i0,j0,k1),0).r * w00 * w10;
      sumW1 += w00 * w10;
    }
    if(!wBlocked(i0,j1,k1))
    {
      value1 += texelFetch(wTex, ivec3(i0,j1,k1),0).r * w00 * w11;
      sumW1 += w00 * w11;
    }
    if(!wBlocked(i1,j0,k1))
    {
      value1 += texelFetch(wTex, ivec3(i1,j0,k1),0).r * w01 * w10;
      sumW1 += w01 * w10;
    }
    if(!wBlocked(i1,j1,k1))
    {
      value1 += texelFetch(wTex, ivec3(i1,j1,k1),0).r * w01 * w11;
      sumW1 += w01 * w11;
    }
    value0 = sumW0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sumW1 > 0.0 ? value1/sumW1 : 0.0;
    float value = wX0*value0 + wX1*value1;
    return value;
}

float interpolateUinGrid(vec3 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int(position.y/dx);
    float downDistance = (position.y - j*dx);
    int k = int(position.z/dx);
    float depthDistance = (position.z - k*dx);

    if(downDistance < dx/2.0)
    {
        j--;
        downDistance += dx/2.0;
    }

    if(depthDistance < dx/2.0)
    {
        k--;
        depthDistance += dx/2.0;
    }

    int i0 = clamp(i,0, Nx);
    int i1 = clamp(i + 1, 0, Nx);

    int k0 = clamp(k,0, Nz-1);
    int k1 = clamp(k + 1, 0, Nz-1);

    int j0 = clamp(j, 0, Ny-1);
    int j1 = clamp(j + 1, 0, Ny-1);

    float wX0 = 1.0 - backDistance/dx;
    float wX1 = backDistance/dx; 
    float w00 = 1.0 - depthDistance/dx; 
    float w01 = depthDistance/dx; 
    float w10 = 1.0 - downDistance/dx; 
    float w11 = downDistance/dx; 

    float value0 = 0.0;
    float sumW0 = 0.0;
    //i0 face
    if(!uBlocked(i0,j0,k0))
    {
      value0 += texelFetch(uTex, ivec3(i0,j0,k0),0).r * w00 * w10;
      sumW0 += w00 * w10;
    }
    if(!uBlocked(i0,j1,k0))
    {
      value0 += texelFetch(uTex, ivec3(i0,j1,k0),0).r * w00 * w11;
      sumW0 += w00 * w11;
    }

    if(!uBlocked(i0,j0,k1))
    {
      value0 += texelFetch(uTex, ivec3(i0,j0,k1),0).r * w01 * w10;
      sumW0 += w01 * w10;
    }
    if(!uBlocked(i0,j1,k1))
    {
      value0 += texelFetch(uTex, ivec3(i0,j1,k1),0).r * w01 * w11;
      sumW0 += w01 * w11;
    }

    float value1 = 0.0;
    float sumW1 = 0.0;
    //i1 face
    if(!uBlocked(i1,j1,k0))
    {
      value1 += texelFetch(uTex, ivec3(i1,j1,k0),0).r * w00 * w11;
      sumW1 += w00 * w11;
    }
    if(!uBlocked(i1,j0,k0))
    {
      value1 += texelFetch(uTex, ivec3(i1,j0,k0),0).r * w00 * w10;
      sumW1 += w00 * w10;
    }
    if(!uBlocked(i1,j0,k1))
    {
      value1 += texelFetch(uTex, ivec3(i1,j0,k1),0).r * w01 * w10;
      sumW1 += w01 * w10;
    }
    if(!uBlocked(i1,j1,k1))
    {
      value1 += texelFetch(uTex, ivec3(i1,j1,k1),0).r * w01 * w11;
      sumW1 += w01 * w11;
    }
    value0 = sumW0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sumW1 > 0.0 ? value1/sumW1 : 0.0;
    float value = wX0*value0 + wX1*value1;
    return value;
}

float interpolateVinGrid(vec3 position)
{ 
    int i = int(position.x/dx);
    float backDistance = position.x - i*dx;
    int j = int(position.y/dx);
    float downDistance = (position.y - j*dx);
    int k = int(position.z/dx);
    float depthDistance = (position.z - k*dx);

    if(backDistance < dx/2.0)
    {
        i--;
        backDistance += dx/2.0;
    }

    if(depthDistance < dx/2.0)
    {
        k--;
        depthDistance += dx/2.0;
    }

    int i0 = clamp(i,0, Nx);
    int i1 = clamp(i + 1, 0, Nx);

    int k0 = clamp(k,0, Nz-1);
    int k1 = clamp(k + 1, 0, Nz-1);

    int j0 = clamp(j, 0, Ny-1);
    int j1 = clamp(j + 1, 0, Ny-1);

    float wX0 = 1.0 - downDistance/dx; 
    float wX1 = downDistance/dx; 
    float w00 = 1.0 - backDistance/dx;
    float w01 = backDistance/dx; 
    float w10 = 1.0 - depthDistance/dx; 
    float w11 = depthDistance/dx; 

    float value0 = 0.0;
    float sumW0 = 0.0;
    //j0 face
    if(!vBlocked(i0,j0,k0))
    {
      value0 += texelFetch(vTex, ivec3(i0,j0,k0),0).r * w00 * w10;
      sumW0 += w00 * w10;
    }
    if(!vBlocked(i0,j0,k1))
    {
      value0 += texelFetch(vTex, ivec3(i0,j0,k1),0).r * w00 * w11;
      sumW0 += w00 * w11;
    }

    if(!vBlocked(i1,j0,k1))
    {
      value0 += texelFetch(vTex, ivec3(i1,j0,k1),0).r * w01 * w11;
      sumW0 += w01 * w11;
    }
    if(!vBlocked(i1,j0,k0))
    {
      value0 += texelFetch(vTex, ivec3(i1,j0,k0),0).r * w11 * w10;
      sumW0 += w01 * w10;
    }

    float value1 = 0.0;
    float sumW1 = 0.0;
    //j1 face
    if(!vBlocked(i0,j1,k0))
    {
      value1 += texelFetch(vTex, ivec3(i0,j1,k0),0).r * w00 * w10;
      sumW1 += w00 * w10;
    }
    if(!vBlocked(i0,j1,k0))
    {
      value1 += texelFetch(vTex, ivec3(i0,j1,k0),0).r * w00 * w10;
      sumW1 += w00 * w10;
    }
    if(!vBlocked(i1,j1,k1))
    {
      value1 += texelFetch(vTex, ivec3(i1,j1,k1),0).r * w01 * w10;
      sumW1 += w01 * w10;
    }
    if(!vBlocked(i1,j1,k1))
    {
      value1 += texelFetch(vTex, ivec3(i1,j1,k1),0).r * w01 * w11;
      sumW1 += w01 * w11;
    }
    value0 = sumW0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sumW1 > 0.0 ? value1/sumW1 : 0.0;
    float value = wX0*value0 + wX1*value1;
    return value;
}

float sampleV(vec3 position)
{
    return interpolateVinGrid(position);
}

float sampleU(vec3 position)
{
    return interpolateUinGrid(position);
}

float sampleW(vec3 position)
{
    return interpolateWinGrid(position);
}

vec3 clampPosition(vec3 position)
{
    vec3 minPos = vec3(0.0);
    vec3 maxPos = vec3(gridSize.x * dx, gridSize.y* dx, gridSize.z*dx);

    position = clamp(position, minPos, maxPos);
    return position;
}

vec3 backTracePositionRK2(vec3 position, vec3 velocity)
{
    vec3 halfPos = position - dt*velocity/2.0;
    halfPos = clampPosition(halfPos);
    vec3 halfVelocity = vec3(sampleU(halfPos), sampleV(halfPos), sampleW(halfPos));

    vec3 backtracedPosition = position - dt*halfVelocity;
    backtracedPosition = clampPosition(backtracedPosition);
    return backtracedPosition;
}

void main()
{
    ivec3 id = ivec3(gl_GlobalInvocationID.xyz);
    if(id.x >= gridSize.x || id.y >= gridSize.y || id.z >=gridSize.z+1) return ;
    int i = id.x;
    int j = id.y;
    int k = id.z;
    uint ltype = (k - 1 < 0 || k-1>= gridSize.x) ? SOLID : texelFetch(cellType, ivec3(i,j,k-1), 0).r;
    uint rtype = (k  < 0 || k >= gridSize.x) ? SOLID : texelFetch(cellType, ivec3(i,j,k), 0).r;
    if(ltype == FLUID || rtype == FLUID)
    {
        vec3 positionW = vec3(i*dx+0.5*dx, j*dx+0.5*dx, k*dx);
        vec3 velocityOnW = vec3(interpolateUonWFace(i,j,k), interpolateVonWFace(i,j,k), texelFetch(wTex, ivec3(i,j,k), 0).r);
        vec3 backtracedPosition = backTracePositionRK2(positionW, velocityOnW);
        float w = sampleW(backtracedPosition);
        imageStore(wOut, id, vec4(w));
    }
    else
        imageStore(wOut, id, vec4(0.0));
}