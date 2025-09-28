

#version 430

layout( local_size_x = 1000 ) in;
uniform float Gravity1 = 1000.0;
uniform vec3 BlackHolePos1;
uniform float Gravity2 = 1000.0;
uniform vec3 BlackHolePos2;

uniform float ParticleInvMass = 1.0/0.1;

uniform float DeltaTime = 0.0005;
layout(std430, binding = 0) buffer Pos {
    vec4 positions[];
};

layout(std430, binding = 1) buffer Vel {
    vec4 velocities[];
};

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    vec3 pos = positions[idx].xyz;
    vec3 vel = velocities[idx].xyz;
    vec3 dir1 = BlackHolePos1 - pos;
    vec3 dir2 = BlackHolePos2 - pos;

    vec3 force = (Gravity1 / length(dir1)) * normalize(dir1) + (Gravity2 / length(dir2)) * normalize(dir2);
    vec3 acc = force * ParticleInvMass;

    positions[idx] = vec4(pos + vel * DeltaTime + 0.5 * acc * DeltaTime * DeltaTime, 1.0);
    velocities[idx] = vec4(vel + acc * DeltaTime, 0.0);
}