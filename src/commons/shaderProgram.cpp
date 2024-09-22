#include "headers/commons/shaderProgram.hpp"
#include <iostream>
ShaderProgram::ShaderProgram() : m_programID(glCreateProgram())
{}

bool ShaderProgram::linkProgram()
{
    glLinkProgram(m_programID);

    // Check the program for link errors
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
    glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0) {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(m_programID, infoLogLength, NULL, programErrorMessage.data());
        std::cerr << "Program link error: " << programErrorMessage.data() << std::endl;
        return false;
    }

    if(!result)
    {
        std::cerr << "Program link error of unknown origin!" << std::endl;
        return false;
    }

    // Detach and delete shaders after linking
    for (auto& shader : m_shaders) {
        shader.detachShader(m_programID);
        // Shader destructor will automatically delete the shader
    }

    m_shaders.clear(); // Clear the list of shaders as they are no longer needed
    return true;
}

bool ShaderProgram::addShader(GLuint shaderType, const std::string &shaderPath)
{
    Shader shader(shaderType);

    if(!shader.loadShader(shaderPath))
    {
        std::cerr << "Error loading shader from: " << shaderPath << "\n";
        return false;
    }

    if(!shader.compileShader())
    {
        std::cerr << "Error linking shader from: " << shaderPath << "\n";
        return false;
    }

    shader.attachShader(m_programID);
    m_shaders.push_back(std::move(shader));

    return true;
}

ShaderProgram::~ShaderProgram()
{
    if (m_programID != 0) 
    {
        glDeleteProgram(m_programID);
    }
}
