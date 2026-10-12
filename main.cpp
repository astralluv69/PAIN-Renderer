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

int width = 1280;
int height = 720;
const char* title = "PAIN Renderer";
GLFWwindow* window;

glm::mat4 uiProj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

int main(int, char**){
    bool wireframeToggle = false;
    bool testModel = false;
    bool updateMesh = false;
    std::optional<std::string> activePath = std::nullopt;

    const char* acceptedFileFilters[] = {"*.obj"};

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
    std::optional<ObjMesh> objFileObj = std::nullopt;

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

    ButtonElement wireframeButton(
        "wireframeButton", 
        glm::vec2(0.0f, 0.0f), 
        glm::vec2(80.0f, 20.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        glm::vec3(0.35f, 0.0f, 0.73f), 
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
        glm::vec3(0.0f, 0.0f, 0.0f)
    );

    ButtonElement changeFileButton(
        "changeFileButton",
        glm::vec2(80.0f, 0.0f),
        glm::vec2(160.0f, 20.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.35f, 0.0f, 0.73f),
        [&activePath, &acceptedFileFilters, &testModel, &updateMesh](){
            const char* newPath = tinyfd_openFileDialog(
                "Select the Model to View",
                "",
                1,
                acceptedFileFilters,
                "Model Files (*.obj)",
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
        glm::vec3(0.0f, 0.0f, 0.0f)  
    );

    ButtonElement testButton(
        "testButton", 
        glm::vec2(1200.0f, 0.0f), 
        glm::vec2(1280.0f, 20.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        glm::vec3(0.35f, 0.0f, 0.73f), 
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
        glm::vec3(0.0f, 0.0f, 0.0f)
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        if (dt >= 0) {
            ribbon.Update(window, dt);

            camera.Inputs(window, dt);

            //glfwSetWindowTitle(window, ("PAIN Renderer - FPS " + std::to_string(1.0f / dt)).c_str());
        }

        camera.updateMatrix(60.0f, 0.1f, 100.0f);
        if(activePath.has_value() && updateMesh) {
            if(activePath->substr(activePath->size() - 3) == "obj") {
                objFileObj = ObjMesh(activePath->c_str());
            }   

            updateMesh = false;
        }

        if(wireframeToggle) {
            if(testModel) {
                testObj.DrawWireframe(objModelShader, camera, objModel);
            } else {
                if(objFileObj.has_value()) {
                    objFileObj->DrawWireframe(objModelShader, camera, objModel);
                }
            }
        } else {
            if(testModel) {
                testObj.Draw(objModelShader, camera, objModel);
            } else {
                if(objFileObj.has_value()) {
                    objFileObj->Draw(objModelShader, camera, objModel);
                }
            }
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
