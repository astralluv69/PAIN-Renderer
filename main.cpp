#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <tinyfiledialogs.h>

#include "util.h"
#include "mesh.h"
#include "obj_mesh.h"
#include "ui_element.h"

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera) {
        camera->handleMouseButtons(window, button, action, mods);
    }
}

void mousePosCallback(GLFWwindow* window, double xPos, double yPos) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera) {
        camera->handleMouseMovement(window, xPos, yPos);
    }
}

void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera) {
        camera->handleMouseScroll(xOffset, yOffset);
    }
}

void keyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera) {
        camera->handleKeyboardKeys(window, key, scancode, action, mods);
    }
}

int main(int, char**){
    int width = 1280;
    int height = 720;
    const char* title = "PAIN Renderer";
    GLFWwindow* window;

    glm::mat4 uiProj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

    glm::vec3 buttonColor(0.16f, 0.09f, 0.24f);
    glm::vec3 buttonHoverColor(0.35f, 0.0f, 0.73f);
    glm::vec3 ribbonBgColor(0.43f, 0.35f, 0.53f);

    bool wireframeToggle = false;
    bool testModel = true;
    bool updateMesh = false;
    std::optional<std::string> activePath = std::nullopt;
    bool willClip = false;

    glfwInit();

    std::cout << "GLFW Version " << glfwGetVersionString() << std::endl;

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

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetKeyCallback(window, keyboardKeyCallback);

    ObjMesh testObj("models/stanford-bunny.obj", willClip);
    std::optional<ObjMesh> objFileObj = std::nullopt;

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objModel = glm::mat4(1.0f);
    objModel = glm::translate(objModel, objPos);
    objModel = glm::scale(objModel, glm::vec3(0.1f, 0.1f, 0.1f));

    glm::mat4 testObjModel = glm::translate(glm::mat4(1.0f), objPos);

    Shader objModelShader("shaders/objModel.vert", "shaders/objModel.frag");
    objModelShader.loadLightColorAndPos(lightColor, lightPos);

    Camera camera(width, height, glm::vec3(0.0f, 2.0f, 2.0f));
    glfwSetWindowUserPointer(window, &camera);

    Element ribbonBg("ribbonBg", glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 20.0f), ribbonBgColor);

    ButtonElement wireframeButton(
        "wireframeButton", 
        glm::vec2(0.0f, 0.0f), 
        glm::vec2(80.0f, 20.0f), 
        buttonColor, 
        buttonHoverColor, 
        [&wireframeToggle](){
            static double timer = glfwGetTime();
            if (glfwGetTime() - timer > 0.2) {
                wireframeToggle = !wireframeToggle;
                timer = glfwGetTime();
            }
        }, 
        [](){},
        "Toggle Wire", 
        12,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    ButtonElement changeFileButton(
        "changeFileButton",
        glm::vec2(80.0f, 0.0f),
        glm::vec2(160.0f, 20.0f),
        buttonColor, 
        buttonHoverColor,
        [&activePath, &testModel, &updateMesh](){
            const char* acceptFileFilters[] = {"*.obj", "*.stl"};

            const char* newPath = tinyfd_openFileDialog(
                "Select the Model to View",
                "",
                2,
                acceptFileFilters,
                "Model Files (*.obj, *.stl)",
                0
            );

            if(newPath != nullptr) {
                activePath = std::string(newPath);
                testModel = false;
                updateMesh = true;
            }
        },
        [](){},
        "Change File",
        12,
        glm::vec3(1.0f, 1.0f, 1.0f)  
    );

    ButtonElement testButton(
        "testButton", 
        glm::vec2(1200.0f, 0.0f), 
        glm::vec2(1280.0f, 20.0f), 
        buttonColor, 
        buttonHoverColor, 
        [&testModel](){
            static double timer = glfwGetTime();
            if (glfwGetTime() - timer > 0.2) {
                testModel = !testModel;
                timer = glfwGetTime();
            }
        }, 
        [](){},
        "Toggle Test Model", 
        10,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );

    UiElement ribbon("ribbon");
    ribbon.AddElement(ribbonBg);
    ribbon.AddElement(wireframeButton);
    ribbon.AddElement(changeFileButton);
    ribbon.AddElement(testButton);
    
    double end;
    double begin = glfwGetTime();
    double dt = -1;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        if (dt >= 0) {
            ribbon.Update(window, dt);

            camera.Inputs(dt);

            //glfwSetWindowTitle(window, ("PAIN Renderer - FPS " + std::to_string(1.0f / dt)).c_str());
        }

        camera.updateMatrix(60.0f, 0.1f, 100.0f);
        if(activePath.has_value() && updateMesh) {
            objFileObj = ObjMesh(activePath->c_str(), willClip);  

            if (willClip) {
                objModel = glm::scale(glm::translate(glm::mat4(1.0f), objPos), glm::vec3(0.01f, 0.01f, 0.01f));
            } else {
                objModel = glm::translate(glm::mat4(1.0f), objPos);
            }

            updateMesh = false;
        }

        if(wireframeToggle) {
            if(testModel) {
                testObj.DrawWireframe(objModelShader, camera, testObjModel);
            } else {
                if(objFileObj.has_value()) {
                    objFileObj->DrawWireframe(objModelShader, camera, objModel);
                }
            }
        } else {
            if(testModel) {
                testObj.Draw(objModelShader, camera, testObjModel);
            } else {
                if(objFileObj.has_value()) {
                    objFileObj->Draw(objModelShader, camera, objModel);
                }
            }
        }

        ribbon.Draw(uiProj);

        glfwSwapBuffers(window);

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
    if (objFileObj.has_value()) {
        objFileObj->Delete();
    }

    glfwTerminate();

    return 0;
}
