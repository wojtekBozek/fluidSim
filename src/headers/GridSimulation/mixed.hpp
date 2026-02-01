#pragma once
#include <GridSimulation/grid2d.hpp>

class ParticleInCell2D : public Grid2D
{
    public:
        ParticleInCell2D()
        {}
        void run() override;
	    void setup() override;
        void initialize();
        void setShaders() override;
        void restart() override;
    private:

        struct Particle
        {
            glm::vec2 position;
            glm::vec2 velocity;
        };
	    std::unique_ptr<ShaderProgram> m_transferVelocityToGridShader;
	    std::unique_ptr<ShaderProgram> m_calculateVelocityOfParticles;
        GLuint weightsUssbo, weightsVssbo, Ussbo, Vssbo;
};