#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{
    public:
        Shader(GLuint shaderType);
        bool loadShader(const std::string& filePath);
        bool compileShader();
        void attachShader(GLuint programID);
        void detachShader(GLuint programID);
        ~Shader();
    private:
        std::string m_shaderCode;
        const GLuint m_shaderID;
};