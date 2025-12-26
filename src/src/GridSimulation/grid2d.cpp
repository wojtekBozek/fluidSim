#include "grid2d.hpp"
#include <iostream>

void Grid2D::run()
{
    /*
    0. calculate timestep based on max velocity
    1. particle advection
    2. cell reconfiguration
    3. velocity advection
    4. External forces
    5. divergence
    5. Pressure calculation (40-80 iterations)
    6. Pressure projection
    7. Boundary conditions (optional if not already applied)
    */

    GLuint query;
    glGenQueries(1, &query);

    glBeginQuery(GL_TIME_ELAPSED, query);
    //czyszczenie
    m_clearFluidShader->useProgram();
    glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
    m_clearFluidShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    
    m_cellUpdateShader->useProgram();
    glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);    
    m_cellUpdateShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_cellUpdateShader->setVec2("gridMin", glm::vec2(0.0f,0.0f));
    m_cellUpdateShader->setVec2("gridMax", glm::vec2(nx*dx,ny*dx));
    m_cellUpdateShader->setUint("numOfParticles", particles.size());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
    glDispatchCompute((particles.size() + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_addForcesShader->useProgram();
    //set textures
    
    glBindImageTexture(0, vInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    //glBindImageTexture(1, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    //glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    //
    m_addForcesShader->setFloat("vAccelerations", -9.8f);
    m_addForcesShader->setFloat("dt", dt);
    m_addForcesShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    
    glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);


    /*
    m_velocityAdvectionShader->useProgram();
    //setTextures
    
    glBindImageTexture(0, uInTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(1, vInTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(2, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(3, uOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(4, vOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    //
    m_velocityAdvectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_velocityAdvectionShader->setFloat("dt", dt);
    m_velocityAdvectionShader->setFloat("dx", dx);
        
    glDispatchCompute((nx + 1) / 16, (ny+1) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    m_divergenceShader->useProgram();

    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(1, vOutTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(2, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    m_divergenceShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_divergenceShader->setFloat("dt", dt);
    m_divergenceShader->setFloat("dx", dx);
        
    glDispatchCompute((nx) / 16, (ny) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    m_jacobiPSolverShader->useProgram();
    //setTextures
    //
    m_jacobiPSolverShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_jacobiPSolverShader->setFloat("dx", dx);
    glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glBindImageTexture(2, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    m_jacobiPSolverShader->setFloat("dt", dt);
    m_jacobiPSolverShader->setFloat("density", 997.0);

    for(int i=0; i<pressureIterations; ++i)
    {        
        glBindImageTexture(0, pressureInTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx) / 16, (ny) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        std::swap(pressureInTex, pressureOutTex);
    }

    m_pressureProjectionShader->useProgram();
    //setTextures
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(1, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glBindImageTexture(2, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    //
    m_pressureProjectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionShader->setFloat("dt", dt);
    m_pressureProjectionShader->setFloat("dx", dx);

    glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    */

    //m_boundaryShader->useProgram();
    ////setTextures
    //glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    ////
    //m_boundaryShader->setIVec2("gridSize", glm::ivec2(nx,ny));
//
    //glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    glEndQuery(GL_TIME_ELAPSED);
    
    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &m_computeTime);
    
    //przesuwanie cząsteczek
    //glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    //
    m_particleAdvectionShader->useProgram();
    
    //glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    //glBindImageTexture(2, uInTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, uInTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, vInTex);
    //glBindImageTexture(3, vInTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    m_particleAdvectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));    
    m_particleAdvectionShader->setUint("numOfParticles", particles.size());
    m_particleAdvectionShader->setFloat("dt", dt);
    m_particleAdvectionShader->setFloat("dx", dx);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
    glDispatchCompute((particles.size() + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    //std::swap(uInTex, uOutTex);
    //std::swap(vInTex, vOutTex);
    //std::cout << m_computeTime << "\n";
}

void Grid2D::setup()
{
    setShaders();
    setTextures();
    initilizeGrid();
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

    glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
    glBindImageTexture(1, uInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(2, vInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(3, pressureInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    glBindImageTexture(4, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    m_initShader->setInt2("gridSize", nx, ny);
    glDispatchCompute((nx+15) / 16, (ny + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);*/
    
    std::vector<GLuint> type(nx * ny, 0);
    for (uint32_t y = 0; y < ny; ++y)
    {
        for (uint32_t x = 0; x < nx; ++x)
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
    std::cout << "Num of Particles: " << particles.size() << ".\n";
    //glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED_INTEGER, GL_UNSIGNED_BYTE, type.data());

    GLuint bufsize = particles.size() * sizeof(glm::vec2);
    if (glIsBuffer(m_particleBuffer) == GL_FALSE)
    {
        glGenBuffers(1, &m_particleBuffer);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, particles.data(), GL_DYNAMIC_DRAW);
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

    glGenTextures(1, &cellTypeTex);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, nx, ny);
    std::vector<GLuint> type(nx * ny, 0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED_INTEGER, GL_UNSIGNED_INT, type.data());

}

void Grid2D::setShaders()
{
    m_boundaryShader = std::make_unique<ShaderProgram>();
    m_boundaryShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/boundaryNoSlip.shader");
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
    m_addForcesShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/addExternalForces.shader");
    m_addForcesShader->linkProgram();

    m_clearFluidShader = std::make_unique<ShaderProgram>();
    m_clearFluidShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/zeroFluid.shader");
    m_clearFluidShader->linkProgram();
}
