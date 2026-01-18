#version 430 core
layout(location = 0) in vec3 aPos;
out vec3 uv;
void main() {
    uv = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 1.0);
}