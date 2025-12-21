#version 430
layout(local_size_x=16, local_size_y=16) in;
layout(r32f, binding=0) uniform image2D vTex;
layout(r32f, binding = 1) uniform writeonly image2D vOut;
uniform float vAccelerations;
uniform float dt;
uniform ivec2 gridSize;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);

    if(id.x >= gridSize.x || id.y >= gridSize.y + 1) return;

    float val = imageLoad(vTex, id).r;

    val += vAccelerations*dt;
    imageStore(vOut, id, vec4(val));
}