#include <GL/glew.h>
#include <string>

// Function to convert OpenGL shader type to human-readable string

namespace debugOpenGL
{
    std::string getShaderTypeName(GLuint shaderType) {
        switch(shaderType) {
            case GL_VERTEX_SHADER: return "Vertex Shader";
            case GL_FRAGMENT_SHADER: return "Fragment Shader";
            case GL_GEOMETRY_SHADER: return "Geometry Shader";
            case GL_COMPUTE_SHADER: return "Compute Shader";
            case GL_TESS_CONTROL_SHADER: return "Tessellation Control Shader";
            case GL_TESS_EVALUATION_SHADER: return "Tessellation Evaluation Shader";
            default: return "Unknown Shader Type";
        }
    }
}