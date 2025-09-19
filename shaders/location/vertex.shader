#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


uniform mat4 MVP;


out vec3 FragPos;
out vec3 Normal;


void main() { 
vec3 Pos = vec3(MVP * vec4(aPos, 1.0));
Normal = mat3(transpose(inverse(MVP))) * aNormal;
gl_Position = MVP * vec4(aPos, 1.0);
FragPos = Pos;
}