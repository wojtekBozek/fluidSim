#pragma once
#include "renderer.hpp"
#include "grid2d.hpp"
class  GridRenderer : public BaseRenderer
{
public:
	GridRenderer() {}
	void setupSimulation(std::shared_ptr<Grid2D> grid) { gridSimulation = grid; }
	void render(std::shared_ptr<rendering::Camera> camera) override
	{
		activeProgram->useProgram();
		glActiveTexture(GL_TEXTURE0);
		if(activeProgram == shaderProgram)
			glBindTexture(GL_TEXTURE_2D, gridSimulation->getTypeCell());
		if(activeProgram == vVelocityProgram)
			glBindTexture(GL_TEXTURE_2D, gridSimulation->getvTex());
		if(activeProgram == uVelocityProgram)
			glBindTexture(GL_TEXTURE_2D, gridSimulation->getuTex());
		if(activeProgram == divergenceProgram)
			glBindTexture(GL_TEXTURE_2D, gridSimulation->getDivergenceTex());
		if(activeProgram == pressureProgram)
			glBindTexture(GL_TEXTURE_2D, gridSimulation->getPressureTex());

		shaderProgram->setInt("tex", 0);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void setupBackend()
	{
		shaderProgram = std::make_shared<ShaderProgram>();
		shaderProgram->addShader(GL_VERTEX_SHADER, "shaders/gridFluid/vertex.shader");
		shaderProgram->addShader(GL_FRAGMENT_SHADER, "shaders/gridFluid/fragment.shader");
		shaderProgram->linkProgram();

		vVelocityProgram = std::make_shared<ShaderProgram>();
		vVelocityProgram->addShader(GL_VERTEX_SHADER, "shaders/gridFluid/vertex.shader");
		vVelocityProgram->addShader(GL_FRAGMENT_SHADER, "shaders/gridFluid/debugShaders/vVelocity.shader");
		vVelocityProgram->linkProgram();

		uVelocityProgram = std::make_shared<ShaderProgram>();
		uVelocityProgram->addShader(GL_VERTEX_SHADER, "shaders/gridFluid/vertex.shader");
		uVelocityProgram->addShader(GL_FRAGMENT_SHADER, "shaders/gridFluid/debugShaders/uVelocity.shader");
		uVelocityProgram->linkProgram();

		divergenceProgram = std::make_shared<ShaderProgram>();
		divergenceProgram->addShader(GL_VERTEX_SHADER, "shaders/gridFluid/vertex.shader");
		divergenceProgram->addShader(GL_FRAGMENT_SHADER, "shaders/gridFluid/debugShaders/divergence.shader");
		divergenceProgram->linkProgram();

		pressureProgram = std::make_shared<ShaderProgram>();
		pressureProgram->addShader(GL_VERTEX_SHADER, "shaders/gridFluid/vertex.shader");
		pressureProgram->addShader(GL_FRAGMENT_SHADER, "shaders/gridFluid/debugShaders/pressureGradient.shader");
		pressureProgram->linkProgram();

		activeProgram = shaderProgram;
		quadVAO = createQuadVAO();
		glDisable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_CULL_FACE);
    	glDisable(GL_DEPTH_TEST);
    	glDisable(GL_BLEND);
	}

	GLuint createQuadVAO()
	{
		float quad[] =
		{
			-1, -1,
			 1, -1,
			-1,  1,
			 1,  1
		};

		GLuint vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*)0);

		return vao;
	}

	void setUVelTexActive(){activeProgram = uVelocityProgram;}
	void setVVelTexActive(){activeProgram = vVelocityProgram;}
	void setdivergenceTexActive(){activeProgram = divergenceProgram;}
	void setPressureTexActive(){activeProgram = pressureProgram;}
	void setDefaultView(){activeProgram = shaderProgram;}
private:
	std::shared_ptr<ShaderProgram> activeProgram;
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::shared_ptr<ShaderProgram> uVelocityProgram;
	std::shared_ptr<ShaderProgram> vVelocityProgram;
	std::shared_ptr<ShaderProgram> divergenceProgram;
	std::shared_ptr<ShaderProgram> pressureProgram;
	std::shared_ptr<Grid2D> gridSimulation;

	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
};
