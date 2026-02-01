#include "mixed.hpp"
#include <iostream>
void ParticleInCell2D::run()
{
    GLuint query;
    GLuint64 temp = 0;
    uint32_t steps = 0;
    while(temp < 16000000 && steps*dt < 0.0166){
        
        glGenQueries(1, &query);

        glBeginQuery(GL_TIME_ELAPSED, query);
        //czyszczenie

        m_clearFluidShader->useProgram();
        glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
        glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glBindImageTexture(2, pressureInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        m_clearFluidShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_clearFluidShader->setBool("zeroPressure", zeroPressure);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        
        m_cellUpdateShader->useProgram();
        glBindImageTexture(0, cellTypeTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);    
        m_cellUpdateShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_cellUpdateShader->setVec2("gridMin", glm::vec2(0.0f,0.0f));
        m_cellUpdateShader->setVec2("gridMax", glm::vec2(nx*dx,ny*dx));
        m_cellUpdateShader->setUint("numOfParticles", m_numOfParticles);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
        glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        

        m_velocityUAdvectionShader->useProgram();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uInTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, vInTex);
        glBindImageTexture(3, uOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_velocityUAdvectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_velocityUAdvectionShader->setFloat("dt", dt);
        m_velocityUAdvectionShader->setFloat("dx", dx);
        
        glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        m_velocityVAdvectionShader->useProgram();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uInTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, vInTex);
        glBindImageTexture(3, vOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_velocityVAdvectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_velocityVAdvectionShader->setFloat("dt", dt);
        m_velocityVAdvectionShader->setFloat("dx", dx);
        
        glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        m_addForcesShader->useProgram();    
        glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        m_addForcesShader->setFloat("vAccelerations", -9.8f);
        m_addForcesShader->setFloat("dt", dt);
        m_addForcesShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        
        glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        
        /**/
        m_extrapolateUVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uOutTex);
        glBindImageTexture(2, uInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_extrapolateUVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_extrapolateUVelocityShader->setFloat("dt", dt);
        m_extrapolateUVelocityShader->setFloat("dx", dx);
        m_extrapolateUVelocityShader->setInt("borderSize", borderSize);
        glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        
        m_extrapolateVVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vOutTex);
        glBindImageTexture(2, vInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_extrapolateVVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_extrapolateVVelocityShader->setFloat("dt", dt);
        m_extrapolateVVelocityShader->setFloat("dx", dx);
        m_extrapolateVVelocityShader->setInt("borderSize", borderSize);
        glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);    
        
        std::swap(uInTex, uOutTex);
        std::swap(vInTex, vOutTex);
        /**/
        /*
        */
        if(m_solver == SOLVER::JACOBI)
        {
            JacobiSolver();
        }
        else if(m_solver == SOLVER::GS_DIVERGENCE)
        {
            GaussSiedelSolver();
        }
        else if(m_solver == SOLVER::GS_PRESSURE)
        {
            GaussSiedelPressureSolver();
        }
        m_extrapolateUVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uOutTex);
        glBindImageTexture(2, uInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_extrapolateUVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_extrapolateUVelocityShader->setFloat("dt", dt);
        m_extrapolateUVelocityShader->setFloat("dx", dx);
        m_extrapolateUVelocityShader->setInt("borderSize", borderSize);
        glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        m_extrapolateVVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vOutTex);
        glBindImageTexture(2, vInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        m_extrapolateVVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_extrapolateVVelocityShader->setFloat("dt", dt);
        m_extrapolateVVelocityShader->setFloat("dx", dx);
        m_extrapolateVVelocityShader->setInt("borderSize", borderSize);
        glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        m_particleAdvectionShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, uInTex);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, vInTex);
        m_particleAdvectionShader->setIVec2("gridSize", glm::ivec2(nx,ny));    
        m_particleAdvectionShader->setUint("numOfParticles", m_numOfParticles);
        m_particleAdvectionShader->setFloat("dt", dt);
        m_particleAdvectionShader->setFloat("dx", dx);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
        glDispatchCompute((m_numOfParticles + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        /*
        */
        /*
        std::swap(uInTex, uOutTex);
        std::swap(vInTex, vOutTex);

        m_boundaryShader->useProgram();
        glBindImageTexture(0, uInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glBindImageTexture(1, vInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        m_boundaryShader->setIVec2("gridSize", glm::ivec2(nx,ny));

        glDispatchCompute((nx+1 + 15) / 16, (ny+1+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
         */

        /*
        float maxU, maxV;
        m_maxVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uInTex);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, floatSSBO);
        glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        uint32_t maxValueUint;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &maxValueUint);
        maxU = *reinterpret_cast<float*>(&maxValueUint);

        m_maxVelocityShader->useProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vInTex);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, floatSSBO2);
        glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO2);
        uint32_t maxValueVint;
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &maxValueVint);
        maxV = *reinterpret_cast<float*>(&maxValueVint);
        std::cout << " : " << maxU << " : " << maxV << "\n";
        */
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectui64v(query, GL_QUERY_RESULT, &m_computeTime);
        temp += m_computeTime;
        time += dt;
        steps++;
        m_currentStep++;
    }
}

void ParticleInCell2D::setup()
{
}

void ParticleInCell2D::initialize()
{
    std::vector<Particle> particles;
    uint32_t sqr = sqrt(particlesPerCell);
    std::vector<GLuint> type(nx * ny, 0);
    for (uint32_t y = 0; y < ny; ++y)
    {
        for (uint32_t x = 0; x < nx; ++x)
        {
            if(5 > x || 5> y || nx-5 <= x || ny-5 <= y)
            {
                type[y * nx + x] = SOLID;
                continue;
            }
            if (x >= initFluidX && x < initFluidX + initFluidWidth && y >= initFluidY && y < initFluidY + initFluidHeight)
            {
                type[y * nx + x] = FLUID;

                for(int p = 0; p<particlesPerCell; p++)
                {
                    float randX = p%sqr;//*/float(std::rand())/float(RAND_MAX);
                    float randY = p/sqr;//*/float(std::rand())/float(RAND_MAX);
                    particles.push_back({glm::vec2(randX/float(sqr) * dx + x * dx, randY/float(sqr) * dx + y * dx),glm::vec2(0.0,0.0)});
                }
            }
            else
            {
                type[y * nx + x] = AIR;
            }
        }
    }
    m_numOfParticles = particles.size();
    std::cout << "Num of Particles: " << particles.size() << ".\n";
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED_INTEGER, GL_UNSIGNED_INT, type.data());

    if (glIsBuffer(m_particleBuffer) == GL_FALSE)
    {
        glGenBuffers(1, &m_particleBuffer);
    }
    GLuint bufsize = particles.size() * sizeof(Particle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, particles.data(), GL_DYNAMIC_DRAW);

    if (glIsBuffer(weightsUssbo) == GL_FALSE)
    {
        glGenBuffers(1, &weightsUssbo);
    }
    bufsize = (ny-1)*nx * sizeof(float);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, weightsUssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, nullptr, GL_DYNAMIC_DRAW);
    if (glIsBuffer(Ussbo) == GL_FALSE)
    {
        glGenBuffers(1, &Ussbo);
    }
    bufsize = (ny-1)*nx * sizeof(float);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, Ussbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, nullptr, GL_DYNAMIC_DRAW);
    if (glIsBuffer(Vssbo) == GL_FALSE)
    {
        glGenBuffers(1, &Vssbo);
    }
    bufsize = ny*(nx-1) * sizeof(float);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, Vssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, nullptr, GL_DYNAMIC_DRAW);
    if (glIsBuffer(weightsVssbo) == GL_FALSE)
    {
        glGenBuffers(1, &weightsVssbo);
    }
    bufsize = ny*(nx-1) * sizeof(float);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, weightsVssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, nullptr, GL_DYNAMIC_DRAW);
}

void ParticleInCell2D::setShaders()
{
    m_transferVelocityToGridShader = std::make_unique<ShaderProgram>();
    m_transferVelocityToGridShader->addShader(GL_COMPUTE_SHADER, "shaders/mixed/particleToGridProjection.shader");
    m_transferVelocityToGridShader->linkProgram();

    m_calculateVelocityOfParticles = std::make_unique<ShaderProgram>();
    m_calculateVelocityOfParticles->addShader(GL_COMPUTE_SHADER, "shaders/mixed/gridToPraticleProjection.shader");

    m_boundaryShader = std::make_unique<ShaderProgram>();
    m_boundaryShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/boundaryNoSlip.shader");
    m_boundaryShader->linkProgram();
    
    m_divergenceShader = std::make_unique<ShaderProgram>();
    m_divergenceShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/divergence.shader");
    m_divergenceShader->linkProgram();
    
    m_particleAdvectionShader = std::make_unique<ShaderProgram>();
    m_particleAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/particleForwardAdvection.shader");
    m_particleAdvectionShader->linkProgram();

    m_jacobiPSolverShader = std::make_unique<ShaderProgram>();
    m_jacobiPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/jacobiPressureSolver.shader");
    m_jacobiPSolverShader->linkProgram();

    m_pressureProjectionUShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionUShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/pressureUProjection.shader");
    m_pressureProjectionUShader->linkProgram();

    m_gsBlackPSolverShader = std::make_unique<ShaderProgram>();
    m_gsBlackPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/pressureUpdateBlack.shader");
    m_gsBlackPSolverShader->linkProgram();
    m_gsRedPSolverShader = std::make_unique<ShaderProgram>();
    m_gsRedPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/pressureUpdateRed.shader");
    m_gsRedPSolverShader->linkProgram();

    m_pressureProjectionVShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionVShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/pressureVProjection.shader");
    m_pressureProjectionVShader->linkProgram();
    
    m_cellUpdateShader = std::make_unique<ShaderProgram>();
    m_cellUpdateShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/gridCellTypeUpdate.shader");
    m_cellUpdateShader->linkProgram();

    m_addForcesShader = std::make_unique<ShaderProgram>();
    m_addForcesShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/addExternalForces.shader");
    m_addForcesShader->linkProgram();

    m_clearFluidShader = std::make_unique<ShaderProgram>();
    m_clearFluidShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/zeroFluid.shader");
    m_clearFluidShader->linkProgram();

    m_maxVelocityShader = std::make_unique<ShaderProgram>();
    m_maxVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/maxVelocity.shader");
    m_maxVelocityShader->linkProgram();

    m_extrapolateUVelocityShader = std::make_unique<ShaderProgram>();
    m_extrapolateUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/airUextrapolation.shader");
    m_extrapolateUVelocityShader->linkProgram();

    m_extrapolateVVelocityShader = std::make_unique<ShaderProgram>();
    m_extrapolateVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/airVextrapolation.shader");
    m_extrapolateVVelocityShader->linkProgram();


    m_gaussSiedelDivergenceShader = std::make_unique<ShaderProgram>();
    m_gaussSiedelDivergenceShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/divergenceRedBlack.shader");
    m_gaussSiedelDivergenceShader->linkProgram();

    m_redUVelocityShader = std::make_unique<ShaderProgram>();
    m_redUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/uRedUpdate.shader");
    m_redUVelocityShader->linkProgram();

    m_redVVelocityShader = std::make_unique<ShaderProgram>();
    m_redVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/VRedUpdate.shader");
    m_redVVelocityShader->linkProgram();

    m_blackUVelocityShader = std::make_unique<ShaderProgram>();
    m_blackUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/UBlackUpdate.shader");
    m_blackUVelocityShader->linkProgram();

    m_blackVVelocityShader = std::make_unique<ShaderProgram>();
    m_blackVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/GaussSeidelSolve/VBlackUpdate.shader");
    m_blackVVelocityShader->linkProgram();
}

void ParticleInCell2D::restart()
{
    glDeleteTextures(1, &uInTex); 
    glDeleteTextures(1, &vInTex); 
    glDeleteTextures(1, &uOutTex); 
    glDeleteTextures(1, &vOutTex); 
    glDeleteTextures(1, &divergenceTex); 
    glDeleteTextures(1, &pressureInTex); 
    glDeleteTextures(1, &pressureOutTex); 
    glDeleteTextures(1, &cellTypeTex);
    setTextures();
    initilizeGrid();
}
