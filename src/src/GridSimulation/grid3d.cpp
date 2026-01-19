#include "grid3d.hpp"
#include <iostream>

void Grid3D::JacobiSolver()
{
    m_divergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_divergenceShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_divergenceShader->setFloat("dt", dt);
    m_divergenceShader->setFloat("dx", dx);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_jacobiPSolverShader->useProgram();
    //setTextures
    //
    m_jacobiPSolverShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_jacobiPSolverShader->setFloat("dx", dx);
    m_jacobiPSolverShader->setFloat("dens", m_dens);
    m_jacobiPSolverShader->setFloat("dt", dt);
    m_jacobiPSolverShader->setFloat("overrelaxation", m_overrelaxation);
    //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, divergenceTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    for(int i=0; i<pressureIterations; ++i)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        std::swap(pressureInTex, pressureOutTex);     
    }
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionUShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    m_pressureProjectionUShader->setFloat("dens", m_dens);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionVShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    m_pressureProjectionVShader->setFloat("dens", m_dens);
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionVShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    m_pressureProjectionVShader->setFloat("dens", m_dens);
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_divergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_divergenceShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_divergenceShader->setFloat("dt", dt);
    m_divergenceShader->setFloat("dx", dx);
    m_divergenceShader->setFloat("overrelaxation", 1.9f);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
}

void Grid3D::GaussSiedelSolver()
{
    for(int i = 0; i< pressureIterations; i++)
    {
        m_gaussSiedelDivergenceShader->useProgram();
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, uOutTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, vOutTex);
        glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        //glBindImageTexture(4, pressureInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        
        m_gaussSiedelDivergenceShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
        m_gaussSiedelDivergenceShader->setFloat("dt", dt);
        m_gaussSiedelDivergenceShader->setFloat("dx", dx);
        m_gaussSiedelDivergenceShader->setFloat("density", m_dens);
        m_gaussSiedelDivergenceShader->setFloat("overrelaxation", m_overrelaxation);
        
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        if(i%2==1)
        {
            m_redUVelocityShader->useProgram();
            glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, cellTypeTex);
            m_redUVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
            m_redUVelocityShader->setFloat("dt", dt);
            m_redUVelocityShader->setFloat("dx", dx);

            glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+1)/16);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);


            m_redVVelocityShader->useProgram();
            glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, cellTypeTex);
            m_redVVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
            m_redVVelocityShader->setFloat("dt", dt);
            m_redVVelocityShader->setFloat("dx", dx);

            glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        else
        {
            m_blackUVelocityShader->useProgram();
            glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, cellTypeTex);
            m_blackUVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
            m_blackUVelocityShader->setFloat("dt", dt);
            m_blackUVelocityShader->setFloat("dx", dx);

            glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

            m_blackVVelocityShader->useProgram();
            glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, divergenceTex);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_3D, cellTypeTex);
            m_blackVVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
            m_blackVVelocityShader->setFloat("dt", dt);
            m_blackVVelocityShader->setFloat("dx", dx);

            glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
    }
    /*
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionUShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionVShader->setIVec2("gridSize", glm::ivec2(nx,ny));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    */
}

void Grid3D::GaussSiedelPressureSolver()
{
    m_gaussSiedelDivergenceShader->useProgram();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glBindImageTexture(3, divergenceTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    
    m_gaussSiedelDivergenceShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_gaussSiedelDivergenceShader->setFloat("dt", dt);
    m_gaussSiedelDivergenceShader->setFloat("dx", dx);
    m_gaussSiedelDivergenceShader->setFloat("overrelaxation", m_overrelaxation);
    
    glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    for(int i=0; i<pressureIterations; ++i)
    {
        if(i%2 == 0){
        m_gsBlackPSolverShader->useProgram();
        //setTextures
        //
        m_gsBlackPSolverShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
        m_gsBlackPSolverShader->setFloat("dx", dx);
        m_gsBlackPSolverShader->setFloat("dens", m_dens);
        m_gsBlackPSolverShader->setFloat("dt", dt);
        //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, divergenceTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        else{
            m_gsRedPSolverShader->useProgram();
        //setTextures
        //
        m_gsRedPSolverShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
        m_gsRedPSolverShader->setFloat("dx", dx);
        m_gsRedPSolverShader->setFloat("dens", m_dens);
        m_gsRedPSolverShader->setFloat("dt", dt);
        //glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, divergenceTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, cellTypeTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, pressureInTex);
        glBindImageTexture(3, pressureOutTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        std::swap(pressureInTex, pressureOutTex);
    }
    m_pressureProjectionUShader->useProgram();
    glBindImageTexture(0, uOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionUShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_pressureProjectionUShader->setFloat("dt", dt);
    m_pressureProjectionUShader->setFloat("dx", dx);
    m_pressureProjectionUShader->setFloat("dens", m_dens);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

    m_pressureProjectionVShader->useProgram();
    glBindImageTexture(0, vOutTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_pressureProjectionVShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_pressureProjectionVShader->setFloat("dt", dt);
    m_pressureProjectionVShader->setFloat("dx", dx);
    m_pressureProjectionVShader->setFloat("dens", m_dens);
    glDispatchCompute((nx + 15) / 16, (ny+1) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}

void Grid3D::run()
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
/*
m_clearFluidShader->useProgram();
glBindImageTexture(0, cellTypeTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);
glBindImageTexture(1, divergenceTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
glBindImageTexture(2, pressureInTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
m_clearFluidShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
m_clearFluidShader->setBool("zeroPressure", zeroPressure);
glDispatchCompute((nx+15) / 16, (ny+15) / 16, (nz+15)/16);
glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

m_cellUpdateShader->useProgram();
glBindImageTexture(0, cellTypeTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);    
m_cellUpdateShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
m_cellUpdateShader->setVec3("gridMin", glm::vec3(0.0f,0.0f,0.0f));
m_cellUpdateShader->setVec3("gridMax", glm::vec3(nx*dx,ny*dx,nz*dx));
m_cellUpdateShader->setUint("numOfParticles", particles.size());
glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
glDispatchCompute((particles.size() + 255) / 256, 1, 1);
glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

*/
   /*
   m_velocityUAdvectionShader->useProgram();
   glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uInTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, wInTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glBindImageTexture(4, uOutTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_velocityUAdvectionShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_velocityUAdvectionShader->setFloat("dt", dt);
    m_velocityUAdvectionShader->setFloat("dx", dx);
    
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_velocityVAdvectionShader->useProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uInTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, wInTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glBindImageTexture(4, vOutTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_velocityVAdvectionShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_velocityVAdvectionShader->setFloat("dt", dt);
    m_velocityVAdvectionShader->setFloat("dx", dx);
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_velocityWAdvectionShader->useProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uInTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, vInTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, wInTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glBindImageTexture(4, wOutTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_velocityWAdvectionShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_velocityWAdvectionShader->setFloat("dt", dt);
    m_velocityWAdvectionShader->setFloat("dx", dx);
    glDispatchCompute((nx + 15) / 16, (ny+15) / 16, ((nz+1)+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    */
    
    m_addForcesShader->useProgram();    
    glBindImageTexture(0, vInTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    m_addForcesShader->setFloat("vAccelerations", -9.8f);
    m_addForcesShader->setFloat("dt", dt);
    m_addForcesShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));    
    glDispatchCompute((nx + 7) / 8, ((ny+1)+7) / 8, (nz+7)/8);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    /*
    m_extrapolateUVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glBindImageTexture(2, uInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateUVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateUVelocityShader->setFloat("dt", dt);
    m_extrapolateUVelocityShader->setFloat("dx", dx);
    m_extrapolateUVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_extrapolateVVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glBindImageTexture(2, vInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateVVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateVVelocityShader->setFloat("dt", dt);
    m_extrapolateVVelocityShader->setFloat("dx", dx);
    m_extrapolateVVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);    
    
    m_extrapolateWVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, wOutTex);
    glBindImageTexture(2, wInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateWVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateWVelocityShader->setFloat("dt", dt);
    m_extrapolateWVelocityShader->setFloat("dx", dx);
    m_extrapolateWVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute((nx + 15) / 16, (ny+15) / 16, (nz+1+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    */
   //std::swap(uInTex, uOutTex);
   //std::swap(vInTex, vOutTex);
   //std::swap(wInTex, wOutTex);
    /**/
    /*
    if(m_solver == SOLVER::JACOBI)
    {
        JacobiSolver();
    }
    else if(m_solver == SOLVER::GS_DIVERGENCE)
    {
        //GaussSiedelSolver();
    }
    else if(m_solver == SOLVER::GS_PRESSURE)
    {
        //GaussSiedelPressureSolver();
    }
    */
    /*
    m_extrapolateUVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glBindImageTexture(2, uInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateUVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateUVelocityShader->setFloat("dt", dt);
    m_extrapolateUVelocityShader->setFloat("dx", dx);
    m_extrapolateUVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute(((nx+1) + 15) / 16, (ny+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_extrapolateVVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glBindImageTexture(2, vInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateVVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateVVelocityShader->setFloat("dt", dt);
    m_extrapolateVVelocityShader->setFloat("dx", dx);
    m_extrapolateVVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute((nx + 15) / 16, ((ny+1)+15) / 16, (nz+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    m_extrapolateWVelocityShader->useProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, wOutTex);
    glBindImageTexture(2, wInTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
    m_extrapolateWVelocityShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));
    m_extrapolateWVelocityShader->setFloat("dt", dt);
    m_extrapolateWVelocityShader->setFloat("dx", dx);
    m_extrapolateWVelocityShader->setInt("borderSize", borderSize);
    glDispatchCompute((nx + 15) / 16, (ny+15) / 16, ((nz+1)+15)/16);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
    
    */
   
    m_particleAdvectionShader->useProgram();
    glBindTextureUnit(0, cellTypeTex);
    glBindTextureUnit(2, uInTex);
    glBindTextureUnit(3, vOutTex);
    glBindTextureUnit(4, wInTex);
    m_particleAdvectionShader->setIVec3("gridSize", glm::ivec3(nx,ny,nz));    
    m_particleAdvectionShader->setUint("numOfParticles", particles.size());
    m_particleAdvectionShader->setFloat("dt", dt);
    m_particleAdvectionShader->setFloat("dx", dx);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBuffer);
    glDispatchCompute((particles.size() + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    
   /*
   std::swap(uInTex, uOutTex);
   std::swap(vInTex, vOutTex);
   
   m_boundaryShader->useProgram();
   glBindImageTexture(0, uInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
   glBindImageTexture(1, vInTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_3D, cellTypeTex);
   m_boundaryShader->setIVec2("gridSize", glm::ivec2(nx,ny));
   
   glDispatchCompute((nx+1 + 15) / 16, (ny+1+15) / 16, 1);
   glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    */

   /*
   float maxU, maxV;
   m_maxVelocityShader->useProgram();
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_3D, uInTex);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, floatSSBO);
   glDispatchCompute((nx + 15) / 16, (ny+15) / 16, 1);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   uint32_t maxValueUint;
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO);
   glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &maxValueUint);
   maxU = *reinterpret_cast<float*>(&maxValueUint);
   
   m_maxVelocityShader->useProgram();
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_3D, vInTex);
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
}
}

void Grid3D::setup()
{
    setShaders();
    setTextures();
    initilizeGrid();

    //glGenBuffers(1, &floatSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO);
//
    //uint32_t init = 0x80000000;
    //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &init, GL_DYNAMIC_COPY);
    //
    //glGenBuffers(1, &floatSSBO2);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, floatSSBO2);
//
    //glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t), &init, GL_DYNAMIC_COPY);
}

void Grid3D::setDimensions(uint32_t x, uint32_t y, uint32_t z)
{   
    nx = x;
    ny = y;
    nz = z;
}

void Grid3D::setCellSize(float x)
{
    dx = x;
}

const std::vector<glm::vec4>& Grid3D::getParticles() const
{
    return particles;
}

void Grid3D::initilizeGrid()
{
    std:: cout << "Initializing grid\n";
    uint32_t cbr = cbrt(particlesPerCell);
    std:: cout << cbr << " - spacing\n";
    std::vector<GLuint> type(nx * ny * nz, AIR);
    for (uint32_t z = 0; z < nz; ++z)
    {
        for (uint32_t y = 0; y < ny; ++y)
        {
            for (uint32_t x = 0; x < nx; ++x)
            {
                uint32_t idx = (z * ny + y) * nx + x;
                if(1 > x || 1> y || nx-1 <= x || ny-1 <= y || 1> z || nz-1 <= z)
                {
                    type[idx] = AIR;
                    continue;
                }
                if ((x >= initFluidX && x < initFluidX + initFluidWidth) && (y >= initFluidY && y < initFluidY + initFluidHeight) && (z >= initFluidZ && z < initFluidZ + initFluidDepth))
                {
                    type[idx] = FLUID;

                    for(int p = 0; p<particlesPerCell; p++)
                    {
                        float randX = p%cbr;
                        float randY = (p/cbr)%cbr;
                        float randZ = p/(cbr*cbr);
                        particles.push_back(glm::vec4(randX/float(cbr) * dx + x * dx + 0.1*dx, randY/float(cbr) * dx + y * dx + 0.1*dx, randZ/float(cbr) * dx + z * dx+0.1*dx, 1.0));
                        //std::cout << particles.back().x<<"/" << particles.back().y << "/"<< particles.back().z << "\n";
                    }
                }
                else
                {
                    type[idx] = AIR;
                }
            }
        }
    }
    //particles.clear();
    //std::cout << "Num of Particles: " << particles.size() << ".\n";
    //std::cout << sizeof(glm::vec4) << "\n";
    //particles.push_back({5.0,1.0,1.0,1.0});
    //particles.push_back({2.0,1.0,1.0,1.0});
    //particles.push_back({3.0,1.0,1.0,1.0});
    //particles.push_back({4.0,1.0,1.0,1.0});
    GLuint bufsize = particles.size() * sizeof(glm::vec4);
    //static_assert(sizeof(glm::vec4) == 12, "glm::vec3 is padded!");
    std::cout <<particles.size() << "\n";
    std::cout << bufsize << "\n";
    
    if (glIsBuffer(m_particleBuffer) == GL_FALSE)
    {
        glGenBuffers(1, &m_particleBuffer);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufsize, particles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBuffer);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz, GL_RED_INTEGER, GL_UNSIGNED_INT, type.data());
}

void Grid3D::setTextures()
{
    std:: cout << "Setting Textures \n";
    
    std:: cout << "Uin\n";
    glGenTextures(1, &uInTex);
    glBindTexture(GL_TEXTURE_3D, uInTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx + 1, ny, nz);
    std::vector<GLfloat> zeros((nx + 1) * ny *nz, 0.0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx+1, ny, nz, GL_RED, GL_FLOAT, zeros.data());

    std:: cout << "Uout\n";
    glGenTextures(1, &uOutTex);
    glBindTexture(GL_TEXTURE_3D, uOutTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx + 1, ny, nz);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx + 1, ny, nz, GL_RED, GL_FLOAT, zeros.data());

    std:: cout << "Vin\n";
    glGenTextures(1, &vInTex);
    glBindTexture(GL_TEXTURE_3D, vInTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny+1, nz);
    zeros = std::vector<GLfloat>(nx * (ny + 1) *nz, 1.0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny + 1, nz, GL_RED, GL_FLOAT, zeros.data());

    std:: cout << "Vout\n";
    glGenTextures(1, &vOutTex);
    glBindTexture(GL_TEXTURE_3D, vOutTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny+1, nz);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny + 1, nz, GL_RED, GL_FLOAT, zeros.data());

    
    std:: cout << "Win\n";
    glGenTextures(1, &wInTex);
    glBindTexture(GL_TEXTURE_3D, wInTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny, nz+1);
    zeros = std::vector<GLfloat>(nx * ny * (nz+1), 0.0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz+1, GL_RED, GL_FLOAT, zeros.data());

    std:: cout << "Wout\n";
    glGenTextures(1, &wOutTex);
    glBindTexture(GL_TEXTURE_3D, wOutTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny, nz+1);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz+1, GL_RED, GL_FLOAT, zeros.data());

    
    std:: cout << "PressureIn\n";
    glGenTextures(1, &pressureInTex);
    glBindTexture(GL_TEXTURE_3D, pressureInTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny, nz);
    zeros = std::vector<GLfloat>(nx * ny * nz, 0.0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz, GL_RED, GL_FLOAT, zeros.data());

    
    std:: cout << "PressureOut\n";
    glGenTextures(1, &pressureOutTex);
    glBindTexture(GL_TEXTURE_3D, pressureOutTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny, nz);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz, GL_RED, GL_FLOAT, zeros.data());

    std:: cout << "Divergence\n";
    glGenTextures(1, &divergenceTex);
    glBindTexture(GL_TEXTURE_3D, divergenceTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, nx, ny, nz);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz, GL_RED, GL_FLOAT, zeros.data());

    
    std:: cout << "CellType\n";
    glGenTextures(1, &cellTypeTex);
    glBindTexture(GL_TEXTURE_3D, cellTypeTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32UI, nx, ny, nz);
    std::vector<GLuint> type(nz * nx * ny, 0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, nx, ny, nz, GL_RED_INTEGER, GL_UNSIGNED_INT, type.data());
    
    std:: cout << "Texture fin\n";

}

void Grid3D::setShaders()
{
    
    std:: cout << "SettingShaders\n";
    m_boundaryShader = std::make_unique<ShaderProgram>();
    m_boundaryShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/boundaryNoSlip.shader");
    m_boundaryShader->linkProgram();
    
    m_divergenceShader = std::make_unique<ShaderProgram>();
    m_divergenceShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/divergence.shader");
    m_divergenceShader->linkProgram();
    
    m_particleAdvectionShader = std::make_unique<ShaderProgram>();
    m_particleAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/particleForwardAdvection.shader");
    m_particleAdvectionShader->linkProgram();
    
    m_velocityVAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityVAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/advectVvelocities.shader");
    m_velocityVAdvectionShader->linkProgram();

    m_velocityUAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityUAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/advectUvelocities.shader");
    m_velocityUAdvectionShader->linkProgram();

    m_velocityWAdvectionShader = std::make_unique<ShaderProgram>();
    m_velocityWAdvectionShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/advectWvelocities.shader");
    m_velocityWAdvectionShader->linkProgram();

    m_jacobiPSolverShader = std::make_unique<ShaderProgram>();
    m_jacobiPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/jacobiPressureSolver.shader");
    m_jacobiPSolverShader->linkProgram();

    m_pressureProjectionUShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionUShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/pressureUProjection.shader");
    m_pressureProjectionUShader->linkProgram();

    m_pressureProjectionVShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionVShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/pressureVProjection.shader");
    m_pressureProjectionVShader->linkProgram();

    m_pressureProjectionWShader = std::make_unique<ShaderProgram>();
    m_pressureProjectionWShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/pressureWProjection.shader");
    m_pressureProjectionWShader->linkProgram();

    m_gsBlackPSolverShader = std::make_unique<ShaderProgram>();
    m_gsBlackPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/pressureUpdateBlack.shader");
    m_gsBlackPSolverShader->linkProgram();
    m_gsRedPSolverShader = std::make_unique<ShaderProgram>();
    m_gsRedPSolverShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/pressureUpdateRed.shader");
    m_gsRedPSolverShader->linkProgram();
    
    m_cellUpdateShader = std::make_unique<ShaderProgram>();
    m_cellUpdateShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/gridCellTypeUpdate.shader");
    m_cellUpdateShader->linkProgram();

    m_addForcesShader = std::make_unique<ShaderProgram>();
    m_addForcesShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/addExternalForces.shader");
    m_addForcesShader->linkProgram();

    m_clearFluidShader = std::make_unique<ShaderProgram>();
    m_clearFluidShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/zeroFluid.shader");
    m_clearFluidShader->linkProgram();

    m_maxVelocityShader = std::make_unique<ShaderProgram>();
    m_maxVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/maxVelocity.shader");
    m_maxVelocityShader->linkProgram();

    m_extrapolateUVelocityShader = std::make_unique<ShaderProgram>();
    m_extrapolateUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/airUextrapolation.shader");
    m_extrapolateUVelocityShader->linkProgram();

    m_extrapolateVVelocityShader = std::make_unique<ShaderProgram>();
    m_extrapolateVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/airVextrapolation.shader");
    m_extrapolateVVelocityShader->linkProgram();
    
    m_extrapolateWVelocityShader = std::make_unique<ShaderProgram>();
    m_extrapolateWVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/airWextrapolation.shader");
    m_extrapolateWVelocityShader->linkProgram();

    m_gaussSiedelDivergenceShader = std::make_unique<ShaderProgram>();
    m_gaussSiedelDivergenceShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/divergenceRedBlack.shader");
    m_gaussSiedelDivergenceShader->linkProgram();

    m_redUVelocityShader = std::make_unique<ShaderProgram>();
    m_redUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/uRedUpdate.shader");
    m_redUVelocityShader->linkProgram();

    m_redVVelocityShader = std::make_unique<ShaderProgram>();
    m_redVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/VRedUpdate.shader");
    m_redVVelocityShader->linkProgram();

    m_blackUVelocityShader = std::make_unique<ShaderProgram>();
    m_blackUVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/UBlackUpdate.shader");
    m_blackUVelocityShader->linkProgram();

    m_blackVVelocityShader = std::make_unique<ShaderProgram>();
    m_blackVVelocityShader->addShader(GL_COMPUTE_SHADER, "shaders/grid3d/GaussSeidelSolve/VBlackUpdate.shader");
    m_blackVVelocityShader->linkProgram();
    
    std:: cout << "SettingShaders Fin\n";
}

void Grid3D::restart()
{
    glDeleteTextures(1, &uInTex); 
    glDeleteTextures(1, &vInTex); 
    glDeleteTextures(1, &wInTex); 
    glDeleteTextures(1, &uOutTex); 
    glDeleteTextures(1, &vOutTex); 
    glDeleteTextures(1, &wOutTex); 
    glDeleteTextures(1, &divergenceTex); 
    glDeleteTextures(1, &pressureInTex); 
    glDeleteTextures(1, &pressureOutTex); 
    glDeleteTextures(1, &cellTypeTex); 
    particles.clear();
    setTextures();
    initilizeGrid();
}
