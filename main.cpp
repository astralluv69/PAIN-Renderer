#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "vao.h"
#include "ebo.h"

int width = 1280;
int height = 720;
const char* title = "PAIN Renderer";
GLFWwindow* window;

Vertex testVertices[] = {
                      //Coordinates                   //Color
    Vertex{glm::vec3(-0.5f, -0.5f, 0.0f),   glm::vec3(1.0f, 0.0f, 0.0f)},  //Vertex 0
    Vertex{glm::vec3(-0.5f,  0.5f, 0.0f),   glm::vec3(0.0f, 1.0f, 0.0f)},  //Vertex 1
    Vertex{glm::vec3( 0.5f,  0.5f, 0.0f),   glm::vec3(0.0f, 0.0f, 1.0f)},  //Vertex 2
    Vertex{glm::vec3( 0.5f, -0.5f, 0.0f),   glm::vec3(1.0f, 1.0f, 1.0f)}   //Vertex 3
};

GLuint testIndices[] = {
    0, 1, 3,
    3, 1, 2
};

int main(int, char**){
    glfwInit();

    std::cout << "Hello from GLFW Version " << glfwGetVersionString() << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, width, height);

    Shader testShader("shaders/test.vert", "shaders/test.frag");
    std::vector <Vertex> testVerts(testVertices, testVertices + sizeof(testVertices) / sizeof(Vertex));
    std::vector <GLuint> testInd(testIndices, testIndices + sizeof(testIndices) / sizeof(GLuint));

    VAO testVAO;

    testVAO.Bind();

    VBO testVBO(testVerts);
    EBO testEBO(testInd);

    testVAO.LinkAttrib(testVBO, 0, 3, GL_FLOAT, sizeof(Vertex), 0);
    testVAO.LinkAttrib(testVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));

    testVAO.Unbind();
    testVBO.Unbind();
    testEBO.Unbind();

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        testShader.Activate();
        testVAO.Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    testEBO.Delete();
    testVBO.Delete();
    testVAO.Delete();
    testShader.Delete();
    glfwTerminate();
    return 0;
}
