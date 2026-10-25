#include "textureTest.hpp"

void TextureTestContext::processContext(std::shared_ptr<rendering::Camera> camera)
{
    GLuint query;
    glGenQueries(1, &query);

    glBeginQuery(GL_TIME_ELAPSED, query);
    computeShaderProgram->useProgram();
    glBindImageTexture(0, tex, 0, GL_FALSE, 0,
        GL_WRITE_ONLY, GL_RGBA32F);
    computeShaderProgram->setFloat("time", (static_cast<int>(glfwGetTime())%2!=0 ? (glfwGetTime()-double(static_cast<int>(glfwGetTime()))) : 1.0-(glfwGetTime() - double(static_cast<int>(glfwGetTime()))) ));
    glDispatchCompute(TEX_W / 16, TEX_H / 16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // -------------------------------
    // Draw texture
    // -------------------------------
    glClear(GL_COLOR_BUFFER_BIT);

    renderShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    renderShaderProgram->setInt("tex", 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEndQuery(GL_TIME_ELAPSED);

    glGetQueryObjectui64v(query, GL_QUERY_RESULT, &m_computeTime);
}

void TextureTestContext::initContext()
{
	computeShaderProgram = std::make_shared<ShaderProgram>();
	computeShaderProgram->addShader(GL_COMPUTE_SHADER, "shaders/tests/textureTest.shader");
	computeShaderProgram->linkProgram();

	renderShaderProgram = std::make_shared<ShaderProgram>();
	renderShaderProgram->addShader(GL_VERTEX_SHADER, "shaders/tests/vertex.shader");
	renderShaderProgram->addShader(GL_FRAGMENT_SHADER, "shaders/tests/fragment.shader");
	renderShaderProgram->linkProgram();

    tex = createTexture();
    quadVAO = createQuadVAO();
}
