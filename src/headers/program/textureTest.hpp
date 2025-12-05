#pragma once
#include "context.hpp"
#include "shaderProgram.hpp"
#include "renderer.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

static const int TEX_W = 512;
static const int TEX_H = 512;


class TextureTestContext : public AbstractContext
{
public:
	TextureTestContext(GLFWwindow* window, std::shared_ptr<ProgramState> state) : AbstractContext(window, state) {}
	void processContext(std::shared_ptr<rendering::Camera> camera) override;
	void initContext() override;
    void exitButton() {
        if (ImGui::Button("Main Menu"))
        {
            *m_state = ProgramState::MAIN_MENU;
        }
    }
	void showUI() override
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("TextureTest");
        exitButton(); 
        float fr = 1.0f / (static_cast<float>(m_computeTime) / 1000000000.0f);

        ImGui::Value("FrameRate", fr, "%.001f");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
    GLuint createTexture()
    {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, TEX_W, TEX_H);

        // Fill with zeros
        std::vector<float> empty(TEX_W * TEX_H * 4, 0.0f);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEX_W, TEX_H,
            GL_RGBA, GL_FLOAT, empty.data());
        return tex;
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
	std::shared_ptr<ContextState> state;
	std::shared_ptr<ShaderProgram> computeShaderProgram;
	std::shared_ptr<ShaderProgram> renderShaderProgram;

    GLuint tex;
    GLuint quadVAO;
    GLuint64 m_computeTime;
};