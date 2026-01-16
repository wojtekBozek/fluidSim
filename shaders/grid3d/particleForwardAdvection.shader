#version 430 

layout(local_size_x = 256) in;

layout(binding = 0) uniform usampler3D cellType;


struct Particle {
    vec3 position;
};

layout(std430, binding = 1) buffer Particles {
    vec3 particles[];
};


layout(binding = 2) uniform sampler3D uTex;
layout(binding = 3) uniform sampler3D vTex; 
layout(binding = 4) uniform sampler3D wTex; 


uniform ivec3 gridSize;
uniform float dt;
uniform float dx;
uniform uint numOfParticles;

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
    return checkCellType(ivec2(i-1, j, k)) == SOLID ||
           checkCellType(ivec2(i,   j, k)) == SOLID;
}

bool vBlocked(int i, int j, int k)
{
    if(0 == j) return true;
    return checkCellType(ivec2(i, j-1, k)) == SOLID ||
           checkCellType(ivec2(i, j  , k)) == SOLID;
}

bool wBlocked(int i, int j, int k)
{
    if(0 == k) return true;
    return checkCellType(ivec2(i, j, k)) == SOLID ||
           checkCellType(ivec2(i, j, k-1)) == SOLID;
}

float interpolateWin3dGrid(vec3 position)
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
      value0 += texelFetch(wTex, ivec3(i1,j1,k0),0).r * w01 * w11
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
    value0 = sum0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sum1 > 0.0 ? value1/sumW1 : 0.0;
    float value = wX0*value0 + wX1*value1;
    return value;
}

float interpolateUin3dGrid(vec3 position)
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
    value0 = sum0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sum1 > 0.0 ? value1/sumW1 : 0.0;
    float value = wX0*value0 + wX1*value1;
    return value;
}

float interpolateVin3dGrid(vec3 position)
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
    if(!VBlocked(i0,j0,k0))
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
      value0 += texelFetch(vTex, ivec3(i1,j0,k1),0).r * w01 * w11
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
    value0 = sum0 > 0.0 ? value0/sumW0 : 0.0;
    value1 = sum1 > 0.0 ? value1/sumW1 : 0.0;
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
vec3 forwardRK2Position(vec3 position, vec3 velocity)
{
    vec3 halfPosition = position + dt*0.5*velocity;
    halfPosition = clampPosition(halfPosition);
    ivec3 halfCell = ivec3(halfPosition.x/dx, halfPosition.y/dx);
    if(halfCell.x < 0 || halfCell.x >= gridSize.x 
        || halfCell.y < 0 || halfCell.y >= gridSize.y 
        || halfCell.z < 0 || halfCell.z >= gridSize.z
        ||texelFetch(cellType, halfCell, 0).r == SOLID)
    {
        halfPosition = position;
    }

    vec3 halfVelocity = vec3(sampleU(halfPosition), sampleV(halfPosition), sampleW(halfPosition));
    vec3 newPosition = position + dt*halfVelocity;
    ivec2 newCell = ivec2(newPosition.x/dx, newPosition.y/dx);
    if(newCell.x < 0 || newCell.x >= gridSize.x 
        || newCell.y < 0 || newCell.y >= gridSize.y 
        || newCell.z < 0 || newCell.y >= gridSize.z 
        ||texelFetch(cellType, newCell, 0).r == SOLID)
    {
        newPosition = position;
    }
    return newPosition;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    if(id >= numOfParticles) return;

    vec3 position = particles[id];
    vec3 velocity = vec2(sampleU(position), sampleV(position), sampleW(position));
    vec3 newPosition = forwardRK2Position(position, velocity);
    particles[id] = newPosition;
}