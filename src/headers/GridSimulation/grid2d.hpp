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
	void setCellSize(double x);
	void initilizeGrid();

	void setTextures();
	void setShaders();
	void restart();
	GLuint getTypeCell() { return cellTypeTex; }
	GLuint getuTex() { return uInTex; }
	GLuint getvTex() { return vInTex; }
	GLuint getDivergenceTex() { return divergenceTex; }
	GLuint getPressureTex() { return pressureInTex; }
	GLuint64 getComputeTime() {return m_computeTime;}

	const std::vector<glm::vec2>& getParticles() const;
private:
	GLuint floatSSBO, floatSSBO2;
	const GLuint FLUID = 0u;
	const GLuint AIR = 1u;
	const GLuint SOLID = 2u;
	uint32_t initFluidX = 0, initFluidY=0;
	uint8_t pressureIterations = 40;
	uint32_t initFluidWidth=200, initFluidHeight=200;
	uint32_t nx = 256, ny=256;
	float dx = 0.5f;
	float dt = 0.001f;
	uint8_t particlesPerCell = 4;
	GLuint uInTex, uOutTex, vInTex, vOutTex;
	GLuint pressureInTex, pressureOutTex;
	GLuint divergenceTex;
	GLuint cellTypeTex;
	std::vector<glm::vec2> particles;

	GLuint m_particleBuffer;
	GLuint64 m_computeTime;

	std::unique_ptr<ShaderProgram> m_clearFluidShader;
	std::unique_ptr<ShaderProgram> m_velocityVAdvectionShader;
	std::unique_ptr<ShaderProgram> m_velocityUAdvectionShader;
	std::unique_ptr<ShaderProgram> m_divergenceShader;
	std::unique_ptr<ShaderProgram> m_jacobiPSolverShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionUShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionVShader;
	std::unique_ptr<ShaderProgram> m_boundaryShader;
	std::unique_ptr<ShaderProgram> m_particleAdvectionShader;
	std::unique_ptr<ShaderProgram> m_cellUpdateShader;
	std::unique_ptr<ShaderProgram> m_addForcesShader;
	std::unique_ptr<ShaderProgram> m_maxVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateUVelocityShader;
	std::unique_ptr<ShaderProgram> m_extrapolateVVelocityShader;

	float time = 0.0f;
};