#include "application.hpp"
#include "mainWindow.hpp"
#include "simulationUI.hpp"
#include "camera_handlers.hpp"
#include "keyboard.hpp"

constexpr GLint WIDTH = 1200, HEIGHT = 900;
constexpr int layoutPos = 0;
constexpr int layoutNormals = 1;

void closeWindow(GLFWwindow* window, int value)
{
    glfwSetWindowShouldClose(window, value);
}

void MyApp::run()
{
    initialize();
    setupResources();
    mainLoop();
    cleanup();
}

void MyApp::setupResources()   
{
    objectsMenager = std::make_shared<ObjectMenager>();
    shaderProgram = std::make_shared<ShaderProgram>();
    
    programState = std::make_shared<ProgramState>(ProgramState::MAIN_MENU);
    UIs.push_back(std::make_shared<MainWindow>(window, programState));
    UIs.push_back(std::make_shared<SimulationUI>(window, programState));
    camera = std::make_shared<rendering::PerspectiveCamera>(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0),
        float(WIDTH)/float(HEIGHT), 0.1f, 100.0f, 45.0f);
    

    rendering::CameraHandler::setActiveCamera(camera);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    rendering::CameraHandler::connectCallbacks(window);


    shaderProgram->addShader(GL_VERTEX_SHADER, "shaders/location/vertex.shader");
    shaderProgram->addShader(GL_FRAGMENT_SHADER, "shaders/location/fragment.shader");
    shaderProgram->linkProgram();

    SceneLoader<SceneLoaderObj>::loadScene("obj/", 
                            *objectsMenager, meshes);

    for (auto& mesh : meshes)
    {
        mesh->bind(layoutPos, layoutNormals);
    }
    light = std::make_shared<PositionedLight>();
    light->position = glm::vec3(-10.00f, 5.0f, 10.0f);
    light->ambient = glm::vec3(0.1f);
    light->diffuse = glm::vec3(0.8f);
    light->specular = glm::vec3(1.0f);
    currentFrame = static_cast<float>(glfwGetTime());
    renderer = std::make_shared<MeshRenderer>(meshes, shaderProgram, light, camera);
}

void MyApp::initialize()
{
    if (!glfwInit())
    {
        printf("GLFW initialization failed");
        glfwTerminate();
        exit(1);
    }

    setup::setupGLFW();

    window = glfwCreateWindow(WIDTH, HEIGHT, "First window", NULL, NULL);

    if (!window)
    {
        printf("GLFWwindow creation failed!");
        glfwTerminate();
        exit(1);
    }

    int  bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(window);

    //Allow modern extension feature;
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("Glew initialization failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }
    glViewport(0, 0, bufferWidth, bufferHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST); 
    glCullFace(GL_BACK);
    
    setup::setupImGui(window);
}

void MyApp::mainLoop()
{
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
        


        glfwPollEvents();

        rendering::CameraHandler::processMovement(window);
        currentFrame = static_cast<float>(glfwGetTime());
        rendering::CameraHandler::setCurrentSpeed(currentFrame - lastFrame);
        lastFrame = currentFrame;
        
        objectsMenager->rotate("cube", glm::vec3(0.0f, 0.0f, 1.0f),1.5);
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram->getProgramId());
        renderer->render();

        glBindVertexArray(0);
        glUseProgram(0);

        ProcessInput(window, GLFW_KEY_ESCAPE, GLFW_PRESS, closeWindow, this->window, GLFW_TRUE);
        for(const auto& ui : UIs)
        {
            ui->showUI();
        }
        glfwSwapBuffers(window);
    }
}

void MyApp::cleanup()
{
    glUseProgram(0);
    glBindVertexArray(0);
    objectsMenager->clearObjects();
    meshes.clear();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
