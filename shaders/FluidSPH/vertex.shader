#version 430

struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    uint type;
};

layout (location = 0) in float dummy;

layout(std430, binding = 0) buffer PosBuffer {
    FluidParticle particles[];
};

uniform uint numOfParticles;
uniform mat4 view;
uniform mat4 proj;
uniform float viewportHeight;
uniform float fovy;
uniform float top;
uniform float bottom;
uniform float particleRadius;
uniform bool perspectiveProjection;

flat out uint isFluid;

void main() {
    
    uint particleID = uint(gl_InstanceID);
    if (particleID >= numOfParticles)
    {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
        gl_PointSize = 0.0;
        return;
    }
    vec4 pos = particles[particleID].position; //particles[particleID].position;
    isFluid = particles[particleID].type;
    vec4 viewPos = view * pos;
    gl_Position = proj *viewPos;
    if(perspectiveProjection)
    {
        float z_eye = -viewPos.z; // positive distance from camera

        gl_PointSize = particleRadius * viewportHeight / (z_eye * tan(fovy/2.0));
    }
    else
    {  
        gl_PointSize = 2*particleRadius * viewportHeight / (top-bottom);
    }
    //gl_Position = proj * view * pos;
    //gl_PointSize = 10.0;
    //gl_PointSize = (particleRadius*2/z_eye) * (viewportHeight/
    //               (z_eye * tan(fovy)));
    // clamp to reasonable range
    //gl_PointSize = clamp(pixelSize, 1.0, 200.0);
}