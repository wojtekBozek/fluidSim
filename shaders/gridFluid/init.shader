#version 430

layout (local_size_x = 8, local_size_y = 8) in;

layout (binging = 0, rgba32f) uniform writeonly image2D uTex;
layout (binging = 1, rgba32f) uniform writeonly image2D vTex;
layout (binging = 2, rgba32f) uniform writeonly image2D pressure;
layout (binging = 3, rgba32f) uniform writeonly image2D divergence;
layout (binging = 4, rgba32f) uniform writeonly image2D density;

uniform ivec2 gridSize;