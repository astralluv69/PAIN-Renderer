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

        void handleKeyboardKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_LEFT_SHIFT) {
                if (action == GLFW_PRESS) {
                    isHoldingShift = true;
                }
                if (action == GLFW_RELEASE) {
                    isHoldingShift = false;
                }
            }
        };

        void handleMouseButtons(GLFWwindow* window, int button, int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (currentY > 20) {
                    if (action == GLFW_PRESS) {
                        glfwGetCursorPos(window, &startX, &startY);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                        glfwSetCursorPos(window, width / 2, height / 2);

                        if (!isHoldingShift) {
                            isPanning = true;
                        } else {
                            radius = glm::length(Position);

                            glm::vec3 normDir = glm::normalize(Position);
                            pitch = glm::degrees(std::asin(normDir.y));
                            yaw = glm::degrees(std::atan2(normDir.z, normDir.x));

                            isOrbiting = true;
                        }

                        firstDragFrame = true;
                    }
                    if (action == GLFW_RELEASE) {
                        isPanning = false;
                        isOrbiting = false;
                        glfwSetCursorPos(window, startX, startY);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    }
                }
            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if (action == GLFW_PRESS && currentY > 20) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                    isGrabbing = true;
                }
                if (action == GLFW_RELEASE) {
                    isGrabbing = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
        };

        void handleMouseMovement(GLFWwindow* window, double xPos, double yPos) {
            double lastX = currentX;
            double lastY = currentY;

            if (firstDragFrame) {
                lastX = xPos;
                lastY = yPos;
                firstDragFrame = false;
            }

            currentX = xPos;
            currentY = yPos;

            currentDx = currentX - lastX;
            currentDy = currentY - lastY;

            if (isOrbiting && yPos > 20 && !isGrabbing && !isPanning) {
                yaw += ((float)currentDx / 75.0f) * sensitivity;
                pitch += ((float)currentDy / 75.0f) * sensitivity;
            }

            pitch = std::clamp(pitch, -89.0f, 89.0f);

            if (isPanning && yPos > 20 && !isGrabbing && !isOrbiting) {
                float rotX = sensitivity * (float) (yPos - (height / 2)) / height;
                float rotY = sensitivity * (float) (xPos - (width / 2)) / width;

                glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

                if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))) {
                    Orientation = newOrientation;
                }

                Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
                glfwSetCursorPos(window, (width / 2), (height / 2));
            }

            if (isGrabbing && yPos > 20 && !isPanning && !isOrbiting) {
                if (currentDx > 0) {
                    Position += (float)(speed / 3 * currentDt) * -glm::normalize(glm::cross(Orientation, Up));
                } else if (currentDx < 0) {
                    Position += (float)(speed / 3 * currentDt) * glm::normalize(glm::cross(Orientation, Up));
                }

                if (currentDy > 0) {
                    Position += (float)(speed / 3 * currentDt) * Up;
                } if (currentDy < 0) {
                    Position += (float)(speed / 3 * currentDt) * -Up;
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

            radius = glm::length(Position);
        };
    
    private:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 currentPivot = glm::vec3(0.0f);

        glm::mat4 camMatrix;

        int width;
        int height;

        double currentX, currentY, currentDx, currentDy, currentDt, startX, startY;
        bool isPanning = false;
        bool isGrabbing = false;
        bool isOrbiting = false;
        bool isHoldingShift = false;
        bool firstDragFrame = false;

        float speed = 15.0f;
        float sensitivity = 200.0f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float radius = 2.0f;
};

#endif