#version 430
layout(local_size_x = 256) in;

struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    float pad0;
};

struct Fluid
{
    float fluidDensity; // kg
    float volume; // m^3
    float stiffnesK;// nazewnictwo luźno powiązane z prędkością dźwięku 
    float stiffnesY;
};

layout(std430, binding = 0) buffer PartBuffer {
    FluidParticle particles[];
};

layout(std430, binding = 1) buffer FluidBuffer {
    Fluid fluid;
};

layout(std430, binding = 2) buffer HashHeadBuffer {
    int hashHead[];
};

layout(std430, binding = 3) buffer NextNodeBuffer {
    int nextNode[];
};


uint hash(ivec3 cell) {
    const uint p1 = 73856093u;
    const uint p2 = 19349663u;
    const uint p3 = 83492791u;
    return (cell.x * p1 ^ cell.y * p2 ^ cell.z * p3);
}

uniform uint numOfParticles;
uniform uint DIMENSION;
uniform uint tableSize;
uniform float sphKernelRadius;
uniform float cellSize;

float CubicSplineKernel(float kernelRadius, float distance, float alfa);

const uint DIMENSION_1 = 0u;
const uint DIMENSION_2 = 1u;
const uint DIMENSION_3 = 2u;

const float PI = 3.14159265359;

float getAlfa(float kernelRadius, uint dimension)
{
    float alfa;

    switch (dimension)
    {
         case DIMENSION_1:
            alfa = 2.0 / (3.0 * kernelRadius);
            break;
        case DIMENSION_2:
            alfa = 10.0 / (7.0 * PI * kernelRadius * kernelRadius);
            break;
        case DIMENSION_3:
        default:
            alfa = 1.0 / (PI * pow(kernelRadius, 3.0));
            break;
    }
    return alfa;
}

void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;

    float alfa = getAlfa(sphKernelRadius, DIMENSION);
    FluidParticle particle = particles[fluidParticle_id];
    vec3 position = particle.position.xyz;
    ivec3 cellPosition = ivec3(floor(position/cellSize));

    uint ourHashValue = hash(cellPosition) % tableSize;

    particle.density=0.0;
    ivec3 theirCellPosition = cellPosition;
    uint theirHashValue = ourHashValue;
    

    int dzMin = (DIMENSION == DIMENSION_2) ? 0 : -1;
    int dzMax = (DIMENSION == DIMENSION_2) ? 0 : 1;

    for (int dx=-1; dx <=1; dx++)
    {
        for (int dy=-1; dy <= 1; dy++)
        {
            for(int dz = dzMin; dz <=dzMax; dz++)  
            {
                theirCellPosition = cellPosition + ivec3(dx, dy, dz);
                theirHashValue = hash(theirCellPosition) % tableSize;

                if(hashHead[theirHashValue] != -1)
                {
                    int currentParticle = hashHead[theirHashValue];
                    do 
                    {
                        if(distance(particle.position.xyz, particles[currentParticle].position.xyz) <= 2*sphKernelRadius)
                        {
                            particle.density += particles[currentParticle].mass * CubicSplineKernel(sphKernelRadius, distance(particle.position.xyz, particles[currentParticle].position.xyz), alfa);       
                        }
                        if(nextNode[currentParticle]!=-1)
                        {
                            currentParticle = nextNode[currentParticle];
                        }
                    }while(nextNode[currentParticle] != -1);
                }
            }     
        }
    }
    particle.pressure = (fluid.stiffnesK*fluid.fluidDensity)/fluid.stiffnesY*(pow((particle.density/fluid.fluidDensity),fluid.stiffnesY)-1.0);// -;
    particles[fluidParticle_id].pressure = particle.pressure;
    particles[fluidParticle_id].density = particle.density;
}

float CubicSplineKernel(float kernelRadius, float distance, float alfa)
{
    float q = distance/kernelRadius;
    float retVal = 0.0;
    if (q >= 0.0 && q < 1.0)
        retVal = 1.0 - 1.5*q*q + 0.75*q*q*q;
    else if (q >= 1.0 && q < 2.0)
        retVal = 0.25 * pow(2.0 - q, 3.0);
    return alfa*retVal;
}
