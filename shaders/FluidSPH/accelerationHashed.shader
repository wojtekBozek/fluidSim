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
    float soundSpeed;// nazewnictwo luźno powiązane z prędkością dźwięku 
    float pad0;
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

uniform uint tableSize;
uniform uint numOfParticles;
uniform float sphKernelRadius;
uniform uint DIMENSION;
uniform vec3 externalAccelerations;

uniform vec3 domainRefPos;
uniform vec3 domainDimennsions;
uniform float boundaryMaxDist;

//uniform float timeStep;
uniform uint toonerP;
uniform float stiffnessK = 100.0;//n/m

uniform float epsilon;
uniform float cellSize;

const float PI = 3.14159265359;
const uint DIMENSION_1 = 0u;
const uint DIMENSION_2 = 1u;
const uint DIMENSION_3 = 2u;

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

float CubicSplineKernel(float kernelRadius, float distance, float alfa);
vec3 KernelGradient(float kernel, vec3 distanceVector, float alfa);
vec3 applyDomainForces(vec3 position, FluidParticle particle);
float PressureByDensity2(FluidParticle part);


void main()
{
    uint fluidParticle_id = gl_GlobalInvocationID.x;
    if(fluidParticle_id >= numOfParticles) return;
    float alfa = getAlfa(sphKernelRadius, DIMENSION);
    FluidParticle particle= particles[fluidParticle_id];
    vec3 temp = vec3(0.0,0.0,0.0);
    float localPBD2 = PressureByDensity2(particle);
    float kernel = 0.0;

    vec3 position = particle.position.xyz;
    ivec3 cellPosition = ivec3(floor(position/cellSize));

    uint ourHashValue = hash(cellPosition) % tableSize;

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
                            kernel = CubicSplineKernel(sphKernelRadius, distance(particle.position.xyz, particles[currentParticle].position.xyz), alfa);
                            vec3 kernelGradient = KernelGradient(sphKernelRadius, particle.position.xyz - particles[currentParticle].position.xyz, alfa);
                            temp += particles[currentParticle].mass*(localPBD2+PressureByDensity2(particles[currentParticle]))*kernelGradient;
                            particle.velocity.xyz += epsilon*particles[currentParticle].mass/particles[currentParticle].density*(particles[currentParticle].velocity.xyz-particle.velocity.xyz)*kernel;       
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
    
    particle.acceleration.xyz = externalAccelerations - temp + applyDomainForces(particle.position.xyz, particle);
    particles[fluidParticle_id].acceleration.xyz = particle.acceleration.xyz;
    particles[fluidParticle_id].velocity.xyz = particle.velocity.xyz;
} 

float PressureByDensity2(FluidParticle part)
{
    return part.pressure/(part.density*part.density);
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

vec3 KernelGradient(float kernelRadius, vec3 distanceVector, float alfa)
{
    float r = length(distanceVector);
    if (r < 0.00001) return vec3(0.0);

    float q = r / kernelRadius;
    float grad_q = 0.0;

    if (q >= 0.0 && q < 1.0)
        grad_q = (-3.0*q + 2.25*q*q);
    else if (q >= 1.0 && q < 2.0)
        grad_q = -0.75 * pow(2.0 - q, 2.0);

    float gradW = alfa * grad_q / kernelRadius;
    return gradW * (distanceVector / r);
}

float WallForce(float dist,float mass)
{
    if(dist < boundaryMaxDist)
    {
        return stiffnessK *pow((boundaryMaxDist - dist)/boundaryMaxDist, toonerP)/mass;
    } else
    {
        return 0.0;
    }
}

vec3 applyDomainForces(vec3 position, FluidParticle particle)
{
    float backSurface = domainRefPos.z;
    vec3 backNormal = vec3(0.0,0.0,1.0);
    float frontSurface = domainRefPos.z + domainDimennsions.z;
    vec3 frontNormal = vec3(0.0,0.0,-1.0);
    float up = domainRefPos.y + domainDimennsions.y;
    vec3 upNormal = vec3(0.0,-1.0,0.0);
    float down = domainRefPos.y;
    vec3 downNormal = vec3(0.0,1.0,0.0);
    float right = domainRefPos.x + domainDimennsions.x;
    vec3 rightNormal = vec3(-1.0,0.0,0.0);
    float left = domainRefPos.x;
    vec3 leftNormal = vec3(1.0,0.0,0.0);

    vec3 domainForces = vec3(0.0,0.0,0.0);
    domainForces += WallForce(abs(position.z-backSurface),particle.mass)*backNormal;
    domainForces += WallForce(abs(position.z-frontSurface),particle.mass)*frontNormal;
    domainForces += WallForce(abs(position.y-up),particle.mass)*upNormal;
    domainForces += WallForce(abs(position.y-down),particle.mass)*downNormal;
    domainForces += WallForce(abs(position.x-right),particle.mass)*rightNormal;
    domainForces += WallForce(abs(position.x-left),particle.mass)*leftNormal;
    return domainForces;
}
