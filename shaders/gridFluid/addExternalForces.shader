#version 430
layout(local_size_x=16, local_size_y=16) in;
layout(r32f, binding=0) image2D vTex;
layout(r32f, binding = 1) image2D writeonly vOut;
uniform float vAccelerations;
unfiorm float dt;
uniform uvec2 gridSize;

int main()
{
    vec2 id = gl_GlobalInvocationID.xy;

    if(id.x >= gridSize.x || id.y >= gridSize.y + 1) return;

    float val = texelFetch(vTex, id, 0).r;

    val += vAccelerations*dt;
    imageStore(vOut, id, vec4(val));
}