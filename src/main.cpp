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
//Window dimensions
constexpr GLint WIDTH = 800, HEIGHT = 600;


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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // allow foreward compatybility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    //CreateTriangle();

    //compileShaders();

    ShaderProgram program;
    program.addShader(GL_VERTEX_SHADER, "C:/Users/Wojtek/Desktop/Programowanie/Magisterka/FluidSimulator/shaders/location/vertex.shader");
    program.addShader(GL_FRAGMENT_SHADER, "C:/Users/Wojtek/Desktop/Programowanie/Magisterka/FluidSimulator/shaders/location/fragment.shader");
    program.linkProgram();
    //Loop until window close
    vertex3D vertices[] = {{{-1.0f, -0.5f, 0.0f},{1.0f, 0.0f, 0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}},
                                {{1.0f, -1.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}}, 
                                {{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 1.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}},
                                {{-1.0f, 1.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f,0.0f},{0.0f,0.0f,0.0f}}};

    int vertices_size = sizeof(vertices)/sizeof(vertices[0]);
    std::cout << vertices_size << "\n";
    std::vector<vertex3D> vertices_vect(vertices_size);
    std::copy(vertices,vertices+vertices_size,vertices_vect.begin());
    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);
    
    Mesh mesh(vertices_vect, indices);
    mesh.bind(0,-1,-1,1);
    MeshLoader loader;
    std::shared_ptr<Mesh> cube_ptr = std::make_shared<Mesh>(loader.load("C:/Users/Wojtek/Desktop/Programowanie/projektyCpp/opengl/obj/cube.obj"));
    cube_ptr->bind(0,-1,-1,1);
    Object cube_obj(cube_ptr);
    std::shared_ptr<PhysicalProperties> prop = std::make_shared<PhysicalProperties>(1.0f, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f,0.0f,0.0f));
    cube_obj.addProperty("physics", prop);
    std::unique_ptr<rendering::PerspectiveCamera> camera(new rendering::PerspectiveCamera(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0),
        float(WIDTH)/float(HEIGHT), 0.1f, 100.0f, 45.0f));
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
    ObjectMenager objectMeneger;
    std::vector<Mesh> meshes;
    SceneLoader<SceneLoaderObj>::loadScene("C:/Users/Wojtek/Desktop/Programowanie/Magisterka/FluidSimulator/obj/", 
                            objectMeneger, meshes);
    std::cout << meshes.size() << " mothefff\n";
    std::cout << &meshes << "addres vector" << &(meshes.back()) << " pointer";
    rendering::CameraHandler::setActiveCamera(std::move(camera));
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
    rendering::CameraHandler::connectCallbacs(mainWindow);
    int vertexColorLocation = program.getUniformLocation("inColor");
    float lastFrame = 0.0f;
    //cube_obj.uniform_scale(2.0,glm::vec3(1.0f,1.0f,1.0f));
    GLuint uniform_MVP_id = glGetUniformLocation(program.getProgramId(), "MVP");
    float currentFrame = static_cast<float>(glfwGetTime());
    //objectMeneger.addObject("Cube", std::make_unique<Object>(cube_obj));
    for (auto& mesha : meshes)
            {
                std::cout <<"trying draw\n";
                mesha.bind(0,-1,-1,1);
            }
    while (!glfwWindowShouldClose(mainWindow))
    {
        float timeValue = glfwGetTime();
        // get + handel user input events
        glfwPollEvents();
        rendering::CameraHandler::processMovement(mainWindow);
        currentFrame = static_cast<float>(glfwGetTime());
        rendering::CameraHandler::setCurrentSpeed(currentFrame - lastFrame);
        prop->setForce(glm::vec3(0,-glm::cos(timeValue),0));
        cube_obj.updateProperties(currentFrame - lastFrame);
        lastFrame = currentFrame;
        // clear window
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program.getProgramId());
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        //cube_obj.rotate(1.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        objectMeneger.rotate("cube", glm::vec3(0.0f, 0.0f, 1.0f),1.5);
        objectMeneger.move("cube", glm::vec3(0,0,glm::sin(timeValue)));
        //cube_obj.translate(glm::vec3(0,0,glm::sin(timeValue)));
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        cube_obj.update();
        cube_ptr->computeMVPs(rendering::CameraHandler::calculateMVP);
        cube_ptr->drawInstances(uniform_MVP_id);
        if(static_cast<int64_t>(timeValue)%3 ==0)
        {
            //cube_ptr->computeMVPs(rendering::CameraHandler::calculateMVP);
            //cube_ptr->drawInstances(uniform_MVP_id);
            mesh.draw(uniform_MVP_id, rendering::CameraHandler::calculateMVP);
        }
        else
        { 
            for (auto& mesha : meshes)
            {
                mesha.computeMVPs(rendering::CameraHandler::calculateMVP);
                mesha.drawInstances(uniform_MVP_id);
            }
        }
        ProcessInput(mainWindow, GLFW_KEY_ESCAPE, GLFW_PRESS, closeWindow, mainWindow, GLFW_TRUE);
        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);


    return 0;
}