// main.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "headers/commons/shaderProgram.hpp"
#include "headers/geometry/vertexData.hpp"
#include "headers/geometry/mesh.hpp"
//Window dimensions
constexpr GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

void CreateTriangle()
{
    //GLfloat vertices[] = {
    //    -1.0f, -0.5f, 0.0f,
    //    1.0f, -1.0f, 0.0f,
    //    0.0f, 1.0f, 0.0f
    //};

    PosColVertex vertices[] = {{{-1.0f, -0.5f, 0.0f},{1.0f, 0.0f, 0.0f}},
                                {{1.0f, -1.0f, 0.0f},{0.0f, 1.0f, 0.0f}}, 
                                {{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 1.0f}}};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    program.addShader(GL_VERTEX_SHADER, "../shaders/location/vertex.shader");
    program.addShader(GL_FRAGMENT_SHADER, "../shaders/location/fragment.shader");
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

    int vertexColorLocation = program.getUniformLocation("inColor");
    while (!glfwWindowShouldClose(mainWindow))
    {
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue)/2.0f) +0.5f;
        // get + handel user input events
        glfwPollEvents();

        // clear window
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program.getProgramId());
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        //glBindVertexArray(VAO);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        mesh.draw();
        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);


    return 0;
}