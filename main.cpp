#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include "mesh.h"
#include "obj_mesh.h"
#include "ui_element.h"

int width = 1280;
int height = 720;
const char* title = "PAIN Renderer";
GLFWwindow* window;

Vertex testVertices[] = { 
                     //Coordinates                         //Color                       //Normals
	Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),       glm::vec3(0.69f, 1.0f, 0.73f),   glm::vec3( 0.0f,  0.0f,  1.0f)}, // Front Bottom Left   1
	Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),       glm::vec3(1.0f, 0.69f, 0.77f),   glm::vec3( 0.0f,  0.0f,  1.0f)}, // Front Bottom Right  2
	Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),       glm::vec3(0.56f, 0.36f, 0.87f),  glm::vec3( 0.0f,  0.0f,  1.0f)}, // Front Top Right     3
	Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),       glm::vec3(0.94f, 0.93f, 0.6f),   glm::vec3( 0.0f,  0.0f,  1.0f)}, // Front Top Left      4

	Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),       glm::vec3(0.36f, 0.87f, 0.78f),  glm::vec3( 0.0f,  0.0f, -1.0f)}, // Back Face           5
	Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),       glm::vec3(0.68f, 0.25f, 0.18f),  glm::vec3( 0.0f,  0.0f, -1.0f)}, //                     6
	Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),       glm::vec3(0.42f, 1.0f, 0.82f),   glm::vec3( 0.0f,  0.0f, -1.0f)}, //                     7
	Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),       glm::vec3(0.1f, 0.36f, 0.63f),   glm::vec3( 0.0f,  0.0f, -1.0f)}, //                     8

    Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),       glm::vec3(0.94f, 0.93f, 0.6f),   glm::vec3( 0.0f,  1.0f,  0.0f)}, // Top Face            4
    Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),       glm::vec3(0.56f, 0.36f, 0.87f),  glm::vec3( 0.0f,  1.0f,  0.0f)}, //                     3
    Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),       glm::vec3(0.1f, 0.36f, 0.63f),   glm::vec3( 0.0f,  1.0f,  0.0f)}, //                     8
    Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),       glm::vec3(0.42f, 1.0f, 0.82f),   glm::vec3( 0.0f,  1.0f,  0.0f)}, //                     7

    Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),       glm::vec3(0.68f, 0.25f, 0.18f),  glm::vec3( 0.0f, -1.0f,  0.0f)}, // Bottom Face         6
    Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),       glm::vec3(0.36f, 0.87f, 0.78f),  glm::vec3( 0.0f, -1.0f,  0.0f)}, //                     5
    Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),       glm::vec3(1.0f, 0.69f, 0.77f),   glm::vec3( 0.0f, -1.0f,  0.0f)}, //                     2
    Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),       glm::vec3(0.69f, 1.0f, 0.73f),   glm::vec3( 0.0f, -1.0f,  0.0f)}, //                     1

    Vertex{glm::vec3( 0.5f, -0.5f,  0.5f),       glm::vec3(1.0f, 0.69f, 0.77f),   glm::vec3( 1.0f,  0.0f,  0.0f)}, // Right Face          2
    Vertex{glm::vec3( 0.5f, -0.5f, -0.5f),       glm::vec3(0.36f, 0.87f, 0.78f),  glm::vec3( 1.0f,  0.0f,  0.0f)}, //                     5
    Vertex{glm::vec3( 0.5f,  0.5f, -0.5f),       glm::vec3(0.1f, 0.36f, 0.63f),   glm::vec3( 1.0f,  0.0f,  0.0f)}, //                     8
    Vertex{glm::vec3( 0.5f,  0.5f,  0.5f),       glm::vec3(0.56f, 0.36f, 0.87f),  glm::vec3( 1.0f,  0.0f,  0.0f)}, //                     3

    Vertex{glm::vec3(-0.5f, -0.5f, -0.5f),       glm::vec3(0.68f, 0.25f, 0.18f),  glm::vec3(-1.0f,  0.0f,  0.0f)}, // left Face           6
    Vertex{glm::vec3(-0.5f, -0.5f,  0.5f),       glm::vec3(0.69f, 1.0f, 0.73f),   glm::vec3(-1.0f,  0.0f,  0.0f)}, //                     1
    Vertex{glm::vec3(-0.5f,  0.5f,  0.5f),       glm::vec3(0.94f, 0.93f, 0.6f),   glm::vec3(-1.0f,  0.0f,  0.0f)}, //                     4
    Vertex{glm::vec3(-0.5f,  0.5f, -0.5f),       glm::vec3(0.42f, 1.0f, 0.82f),   glm::vec3(-1.0f,  0.0f,  0.0f)}  //                     7
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
    0,  1,  2,     0,  2,  3,   // Front Face
    4,  5,  6,     4,  6,  7,   // Back Face
    8,  9, 10,     8, 10, 11,   // Top Face
    12, 13, 14,    12, 14, 15,  // Bottom Face
    16, 17, 18,    16, 18, 19,  // Right Face
    20, 21, 22,    20, 22, 23   // Left Face
};

GLuint testLightIndices[] = {
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
std::vector <GLuint> testLightInd(testLightIndices, testLightIndices + sizeof(testLightIndices) / sizeof(GLuint));

glm::mat4 uiProj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

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

    ObjMesh testObj("models/stanford-bunny.obj");

    Mesh testCube(testVerts, testInd);

    Mesh lightCube(testLightVerts, testLightInd);

    Element testElement("testElement", glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 20.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Hello World", 16);

    UiElement testUI("testUI");
    testUI.AddElement(testElement);

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 testPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 testCubeModel = glm::mat4(1.0f);
    testCubeModel = glm::translate(testCubeModel, testPos);

    glm::mat4 lightCubeModel = glm::mat4(1.0f);
    lightCubeModel = glm::translate(lightCubeModel, lightPos);

    Shader defaultShader("shaders/default.vert", "shaders/default.frag");
    Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");
    lightingShader.loadLightColorAndPos(lightColor, lightPos);
    Shader objModelShader("shaders/objModel.vert", "shaders/objModel.frag");
    objModelShader.loadLightColorAndPos(lightColor, lightPos);

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    Camera camera(width, height, glm::vec3(0.0f, 2.0f, 2.0f));
    
    double end;
    double begin = glfwGetTime();
    double dt = -1;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        if (dt >= 0) {
            camera.Inputs(window, dt);
            
            //glfwSetWindowTitle(window, ("PAIN Renderer - FPS " + std::to_string(1.0f / dt)).c_str());
        }

        camera.updateMatrix(60.0f, 0.1f, 100.0f);

        //testCube.Draw(lightingShader, camera, testCubeModel);
        testObj.Draw(objModelShader, camera, testCubeModel);
        //lightCube.Draw(defaultShader, camera, lightCubeModel);

        testUI.Draw(uiProj);

        glfwSwapBuffers(window);

        glfwPollEvents();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL Error: 0x" << std::hex << err << std::endl;
        }

        end = glfwGetTime();
        dt = end - begin;
        begin = end;
    }

    testUI.Delete();
    lightCube.Delete();
    testCube.Delete();
    defaultShader.Delete();
    lightingShader.Delete();

    glfwTerminate();

    return 0;
}
