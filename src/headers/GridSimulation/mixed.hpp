#pragma once
#include <GridSimulation/grid2d.hpp>

class ParticleInCell2D : public Grid2D
{
    public:
        void run() override;
	    void setup() override;
        void setTextures() override;
        void setShaders() override;
        void restart() override;
    private:
	    std::unique_ptr<ShaderProgram> m_TransferVelocityToGridShader;
	    std::unique_ptr<ShaderProgram> m_calculateVelocityOfParticles;

};