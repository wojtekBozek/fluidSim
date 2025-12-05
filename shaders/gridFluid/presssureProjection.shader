#version 430

layout (binding = 0, rgba32f) uniform image2D uTex;
layout (binding = 1, rgba32f) uniform image2D vTex;

layout (binding = 2) uniform sampler2D pressure;
layout (binding = 3) uniform usampler2D cellType;

uniform float dt;
uniform float dx;
uniform ivec2 gridSize;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);


    //right-left projection
    //bottom-top projection



}