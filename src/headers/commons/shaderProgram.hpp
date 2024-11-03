#pragma once
#include "shader.hpp"
#include <vector>
class ShaderProgram
{
public:
    ShaderProgram();
    bool linkProgram();
    bool addShader(GLuint shaderType, const std::string& shaderPath);
    int getProgramId();
    void useProgram();
    void dropProgram();
    int getUniformLocation(const std::string& uniformName);
    ~ShaderProgram();
private:
    std::vector<Shader> m_shaders;
    const GLuint m_programID;
};