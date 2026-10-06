#pragma once
#include "grid2d.hpp"

class ParticleInCell2D : public Grid2D
{
    public:
        ParticleInCell2D()
        {}
        void run() override;
	    void setup() override;
        void initilizeGrid() override;
        void setShaders() override;
        void restart() override;
        void setCopyTextures();
        float getPicFlipRatio() const {return picFlipRatio;}
        void setPicFlipRatio(float ratio) {picFlipRatio = ratio;}
    private:
        struct alignas(16) Particle
        {
            glm::vec2 position;
            glm::vec2 velocity;
            glm::mat2 apicMat = glm::mat2(0.0);
        };
        float picFlipRatio = 1.0;
        GLuint oldUTex, oldVTex;
	    std::unique_ptr<ShaderProgram> m_transferVelocityToGridShader;
	    std::unique_ptr<ShaderProgram> m_uToGridShader;
	    std::unique_ptr<ShaderProgram> m_vToGridShader;
        std::unique_ptr<ShaderProgram> m_clearUBuffer;
	    std::unique_ptr<ShaderProgram> m_clearVBuffer;
	    std::unique_ptr<ShaderProgram> m_calculateVelocityOfParticles;
        GLuint m_weightsUssbo, m_weightsVssbo, m_Ussbo, m_Vssbo;
};