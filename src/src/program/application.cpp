#include "application.hpp"
#include "mainWindow.hpp"
#include "simulationUI.hpp"
#include "camera_handlers.hpp"
#include "keyboard.hpp"

#include "perspectiveCamera.hpp"
#include "orthographicCamera.hpp"
#include "gridContext.hpp"
#include "grid3dContext.hpp"
#include "textureTest.hpp"
#include "mixedContext.hpp"

constexpr GLint WIDTH = 1200, HEIGHT = 900;
constexpr int layoutPos = 0;
constexpr int layoutNormals = 1;

void closeWindow(GLFWwindow* window, int value)
{
    glfwSetWindowShouldClose(window, value);
}

void MyApp::mainLoop()
{
    lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        
        rendering::CameraHandler::processMovement(window);
        currentFrame = glfwGetTime();
        rendering::CameraHandler::setCurrentSpeed(static_cast<float>(currentFrame - lastFrame));
        lastFrame = currentFrame;
        activeContext->processContext(rendering::CameraHandler::getInstance()->getCamera());
        glBindVertexArray(0);
        glUseProgram(0);

        if((rendering::CameraHandler::getInstance())->isRotationOn() == false)
            activeContext->showUI();
        glfwSwapBuffers(window);
        processProgramState();
    }
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
    programState = std::make_shared<ProgramState>(ProgramState::MAIN_MENU);

    rendering::CameraHandler::addActiveCamera(std::make_shared<rendering::PerspectiveCamera>(glm::vec3(0, 2, 25), glm::vec3(0, 1, 0),
        float(WIDTH) / float(HEIGHT), 0.1f, 100.0f, window, 60.0f));
    rendering::CameraHandler::addCamera(std::make_shared<rendering::OrthographicCamera>(glm::vec3(0, 2, 5), glm::vec3(0, 1, 0),
    -5.1315f, 5.1315f,     // left, right
    -2.8867f, 2.8867f,     // bottom, top
     0.1f, 100.0f, window
    ));
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    rendering::CameraHandler::connectCallbacks(window);
    currentFrame = static_cast<float>(glfwGetTime());
    contextMap[ProgramState::SPH_SIM] = std::make_shared<SPHSimulationContext>(window, programState);
    contextMap[ProgramState::MAIN_MENU] = std::make_shared<MainWindow>(window, programState);
    contextMap[ProgramState::GRID_SIM] = std::make_shared<GridSimulationContext>(window, programState);
    contextMap[ProgramState::GRID_SIM_3D] = std::make_shared<Grid3DSimulationContext>(window, programState);
    contextMap[ProgramState::TEXTURE_TEST] = std::make_shared<TextureTestContext>(window, programState);
    contextMap[ProgramState::MIXED_SIM] = std::make_shared<MixedSimulationContext>(window, programState);
    activeContext = contextMap[ProgramState::MAIN_MENU];
    lastProgramState = ProgramState::MAIN_MENU;
    activeContext->initContext();
    setup::setupImGui(window); // needs to be called last, after creating window and opengl context
}

void MyApp::processProgramState()
{
    if (*programState.get() != lastProgramState)
    {
        activeContext = contextMap[*programState.get()];
        lastProgramState = *programState.get();
        activeContext->initContext();
    }
}

void MyApp::initialize()
{
    if (!glfwInit())
    {
        printf("GLFW initialization failed");
        glfwTerminate();
        exit(1);
    }

    setup::setupGLFW(4,6);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Fluid Simulator", NULL, NULL);

    if (!window)
    {
        printf("GLFWwindow creation failed!");
        glfwTerminate();
        exit(1);
    }

    int  bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	printf("Framebuffer: %d x %d\n", bufferWidth, bufferHeight);
    glfwMakeContextCurrent(window);
    // Set context for GLEW to use

    //Allow modern extension feature;
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("Glew initialization failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);
	printf("Framebuffer: %d x %d\n", bufferWidth, bufferHeight);
    
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST); 
    
    glCullFace(GL_BACK);

    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));   
    printf("GLSL_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void MyApp::cleanup()
{
    glUseProgram(0);
    glBindVertexArray(0);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
