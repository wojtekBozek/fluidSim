#pragma once
#include "baseGrid.hpp"
#include "shaderProgram.hpp"
#include <memory>

class Grid2D : public BaseGrid
{
public:
	void run() override;
	void setup() override;
	void setDimensions(uint32_t x, uint32_t y);
	void setCellSize(float x);
	void setTimeStep(float t){dt = t;}
	void setPressureIterations(uint32_t iter){pressureIterations = iter;}
	void setBorderSize(uint32_t size){borderSize = size;}
	void setFluidInitPosition(uint32_t x, uint32_t y){initFluidX = x; initFluidY = y;}
	void setFluidSize(uint32_t dx, uint32_t dy){initFluidHeight = dy; initFluidWidth = dx;};
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
	glm::uvec2 getSize(){return glm::uvec2(nx,ny);}
	glm::uvec2 getFluidSize() {return glm::uvec2(initFluidWidth, initFluidHeight);}
	glm::uvec2 getFluidPosition() {return glm::uvec2(initFluidX, initFluidY);}
	float getDx() {return dx;}
	uint32_t getBorderSize() {return borderSize;}
	float getTimeStep() {return dt;}
	uint32_t getPSolverIterations() {return pressureIterations;}

	const std::vector<glm::vec2>& getParticles() const;
private:
	void JacobiSolver();
	void GaussSiedelSolver();
	void GaussSiedelPressureSolver();
	SOLVER m_solver = SOLVER::JACOBI;
	float m_overrelaxation = 0.7;
	GLuint floatSSBO, floatSSBO2;
	const GLuint FLUID = 0u;
	const GLuint AIR = 1u;
	const GLuint SOLID = 2u;
	uint32_t initFluidX = 5, initFluidY=5;
	uint32_t borderSize = 3;
	uint32_t pressureIterations = 200;
	uint32_t initFluidWidth=200, initFluidHeight=200;
	uint32_t nx = 256, ny=256;
	float dx = 0.05f;
	float dt = 0.0025f;
	uint8_t particlesPerCell = 16;
	GLuint uInTex, uOutTex, vInTex, vOutTex;
	GLuint pressureInTex, pressureOutTex;
	GLuint divergenceTex;
	GLuint cellTypeTex;
	std::vector<glm::vec2> particles;
	bool zeroPressure = false;
	GLuint m_particleBuffer;
	GLuint64 m_computeTime;

	std::unique_ptr<ShaderProgram> m_clearFluidShader;
	std::unique_ptr<ShaderProgram> m_velocityVAdvectionShader;
	std::unique_ptr<ShaderProgram> m_velocityUAdvectionShader;
	std::unique_ptr<ShaderProgram> m_divergenceShader;
	std::unique_ptr<ShaderProgram> m_jacobiPSolverShader;
	std::unique_ptr<ShaderProgram> m_gsRedPSolverShader;
	std::unique_ptr<ShaderProgram> m_gsBlackPSolverShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionUShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionVShader;
	std::unique_ptr<ShaderProgram> m_boundaryShader;
	std::unique_ptr<ShaderProgram> m_particleAdvectionShader;
	std::unique_ptr<ShaderProgram> m_cellUpdateShader;
	std::unique_ptr<ShaderProgram> m_addForcesShader;
	std::unique_ptr<ShaderProgram> m_maxVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateUVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateVVelocityShader;

	std::unique_ptr<ShaderProgram> m_gaussSiedelDivergenceShader;
	std::unique_ptr<ShaderProgram> m_redUVelocityShader;
	std::unique_ptr<ShaderProgram> m_redVVelocityShader;
	std::unique_ptr<ShaderProgram> m_blackUVelocityShader;
	std::unique_ptr<ShaderProgram> m_blackVVelocityShader;

	float time = 0.0f;
};