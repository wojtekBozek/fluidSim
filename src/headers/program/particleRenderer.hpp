#include "renderer.hpp"
#include "shaderProgram.hpp"
class ParticleRenderer : public BaseRenderer
{
public:
    ParticleRenderer() = default;
    void setupBackend();
    void render() override;
    void generateParticles(GLuint number, float minCoord, float maxCoord);
    void setShaderProgram(std::shared_ptr<ShaderProgram> shader)
    {
        shaderProgram = shader;
    }
    void setComputeShaderProgram(std::shared_ptr<ShaderProgram> shader)
    {
        computeShaderProgram = shader;
    }
private:
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ShaderProgram> computeShaderProgram;
    GLuint particlesNumber;

    std::vector<GLfloat> initPos;
    std::vector<GLfloat> initVel;

    GLuint posBuf;
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    GLuint velBuf;
};