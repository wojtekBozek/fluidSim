// main.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shaderProgram.hpp"
#include "vertexData.hpp"
#include "mesh.hpp"
#include "mesh_loader.hpp"
#include "camera3D.hpp"
#include "camera_handlers.hpp"
#include "object.hpp"
#include "objectsMeneger.hpp"
#include "sceneLoader.hpp"
#include "physics.hpp"
#include "setup.hpp"
#include "mainWindow.hpp"
#include "simulationUI.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "positionedLight.hpp"

//Window dimensions
constexpr GLint WIDTH = 1200, HEIGHT = 900;
constexpr int layoutPos = 0;
constexpr int layoutNormals = 1;

void closeWindow(GLFWwindow* window, int value)
{
    glfwSetWindowShouldClose(window, value);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        printf("GLFW initialization failed");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    //Opengl version

    setup::setupGLFW();

    GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "First window", NULL, NULL);

    if (!mainWindow)
    {
        printf("GLFWwindow creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int  bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    //Allow modern extension feature;
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("Glew initialization failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    
    //Setup Viewport Size

    glViewport(0, 0, bufferWidth, bufferHeight);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);  

    ShaderProgram program;
    program.addShader(GL_VERTEX_SHADER, "shaders/location/vertex.shader");
    program.addShader(GL_FRAGMENT_SHADER, "shaders/location/fragment.shader");
    program.linkProgram();
    
    MeshLoader loader;
    
    std::shared_ptr<PhysicalProperties> prop = std::make_shared<PhysicalProperties>(1.0f, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f,0.0f,0.0f));
    std::shared_ptr<PhysicalProperties> prop2 = std::make_shared<PhysicalProperties>(1.0f, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));

    auto state = std::make_shared<ProgramState>(ProgramState::MAIN_MENU);
    std::vector<std::shared_ptr<BaseUI>> UIs;
    UIs.push_back(std::make_shared<MainWindow>(mainWindow,state));
    UIs.push_back(std::make_shared<SimulationUI>(mainWindow,state));
    std::unique_ptr<rendering::PerspectiveCamera> camera(new rendering::PerspectiveCamera(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0),
        float(WIDTH)/float(HEIGHT), 0.1f, 100.0f, 45.0f));
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
    ObjectMenager objectMeneger;
    std::vector<std::shared_ptr<Mesh>> meshes;
    SceneLoader<SceneLoaderObj>::loadScene("obj/", 
                            objectMeneger, meshes);
                            
    rendering::CameraHandler::setActiveCamera(std::move(camera));
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
    rendering::CameraHandler::connectCallbacs(mainWindow);
    int vertexColorLocation = program.getUniformLocation("inColor");
    float lastFrame = 0.0f;

    GLuint uniform_MVP_id = glGetUniformLocation(program.getProgramId(), "MVP");
    float currentFrame = static_cast<float>(glfwGetTime());
    for (auto& mesh : meshes)
    {
        mesh->bind(layoutPos, layoutNormals);
    }
    
    objectMeneger.addProperty("cube", "physics", prop2);
    
    setup::setupImGui(mainWindow);
    
    glCullFace(GL_BACK);

    PositionedLight light;
    light.position = glm::vec3(-10.00f, 5.0f, 10.0f);
    light.ambient = glm::vec3(0.1f);
    light.diffuse = glm::vec3(0.8f);
    light.specular = glm::vec3(1.0f);

    while (!glfwWindowShouldClose(mainWindow))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

        float timeValue = static_cast<float>(glfwGetTime());
        // get + handel user input events
        glfwPollEvents();
        // (Your code calls glfwPollEvents())
        // ...
        // Start the Dear ImGui frame
        
        
        rendering::CameraHandler::processMovement(mainWindow);
        currentFrame = static_cast<float>(glfwGetTime());
        rendering::CameraHandler::setCurrentSpeed(currentFrame - lastFrame);
        prop2->applyForces(glm::vec3(0,-glm::cos(timeValue),0));
        objectMeneger.update(currentFrame - lastFrame);
        lastFrame = currentFrame;
        
        objectMeneger.rotate("cube", glm::vec3(0.0f, 0.0f, 1.0f),1.5);
        //objectMeneger.move("cube", glm::vec3(0,0,glm::sin(timeValue)));

        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program.getProgramId());

        for (auto& mesh : meshes)
        {
            
            mesh->computeMVPs(rendering::CameraHandler::calculateMVP);
            mesh->draw(program, light, rendering::CameraHandler::getInstance()->getCameraViewPosition());
        }
        
        glBindVertexArray(0);

        glUseProgram(0);

        ProcessInput(mainWindow, GLFW_KEY_ESCAPE, GLFW_PRESS, closeWindow, mainWindow, GLFW_TRUE);
        for(const auto& ui : UIs)
        {
            ui->showUI();
        }
        // Rendering
        // (Your code clears your framebuffer, renders your other stuff etc.)
        
        // (Your code calls glfwSwapBuffers() etc.)
        glfwSwapBuffers(mainWindow);
    }
    
    glUseProgram(0);
    glBindVertexArray(0);
    objectMeneger.clearObjects();
    meshes.clear();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 0;
}