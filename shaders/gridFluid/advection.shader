#version 430

layout(local_size_x=16, local_size_y=16);

layout(rgba32f, binding = 0) uniform sampler2D uTex;
layout(rgba32f, binding = 1) uniform sampler2D vTex;

layout(rgba32f, binding = 2) uniform sampler2D quantity;
layout(binding = 3) uniform usampler2D cellType;

layout(rgba32f, binding = 4) uniform writeonly image2D quantityOut;

uniform float dx;
uniform float dt;

uniform ivec2 gridSize;

float sampleU(vec2 pos)
{
    vec2 uv = (pos + vec2(0.5, 0.0)) / vec2(gridSize.x+1, gridSize.y);
    return texture(uTex, uv).r;
}

float sampleV(vec2 pos)
{
    vec2 uv = (pos + vec2(0.5, 0.0)) / vec2(gridSize.x, gridSize.y+1);
    return texture(vTex, uv).r;
}

float sampleQuantity(vec2 pos)
{
    vec2 uv = (pos + vec2(0.5)) / vec2(size);
    return texture(quantity, uv).r;
}

const uint FLUID = 0u;
const uint AIR = 1u;
const uint SOLID = 2u;

void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    if(id.x >= gridSize.x || id.y >= gridSize.y) return ;

    uint type = texelFetch(cellType, id, 0).r;

    if(type == SOLID)
    {
        float q = texelFetch(quantity, id, 0).r;
        imageStore(quantityOut, id, vec4(q));
        return;
    }
    vec2 pos = vec2(id);
    float u = sampleU(pos);
    float v = sampleV(pos);

    vec2 vel = vec2(u,v);
    vec2 prev = pos - dt * vel/dx;

    prev = clamp(prev, vec2(0.0), vec2(size) - 1.001);

    ivec2 prevCell = ivec2(prev);
    if(texelFetch(cellType, prevCell, 0).r == SOLID)
    {
        prev = pos;
    }

    float result = sampleQuantity(prev);
    imageStore(quantityOut, id, vec4(result));
}