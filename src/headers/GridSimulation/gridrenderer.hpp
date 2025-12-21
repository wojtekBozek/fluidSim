#include "renderer.hpp"
#include "grid2d.hpp"
class  GridRenderer : public BaseRenderer
{
public:
	GridRenderer() {}
	void setupSimulation(std::shared_ptr<Grid2D> grid) { gridSimulation = grid; }
	void render(std::shared_ptr<rendering::Camera> camera) override
	{
		shaderProgram->useProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gridSimulation->getTypeCell());
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

		quadVAO = createQuadVAO();
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

private:

	std::shared_ptr<ShaderProgram> shaderProgram;
	std::shared_ptr<Grid2D> gridSimulation;

	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
};
