#version 430

layout (location = 0) in float dummy;


struct Particle {
    vec2 position;
    vec2 velocity;
};

layout(std430, binding = 0) buffer PosBuffer {
    Particle particles[];
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


void main() {
    
    uint particleID = uint(gl_InstanceID);
    if (particleID >= numOfParticles)
    {
        gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
        gl_PointSize = 0.0;
        return;
    }
    vec2 pos = particles[particleID].position; 
    vec4 viewPos = view * vec4(pos,0.0,1.0);
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