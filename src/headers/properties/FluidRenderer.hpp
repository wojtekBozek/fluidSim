#pragma once
#include "FluidSPHSimulation.hpp"
#include "renderer.hpp"

class SPHSimulationRenderer : public BaseRenderer
{
    public:
    private:
    FluidSPHSimulation simulation;
    GLuint m_SSBO;
}; 