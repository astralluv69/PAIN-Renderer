#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include "util.h"
#include "mesh.h"
#include "obj_mesh.h"
#include "ui_element.h"

int width = 1280;
int height = 720;
const char* title = "PAIN Renderer";
GLFWwindow* window;

std::vector<ButtonElement> buttons;

glm::mat4 uiProj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

int main(int, char**){
    bool wireframeToggle = true;

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

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objModel = glm::mat4(1.0f);
    objModel = glm::translate(objModel, objPos);

    Shader objModelShader("shaders/objModel.vert", "shaders/objModel.frag");
    objModelShader.loadLightColorAndPos(lightColor, lightPos);

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

    Camera camera(width, height, glm::vec3(0.0f, 2.0f, 2.0f));

    Element ribbonBg("ribbonBg", glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 20.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    ButtonElement testButton(
        "testButton", 
        glm::vec2(0.0f, 0.0f), 
        glm::vec2(80.0f, 20.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        glm::vec3(0.35f, 0.0f, 0.73f), 
        buttons, 
        [&wireframeToggle](){
            static double timer = glfwGetTime();
            if (glfwGetTime() - timer > 0.5) {
                wireframeToggle = !wireframeToggle;
                timer = glfwGetTime();
            }
        }, 
        [](){std::cout << "running hover func" << std::endl;},
        "Toggle Wire", 
        12,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );

    UiElement ribbon("ribbon");
    ribbon.AddElement(ribbonBg);
    ribbon.AddElement(testButton);
    
    double end;
    double begin = glfwGetTime();
    double dt = -1;

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        if (dt >= 0) {
            for (ButtonElement button : buttons) {
                button.update(window, dt);
            }

            camera.Inputs(window, dt);

            //glfwSetWindowTitle(window, ("PAIN Renderer - FPS " + std::to_string(1.0f / dt)).c_str());
        }

        camera.updateMatrix(60.0f, 0.1f, 100.0f);

        if(wireframeToggle) {
            testObj.DrawWireframe(objModelShader, camera, objModel);
        } else {
            testObj.Draw(objModelShader, camera, objModel);
        }

        ribbon.Draw(uiProj);

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

    ribbon.Delete();
    testObj.Delete();
    objModelShader.Delete();

    glfwTerminate();

    return 0;
}
