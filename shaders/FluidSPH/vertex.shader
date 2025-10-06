#version 430

struct FluidParticle{
    vec4 position;
    vec4 velocity;
    vec4 acceleration;
    float pressure;
    float density;
    float mass;
    float pad0;
};

layout(std430, binding = 0) buffer PosBuffer {
    FluidParticle particles[];
};


uniform mat4 view;
uniform mat4 proj;
uniform float viewportHeight;
uniform float fovy;
uniform float particleRadius;

void main() {
    uint particleID = gl_InstanceID;
    vec4 pos = particles[particleID].position;

    vec4 viewPos = view * pos;
    float z_eye = -viewPos.z; // positive distance from camera
    gl_PointSize = (particleRadius * viewportHeight) /
                   (z_eye * tan(fovy * 0.5));
    
    gl_Position = proj * viewPos;
    // clamp to reasonable range
    gl_PointSize = clamp(gl_PointSize, 1.0, 200.0);
}