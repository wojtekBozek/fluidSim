#include "texture3dTest.hpp"

Texture3DTestContext::Texture3DTestContext(
    GLFWwindow* window,
    std::shared_ptr<ProgramState> state)
    : AbstractContext(window, state)
{
}

Texture3DTestContext::~Texture3DTestContext()
{
    if (texture3D)
        glDeleteTextures(1, &texture3D);

    if (computeProgram)
        glDeleteProgram(computeProgram);
}

static const char* computeShaderSource = R"(

#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, r32f) uniform image3D tex;

uniform float delta;

void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

    float value = imageLoad(tex, coord).r;

    value += delta;

    imageStore(tex, coord, vec4(value,0,0,0));
}

)";

GLuint Texture3DTestContext::compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[2048];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

        std::cout << log << std::endl;
    }

    return shader;
}

GLuint Texture3DTestContext::createComputeProgram()
{
    GLuint shader = compileShader(GL_COMPUTE_SHADER, computeShaderSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);

        std::cout << log << std::endl;
    }

    glDeleteShader(shader);

    return program;
}

void Texture3DTestContext::initContext()
{
    computeProgram = createComputeProgram();

    deltaLocation = glGetUniformLocation(computeProgram, "delta");

    glGenTextures(1, &texture3D);

    glBindTexture(GL_TEXTURE_3D, texture3D);

    constexpr int SIZE = 4;

    float data[SIZE * SIZE * SIZE] = {};

    data[0] = 5.0f;

    glTexStorage3D(
        GL_TEXTURE_3D,
        1,
        GL_R32F,
        SIZE,
        SIZE,
        SIZE);

    glTexSubImage3D(
        GL_TEXTURE_3D,
        0,
        0,
        0,
        0,
        SIZE,
        SIZE,
        SIZE,
        GL_RED,
        GL_FLOAT,
        data);

    glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3DTestContext::processContext(
    std::shared_ptr<rendering::Camera>)
{
    //if (executed)
    //    return;
//
    //executed = true;

    glUseProgram(computeProgram);

    glUniform1f(deltaLocation, 2.5f);

    glBindImageTexture(
        0,
        texture3D,
        0,
        GL_TRUE,
        0,
        GL_READ_WRITE,
        GL_R32F);

    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindTexture(GL_TEXTURE_3D, texture3D);

    float value = 0.0f;

    float data[64];
    glGetTexImage(
        GL_TEXTURE_3D,
        0,
        GL_RED,
        GL_FLOAT,
        data);

    value = data[0];

    std::cout << "Texture[0,0,0] = "
              << value
              << std::endl;
}

void Texture3DTestContext::exitButton()
{
    if (ImGui::Button("Main Menu"))
    {
        *m_state = ProgramState::MAIN_MENU;
    }
}

void Texture3DTestContext::showUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("3DTextureTest");
    exitButton();
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
