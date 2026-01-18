#pragma once
#include "renderer.hpp"
#include "grid3d.hpp"
#include "orthographicCamera.hpp"
#include "perspectiveCamera.hpp"

class  Grid3dRenderer : public BaseRenderer
{
public:
	Grid3dRenderer() {}
	void setupSimulation(std::shared_ptr<Grid3D> grid) { gridSimulation = grid; }
	void render(std::shared_ptr<rendering::Camera> camera) override
	{
		activeProgram->useProgram();
		glActiveTexture(GL_TEXTURE0);
		if(activeProgram == shaderProgram)
			glBindTexture(GL_TEXTURE_3D, gridSimulation->getTypeCell());
		if(activeProgram == vVelocityProgram)
			glBindTexture(GL_TEXTURE_3D, gridSimulation->getvTex());
		if(activeProgram == uVelocityProgram)
			glBindTexture(GL_TEXTURE_3D, gridSimulation->getuTex());
		if(activeProgram == divergenceProgram)
			glBindTexture(GL_TEXTURE_3D, gridSimulation->getDivergenceTex());
		if(activeProgram == pressureProgram)
			glBindTexture(GL_TEXTURE_3D, gridSimulation->getPressureTex());
		if(activeProgram == particleProgram)
		{
			renderParticles(camera);
			return;
		}
		shaderProgram->setInt("tex", 0);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	void setupBackend()
	{
		shaderProgram = std::make_shared<ShaderProgram>();
		shaderProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/vertex.shader");
		shaderProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/fragment.shader");
		shaderProgram->linkProgram();

		vVelocityProgram = std::make_shared<ShaderProgram>();
		vVelocityProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/vertex.shader");
		vVelocityProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/debugShaders/vVelocity.shader");
		vVelocityProgram->linkProgram();

		uVelocityProgram = std::make_shared<ShaderProgram>();
		uVelocityProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/vertex.shader");
		uVelocityProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/debugShaders/uVelocity.shader");
		uVelocityProgram->linkProgram();

		divergenceProgram = std::make_shared<ShaderProgram>();
		divergenceProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/vertex.shader");
		divergenceProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/debugShaders/divergence.shader");
		divergenceProgram->linkProgram();

		pressureProgram = std::make_shared<ShaderProgram>();
		pressureProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/vertex.shader");
		pressureProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/debugShaders/pressureGradient.shader");
		pressureProgram->linkProgram();

		particleProgram = std::make_shared<ShaderProgram>();
		particleProgram->addShader(GL_VERTEX_SHADER, "shaders/grid3d/debugShaders/particleVertex.shader");
		particleProgram->addShader(GL_FRAGMENT_SHADER, "shaders/grid3d/debugShaders/particleFragment.shader");
		particleProgram->linkProgram();

		activeProgram = particleProgram;
		quadVAO = createQuadVAO();
		createParticleVAO();
		//glDisable(GL_PROGRAM_POINT_SIZE);
		//glDisable(GL_CULL_FACE);
    	//glDisable(GL_DEPTH_TEST);
    	//glDisable(GL_BLEND);
	}


	void renderParticles(std::shared_ptr<rendering::Camera> camera)
	{
		particleProgram->useProgram();
		
    	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gridSimulation->getParticleBuffer());
    	glm::mat4 viewMatrix = camera->getView();
    	glm::mat4 projMatrix = camera->getProjection();
    	particleProgram->setMat4("view", viewMatrix);
    	particleProgram->setMat4("proj", projMatrix);
    	particleProgram->setFloat("viewportHeight", camera->getWindowHeight());
    	particleProgram->setUint("numOfParticles", gridSimulation->getNumOfParticles());
    	if(camera->getCameraType() == rendering::CameraType::perspective)
    	{
    	    particleProgram->setBool("perspectiveProjection", true);
    	    particleProgram->setFloat("fovy", static_cast<rendering::PerspectiveCamera*>(camera.get())->getFOV()/180.0f*3.1415); 
    	}
    	else
    	{
    	    particleProgram->setBool("perspectiveProjection", false);
    	    particleProgram->setFloat("top", static_cast<rendering::OrthographicCamera*>(camera.get())->getTop());
    	    particleProgram->setFloat("bottom", static_cast<rendering::OrthographicCamera*>(camera.get())->getBottom());
    	}
    	particleProgram->setVec3("color", glm::vec3(0.0, 0.3, 0.8));
    	particleProgram->setFloat("particleRadius", gridSimulation->getDx()/float(cbrt(gridSimulation->getParticlesPerCell())));
    	glBindVertexArray(particleVAO);
    	glDrawArraysInstanced(GL_POINTS, 0, 1, gridSimulation->getNumOfParticles());
    	glBindVertexArray(0);
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

	void createParticleVAO()
	{
		GLuint dummyVBO;
    	float dummy = 0.0f;

    	glGenBuffers(1, &dummyVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, dummyVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(float), &dummy, GL_STATIC_DRAW);

    	glGenVertexArrays(1, &particleVAO);
    	glBindVertexArray(particleVAO);
    	glEnableVertexAttribArray(0);
    	glVertexAttribPointer(
    	    0,              // location
    	    1,              // size
    	    GL_FLOAT,
    	    GL_FALSE,
    	    sizeof(float),
    	    (void*)0
    	);
    	glBindVertexArray(0);

    	glEnable(GL_PROGRAM_POINT_SIZE);
    	glEnable(GL_DEPTH_TEST);
    	glDepthFunc(GL_LESS);  // Default: passes if incoming depth < stored depth
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void setUVelTexActive(){activeProgram = uVelocityProgram;}
	void setVVelTexActive(){activeProgram = vVelocityProgram;}
	void setdivergenceTexActive(){activeProgram = divergenceProgram;}
	void setPressureTexActive(){activeProgram = pressureProgram;}
	void setDefaultView(){activeProgram = shaderProgram;}
	void setParticleView()
	{
		activeProgram = particleProgram;
		//glEnable(GL_PROGRAM_POINT_SIZE);
    	//glEnable(GL_DEPTH_TEST);
    	//glDepthFunc(GL_LESS);  // Default: passes if incoming depth < stored depth
    	//glEnable(GL_BLEND);
    	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
private:
	std::shared_ptr<ShaderProgram> activeProgram;
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::shared_ptr<ShaderProgram> uVelocityProgram;
	std::shared_ptr<ShaderProgram> vVelocityProgram;
	std::shared_ptr<ShaderProgram> divergenceProgram;
	std::shared_ptr<ShaderProgram> pressureProgram;
	std::shared_ptr<ShaderProgram> particleProgram;
	std::shared_ptr<Grid3D> gridSimulation;

	GLuint particleVAO = 0;
	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
};
