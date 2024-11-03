#version 330
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

out vec4 vertColor;
void  main()
{
    vertColor = vec4(inColor, 1.0);
    gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);
}