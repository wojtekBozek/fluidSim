#include "grid2d.hpp"
#include <iostream>

void Grid2D::run()
{
    /*
    0. calculate timestep based on max velocity
    1. LevelSet method instead of particle advection
    2. cell reconfiguration
    3. velocity advection
    4. External forces
    5. Pressure calculation (40-80 iterations)
    6. Pressure projection
    7. Boundary conditions (optional if not already applied)
    */



}

void Grid2D::setDimensions(uint32_t x, uint32_t y)
{   
    nx = x;
    ny = y;
}

void Grid2D::setCellSize(double x)
{
    dx = x;
}

const std::vector<glm::vec2>& Grid2D::getParticles() const
{
    return particles;
}

void Grid2D::initilizeGrid()
{
    /*
    m_initShader->useProgram();

    glBindImageTexture(0, CellTypeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
    glBindImageTexture(1, uInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(2, vInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(3, pressureInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(4, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    m_initShader->setInt2("gridSize", nx, ny);
    glDispatchCompute((nx+15) / 16, (ny + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/
    
    std::vector<GLubyte> type(nx * ny, 0);
    for (uint32_t y = 0; y < ny; ++y)
    {
        for (uint32_t x = nx; x < nx; ++x)
        {
            if (x >= initFluidX && x < initFluidX + initFluidWidth && y >= initFluidY && y < initFluidY + initFluidHeight)
            {
                type[y * nx + x] = FLUID;
                particles.push_back(glm::vec2(0.5 * dx + x * dx, 0.5 * dx + y * dx));
            }
            else
            {
                type[y * nx + x] = AIR;
            }
        }
    }
    std::cout << "Num of Particles: " << type.size() << ".\n";
    glBindTexture(GL_TEXTURE_2D, CellTypeTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_UNSIGNED_BYTE, type.data());

    GLuint bufsize = sizeof(particles);
    if (glIsBuffer(m_particleBuffer) == GL_FALSE)
    {
        glGenBuffers(1, &m_particleBuffer);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, &m_particleBuffer, GL_DYNAMIC_DRAW);
}

void Grid2D::setTextures()
{
    glGenTextures(1, &uInTex);
    glBindTexture(GL_TEXTURE_2D, uInTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx + 1, ny);
    std::vector<GLfloat> zeros((nx + 1) * ny, 0.0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx+1, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &uOutTex);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx + 1, ny);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx + 1, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &vInTex);
    glBindTexture(GL_TEXTURE_2D, vInTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny+1);
    zeros = std::vector<GLfloat>(nx * (ny + 1), 0.0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny + 1, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &vOutTex);
    glBindTexture(GL_TEXTURE_2D, vOutTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny+1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny + 1, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &pressureInTex);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
    zeros = std::vector<GLfloat>(nx * ny, 0.0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &pressureOutTex);
    glBindTexture(GL_TEXTURE_2D, pressureOutTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &divergenceTex);
    glBindTexture(GL_TEXTURE_2D, divergenceTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &CellTypeTex);
    glBindTexture(GL_TEXTURE_2D, CellTypeTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, nx, ny);
    std::vector<GLubyte> type(nx * ny, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_UNSIGNED_BYTE, type.data());

}

void Grid2D::setShaders()
{
    m_initShader = std::make_unique<ShaderProgram>();
    m_initShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/initShader.shader");
    m_initShader->linkProgram();

    m_boundaryShader = std::make_unique<ShaderProgram>();
    m_boundaryShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/boundaryNoSplip.shader");
    m_boundaryShader->linkProgram();

    m_divergenceShader = std::make_unique<ShaderProgram>();
    m_divergenceShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/divergence.shader");
    m_divergenceShader->linkProgram();

    m_particleAdvectionShader = std::make_unique<ShaderProgram>();
    m_particleAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/particleForwardAdvection.shader");
    m_particleAdvectionShader->linkProgram();

    m_velocityAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/advectVelocities.shader");
    m_velocityAdvectionShader->linkProgram();

    m_jacobiPSolverShader = std::make_unique<ShaderProgram>();
    m_jacobiPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/jacobiPressureSolver.shader");
    m_jacobiPSolverShader->linkProgram();

    m_pressureProjectionShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/pressureProjection.shader");
    m_pressureProjectionShader->linkProgram();

    m_cellUpdateShader = std::make_unique<ShaderProgram>();
    m_cellUpdateShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/gridCellTypeUpdate.shader");
    m_cellUpdateShader->linkProgram();

    m_addForcesShader = std::make_unique<ShaderProgram>();
    m_addForcesShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/addForces.shader");
    m_addForcesShader->linkProgram();
}
