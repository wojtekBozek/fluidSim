#pragma once
#include "baseGrid.hpp"
#include "shaderProgram.hpp"
#include <memory>

class Grid2D : public BaseGrid
{
public:
	void run() override;
	void setDimensions(uint32_t x, uint32_t y);
	void setCellSize(double x);
	void initilizeGrid();

	void setTextures();
	void setShaders();

	const std::vector<glm::vec2>& getParticles() const;
private:

	const GLubyte FLUID = 0u;
	const GLubyte AIR = 1u;
	const GLubyte SOLID = 2u;
	uint32_t initFluidX = 0, initFluidY=0;
	uint32_t initFluidWidth=128, initFluidHeight=128;
	uint32_t nx = 512, ny=512;
	float dx = 0.5;
	float dt = 0.001;
	GLuint uInTex, uOutTex, vInTex, vOutTex;
	GLuint pressureInTex, pressureOutTex;
	GLuint divergenceTex;
	GLuint CellTypeTex;
	std::vector<glm::vec2> particles;

	GLuint m_particleBuffer;

	std::unique_ptr<ShaderProgram> m_initShader;
	std::unique_ptr<ShaderProgram> m_velocityAdvectionShader;
	std::unique_ptr<ShaderProgram> m_divergenceShader;
	std::unique_ptr<ShaderProgram> m_jacobiPSolverShader;
	std::unique_ptr<ShaderProgram> m_pressureProjectionShader;
	std::unique_ptr<ShaderProgram> m_boundaryShader;
	//std::unique_ptr<ShaderProgram> m_particleAdvectionShader;
	std::unique_ptr<ShaderProgram> m_cellUpdateShader;
	std::unique_ptr<ShaderProgram> m_addForcesShader;
};