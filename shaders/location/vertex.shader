#version 430
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


out vec3 FragPos;
out vec3 Normal;


void main() { 
mat4 MVP = projection * view * model;
vec3 Pos = vec3(MVP * vec4(aPos, 1.0));
Normal = mat3(transpose(inverse(model))) * aNormal;
gl_Position = MVP * vec4(aPos, 1.0);
FragPos = Pos;
}