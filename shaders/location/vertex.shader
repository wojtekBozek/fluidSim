#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


out vec3 FragPos;
out vec3 Normal;


uniform mat4 MVP;


void main() {
FragPos = vec3(MVP * vec4(aPos, 1.0));
Normal = mat3(transpose(inverse(MVP))) * aNormal;
gl_Position = MVP * vec4(FragPos, 1.0);
}