#include "commons/shader.hpp"
#include "commons/debugOpengl.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

Shader::Shader(GLuint shaderType) 
    : m_shaderID(glCreateShader(shaderType))  
{
    if (m_shaderID == 0) {
        std::cerr << "Failed to create shader of type: (type id: " << shaderType << ") " << debugOpenGL::getShaderTypeName(shaderType) << '\n';
    }
}

bool Shader::loadShader(const std::string& filePath)
{
    std::ifstream shaderStream(filePath, std::ios::in);
    if (!shaderStream.is_open()) 
    {
        std::cerr << "Failed to open shader file: " << filePath << '\n';
        return false;
    }

    std::stringstream sstr;
    sstr << shaderStream.rdbuf();
    m_shaderCode = sstr.str();
    shaderStream.close();

    return true;
}

bool Shader::compileShader()
{
    const char* sourcePointer = m_shaderCode.c_str();
    glShaderSource(m_shaderID, 1, &sourcePointer, nullptr);
    glCompileShader(m_shaderID);

    GLint result = GL_FALSE;
    int infoLogLength = 0;
    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0) 
    {
        std::vector<char> shaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(m_shaderID, infoLogLength, nullptr, shaderErrorMessage.data());
        std::cerr << "Error compiling shader:\n" << shaderErrorMessage.data() << '\n';
        return false;
    }

    if (result == GL_FALSE) 
    {
        std::cerr << "Shader compilation failed without additional info.\n";
        return false;
    }

    return true;
}

void Shader::attachShader(GLuint programID)
{
    glAttachShader(programID, m_shaderID);
}

void Shader::detachShader(GLuint programID)
{
    glDetachShader(programID, m_shaderID);
}

Shader::~Shader()
{
    if (m_shaderID != 0) {
        glDeleteShader(m_shaderID);
    }
}
