#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"

int width = 1280;
int height = 720;
const char* title = "PAIN Renderer";
GLFWwindow* window;

Vertex testVertices[] = { 
                    //Coordinates                    //Color
	Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),  glm::vec3(1.0f, 0.0f, 0.0f)},
	Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),  glm::vec3(1.0f, 1.0f, 0.0f)},
	Vertex{glm::vec3(0.5f, -0.5f, -0.5f),   glm::vec3(1.0f, 0.0f, 1.0f)},
	Vertex{glm::vec3(0.5f, -0.5f,  0.5f),   glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),  glm::vec3(0.0f, 1.0f, 0.0f)},
	Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),  glm::vec3(0.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(0.5f,  0.5f, -0.5f),   glm::vec3(1.0f, 0.5f, 0.5f)},
	Vertex{glm::vec3(0.5f,  0.5f,  0.5f),   glm::vec3(1.0f, 1.0f, 1.0f)}
};

Vertex testLightVertices[] = { 
                    //Coordinates                     //Color
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f),   glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f),   glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f),    glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f),    glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f),   glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f),   glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f),    glm::vec3(1.0f, 1.0f, 1.0f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f),    glm::vec3(1.0f, 1.0f, 1.0f)}
};

GLuint testIndices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

std::vector <Vertex> testVerts(testVertices, testVertices + sizeof(testVertices) / sizeof(Vertex));
std::vector <Vertex> testLightVerts(testLightVertices, testLightVertices + sizeof(testLightVertices) / sizeof(Vertex));

std::vector <GLuint> testInd(testIndices, testIndices + sizeof(testIndices) / sizeof(GLuint));

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

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    Shader testShader("shaders/test.vert", "shaders/test.frag");

    Mesh testCube(testVerts, testInd);

    Mesh lightCube(testLightVerts, testInd);

    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 testPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 testCubeModel = glm::mat4(1.0f);
    testCubeModel = glm::translate(testCubeModel, testPos);

    glm::mat4 lightCubeModel = glm::mat4(1.0f);
    lightCubeModel = glm::translate(lightCubeModel, lightPos);

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        camera.updateMatrix(60.0f, 0.1f, 100.0f);
        camera.Inputs(window);

        testCube.Draw(testShader, camera, testCubeModel);
        lightCube.Draw(testShader, camera, lightCubeModel);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    testCube.Delete();
    testShader.Delete();

    glfwTerminate();

    return 0;
}
