#include "grid2d.hpp"
#include <iostream>

void Grid2D::JacobiSolver()
{
    m_divergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_divergenceShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_divergenceShader->setFloat("dt", dt);
    m_divergenceShader->setFloat("dx", dx);
    m_divergenceShader->setFloat("overrelaxation", 1.0f);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_jacobiPSolverShader->useProgram();
    //setTextures
    //
    m_jacobiPSolverShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_jacobiPSolverShader->setFloat("dx", dx);
    m_jacobiPSolverShader->setFloat("dens", 1.0f);
    m_jacobiPSolverShader->setFloat("dt", dt);
    //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, divergenceTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    for(int i=0; i<pressureIterations; ++i)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        std::swap(pressureInTex, pressureOutTex);     
    }
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionUShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionVShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    glDispatchCompute((nx + 15) / 16, (ny+1) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_divergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_divergenceShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_divergenceShader->setFloat("dt", dt);
    m_divergenceShader->setFloat("dx", dx);
    m_divergenceShader->setFloat("overrelaxation", 1.9f);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
}

void Grid2D::GaussSiedelSolver()
{
    for(int i = 0; i< pressureIterations; i++)
    {
        m_gaussSiedelDivergenceShader->useProgram();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uOutTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, vOutTex);
        glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        //glBindImageTexture(4, pressureInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        
        m_gaussSiedelDivergenceShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_gaussSiedelDivergenceShader->setFloat("dt", dt);
        m_gaussSiedelDivergenceShader->setFloat("dx", dx);
        m_gaussSiedelDivergenceShader->setFloat("density", 997.0);
        m_gaussSiedelDivergenceShader->setFloat("overrelaxation", 1.7f);
        
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        if(i%2==1)
        {
            m_redUVelocityShader->useProgram();
            glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cellTypeTex);
            m_redUVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
            m_redUVelocityShader->setFloat("dt", dt);
            m_redUVelocityShader->setFloat("dx", dx);

            glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);


            m_redVVelocityShader->useProgram();
            glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cellTypeTex);
            m_redVVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
            m_redVVelocityShader->setFloat("dt", dt);
            m_redVVelocityShader->setFloat("dx", dx);

            glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        else
        {
            m_blackUVelocityShader->useProgram();
            glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cellTypeTex);
            m_blackUVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
            m_blackUVelocityShader->setFloat("dt", dt);
            m_blackUVelocityShader->setFloat("dx", dx);

            glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

            m_blackVVelocityShader->useProgram();
            glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cellTypeTex);
            m_blackVVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
            m_blackVVelocityShader->setFloat("dt", dt);
            m_blackVVelocityShader->setFloat("dx", dx);

            glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
    }
    /*
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionUShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionVShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    */
}

void Grid2D::GaussSiedelPressureSolver()
{
    m_gaussSiedelDivergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_gaussSiedelDivergenceShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_gaussSiedelDivergenceShader->setFloat("dt", dt);
    m_gaussSiedelDivergenceShader->setFloat("dx", dx);
    m_gaussSiedelDivergenceShader->setFloat("overrelaxation", 1.0f);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    for(int i=0; i<pressureIterations; ++i)
    {
        if(i%2 == 0){
        m_gsBlackPSolverShader->useProgram();
        //setTextures
        //
        m_gsBlackPSolverShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_gsBlackPSolverShader->setFloat("dx", dx);
        m_gsBlackPSolverShader->setFloat("dens", 1.0f);
        m_gsBlackPSolverShader->setFloat("dt", dt);
        //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, divergenceTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        else{
            m_gsRedPSolverShader->useProgram();
        //setTextures
        //
        m_gsRedPSolverShader->setIVec2("gridSize", glm::ivec2(nx,ny));
        m_gsRedPSolverShader->setFloat("dx", dx);
        m_gsRedPSolverShader->setFloat("dens", 1.0f);
        m_gsRedPSolverShader->setFloat("dt", dt);
        //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, divergenceTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        std::swap(pressureInTex, pressureOutTex);
    }
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionUShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    m_pressureProjectionVShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    glDispatchCompute((nx + 15) / 16, (ny+1) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

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
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);    
    
    std::swap(uInTex, uOutTex);
    std::swap(vInTex, vOutTex);
    /**/
    /*
    */
    //GaussSiedelPressureSolver();
    //GaussSiedelSolver();
    JacobiSolver();
    //}
    m_extrapolateUVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glBindImageTexture(2, uInTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateUVelocityShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_extrapolateUVelocityShader->setFloat("dt", dt);
    m_extrapolateUVelocityShader->setFloat("dx", dx);
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
    m_particleAdvectionShader->setUint("numOfParticles", particles.size());
    m_particleAdvectionShader->setFloat("dt", dt);
    m_particleAdvectionShader->setFloat("dx", dx);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
    glDispatchCompute((particles.size() + 255) / 256, 1, 1);
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

    //float maxU, maxV;
    /*
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
    */
    glEndQuery(GL_TIME_ELAPSED);
    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &m_computeTime);
    //std::cout << time << " : "<< m_computeTime << "\n";//  << " : " << maxU << " : " << maxV << "\n";
    temp += m_computeTime;
    time += dt;
    steps++;
}
}

void Grid2D::setup()
{
    setShaders();
    setTextures();
    initilizeGrid();

    glGenBuffers(1, &floatSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO);

    uint32_t init = 0x80000000;
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &init, GL_DYNAMIC_COPY);
    
    glGenBuffers(1, &floatSSBO2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO2);

    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &init, GL_DYNAMIC_COPY);
}

void Grid2D::setDimensions(uint32_t x, uint32_t y)
{   
    nx = x;
    ny = y;
}

void Grid2D::setCellSize(float x)
{
    dx = x;
}

const std::vector<glm::vec2>& Grid2D::getParticles() const
{
    return particles;
}

void Grid2D::initilizeGrid()
{
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
                    float randX = p%sqr;//float(std::rand())/float(RAND_MAX);
                    float randY = p/sqr;//float(std::rand())/float(RAND_MAX);
                    particles.push_back(glm::vec2(randX/float(sqr) * dx + x * dx + dx, randY/float(sqr) * dx + y * dx + dx));
                }
            }
            else
            {
                type[y * nx + x] = AIR;
            }
        }
    }
    std::cout << "Num of Particles: " << particles.size() << ".\n";
    glBindTexture(GL_TEXTURE_2D, cellTypeTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED_INTEGER, GL_UNSIGNED_INT, type.data());

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx + 1, ny);
    std::vector<GLfloat> zeros((nx + 1) * ny, 0.0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx+1, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &uOutTex);
    glBindTexture(GL_TEXTURE_2D, uOutTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx + 1, ny);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx + 1, ny, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &vInTex);
    glBindTexture(GL_TEXTURE_2D, vInTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny+1);
    zeros = std::vector<GLfloat>(nx * (ny + 1), 0.0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny + 1, GL_RED, GL_FLOAT, zeros.data());

    glGenTextures(1, &vOutTex);
    glBindTexture(GL_TEXTURE_2D, vOutTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    
    m_velocityVAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityVAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/advectVvelocities.shader");
    m_velocityVAdvectionShader->linkProgram();

    m_velocityUAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityUAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/gridFluid/advectUvelocities.shader");
    m_velocityUAdvectionShader->linkProgram();

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

void Grid2D::restart()
{
    glDeleteTextures(1, &uInTex); 
    glDeleteTextures(1, &vInTex); 
    glDeleteTextures(1, &uOutTex); 
    glDeleteTextures(1, &vOutTex); 
    glDeleteTextures(1, &divergenceTex); 
    glDeleteTextures(1, &pressureInTex); 
    glDeleteTextures(1, &pressureOutTex); 
    glDeleteTextures(1, &cellTypeTex); 
    particles.clear();
    setTextures();
    initilizeGrid();
}
