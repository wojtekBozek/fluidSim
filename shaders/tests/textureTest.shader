#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D outTex;

uniform float time = 1.0;
void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(outTex);

    float x = float(id.x) / size.x;
    float y =  float(id.y) / size.y;

    vec4 color = time * vec4(x, y, 0.5, 1.0);
    imageStore(outTex, id, color);
}