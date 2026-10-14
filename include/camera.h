#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"

class Camera {
    public:
        Camera(int width, int height, glm::vec3 position);

        void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
        void Matrix(Shader& shader, const char* uniform);
        void Inputs(double dt);

        void handleMouseButtons(GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (action == GLFW_PRESS && currentY > 20) {
                    glfwGetCursorPos(window, &panningStartX, &panningStartY);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                    glfwSetCursorPos(window, width / 2, height / 2);
                    isPanning = true;
                }
                if (action == GLFW_RELEASE && currentY > 20) {
                    isPanning = false;
                    glfwSetCursorPos(window, panningStartX, panningStartY);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if (action == GLFW_PRESS && currentY > 20) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                    isGrabbing = true;
                }
                if (action == GLFW_RELEASE && currentY > 20) {
                    isGrabbing = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
        };

        void handleMouseMovement(GLFWwindow* window, double xPos, double yPos) {
            double lastX = currentX;
            double lastY = currentY;

            currentX = xPos;
            currentY = yPos;

            double dx = currentX - lastX;
            double dy = currentY - lastY;

            if (isPanning && yPos > 20 && !isGrabbing) {
                float rotX = sensitivity * (float) (yPos - (height / 2)) / height;
                float rotY = sensitivity * (float) (xPos - (width / 2)) / width;

                glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

                if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))) {
                    Orientation = newOrientation;
                }

                Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
                glfwSetCursorPos(window, (width / 2), (height / 2));
            }

            if (isGrabbing && yPos > 20 && !isPanning) {
                if (dx > 0) {
                    Position += (float)(speed / 2 * currentDt) * glm::normalize(glm::cross(Orientation, Up));
                } else if (dx < 0) {
                    Position += (float)(speed / 2 * currentDt) * -glm::normalize(glm::cross(Orientation, Up));
                }

                if (dy > 0) {
                    Position += (float)(speed / 2 * currentDt) * -Up;
                } if (dy < 0) {
                    Position += (float)(speed / 2 * currentDt) * Up;
                }
            }
        };

        void handleMouseScroll(double xOffset, double yOffset) {
            if (yOffset < 0) {
                Position += (float)(speed / 1.25 * currentDt) * -Orientation;
            }
            if (yOffset > 0) {
                Position += (float)(speed / 1.25 * currentDt) * Orientation;
            }
        };
    
    private:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 camMatrix;

        int width;
        int height;

        double currentX, currentY, currentDt, panningStartX, panningStartY;
        bool firstClick = true;
        bool isPanning = false;
        bool isGrabbing = false;

        float speed = 15.0f;
        float sensitivity = 200.0f;
};

#endif