#pragma once
#include "baseGrid.hpp"
#include "shaderProgram.hpp"
#include <memory>

class Grid3D : public BaseGrid
{
public:
	void run() override;
	void setup() override;
	void setDimensions(uint32_t x, uint32_t y, uint32_t z);
	void setCellSize(float x);
	void setTimeStep(float t){dt = t;}
	void setPressureIterations(uint32_t iter){pressureIterations = iter;}
	void setBorderSize(uint32_t size){borderSize = size;}
	void setFluidInitPosition(uint32_t x, uint32_t y, uint32_t z){initFluidX = x; initFluidY = y; initFluidZ = z;}
	void setFluidSize(uint32_t dx, uint32_t dy, uint32_t dz){initFluidHeight = dy; initFluidWidth = dx; initFluidDepth = dz;}
	void initilizeGrid();

	void setParticlesPerCell(uint8_t particles){particlesPerCell = particles;}

	enum SOLVER
	{
		JACOBI,
		GS_PRESSURE,
		GS_DIVERGENCE
	};
	void setSolver(SOLVER solver)
	{
		m_solver = solver;
		if(solver == SOLVER::JACOBI)
		{
			m_overrelaxation = 0.7f;
		}
		else
		{
			m_overrelaxation = 1.7f;
		}
	}

	void setOverrelaxation(float overrelaxation)
	{
		m_overrelaxation = overrelaxation;
	}

	SOLVER getSolver() {return m_solver;}
	float getOverrelaxation(){return m_overrelaxation;}
	void setDensity(float dens){m_dens = dens;}
	float getDensity(){ return m_dens; };
	void setTextures();
	void setShaders();
	void restart();
	void setPressureZero(bool value){zeroPressure = value;}
	bool getPressurePolicy(){return zeroPressure;}
	uint32_t getNumOfParticles(){return particles.size();}
	GLuint getTypeCell() { return cellTypeTex; }
	GLuint getuTex() { return uInTex; }
	GLuint getvTex() { return vInTex; }
	GLuint getDivergenceTex() { return divergenceTex; }
	GLuint getPressureTex() { return pressureInTex; }
	GLuint64 getComputeTime() {return m_computeTime;}
	GLuint64 getParticleBuffer() {return m_particleBuffer;}
	uint8_t getParticlesPerCell() {return particlesPerCell;}
	glm::uvec3 getSize(){return glm::uvec3(nx,ny,nz);}
	glm::uvec3 getFluidSize() {return glm::uvec3(initFluidWidth, initFluidHeight,initFluidDepth);}
	glm::uvec3 getFluidPosition() {return glm::uvec3(initFluidX, initFluidY,initFluidZ);}
	float getDx() {return dx;}
	uint32_t getBorderSize() {return borderSize;}
	float getTimeStep() {return dt;}
	uint32_t getPSolverIterations() {return pressureIterations;}

	const std::vector<glm::vec4>& getParticles() const;
private:
	void JacobiSolver();
	void GaussSiedelSolver();
	void GaussSiedelPressureSolver();
	SOLVER m_solver = SOLVER::JACOBI;
	float m_overrelaxation = 0.7;
	float m_dens = 1.0;
	GLuint floatSSBO, floatSSBO2;
	const GLuint FLUID = 0u;
	const GLuint AIR = 1u;
	const GLuint SOLID = 2u;
	uint32_t initFluidX = 5, initFluidY=5,initFluidZ=5;
	uint32_t borderSize = 3;
	uint32_t pressureIterations = 200;
	uint32_t initFluidWidth=1, initFluidHeight=1, initFluidDepth=1;
	uint32_t nx = 64, ny=64, nz=64;
	float dx = 0.5f;
	float dt = 0.0025f;
	uint8_t particlesPerCell = 27;
	GLuint uInTex, uOutTex, vInTex, vOutTex, wInTex, wOutTex;
	GLuint pressureInTex, pressureOutTex;
	GLuint divergenceTex;
	GLuint cellTypeTex;
	std::vector<glm::vec4> particles;
	bool zeroPressure = false;
	GLuint m_particleBuffer;
	GLuint64 m_computeTime;

	std::unique_ptr<ShaderProgram> m_clearFluidShader;
	std::unique_ptr<ShaderProgram> m_velocityVAdvectionShader;
	std::unique_ptr<ShaderProgram> m_velocityUAdvectionShader;
	std::unique_ptr<ShaderProgram> m_velocityWAdvectionShader;
	std::unique_ptr<ShaderProgram> m_divergenceShader;
	std::unique_ptr<ShaderProgram> m_jacobiPSolverShader;
	std::unique_ptr<ShaderProgram> m_gsRedPSolverShader;
	std::unique_ptr<ShaderProgram> m_gsBlackPSolverShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionUShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionVShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionWShader;
	std::unique_ptr<ShaderProgram> m_boundaryShader;
	std::unique_ptr<ShaderProgram> m_particleAdvectionShader;
	std::unique_ptr<ShaderProgram> m_cellUpdateShader;
	std::unique_ptr<ShaderProgram> m_addForcesShader;
	std::unique_ptr<ShaderProgram> m_maxVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateUVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateVVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateWVelocityShader;

	std::unique_ptr<ShaderProgram> m_gaussSiedelDivergenceShader;
	std::unique_ptr<ShaderProgram> m_redUVelocityShader;
	std::unique_ptr<ShaderProgram> m_redVVelocityShader;
	std::unique_ptr<ShaderProgram> m_blackUVelocityShader;
	std::unique_ptr<ShaderProgram> m_blackVVelocityShader;

	float time = 0.0f;
};