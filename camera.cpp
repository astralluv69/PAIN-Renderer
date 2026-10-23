#include "camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
    Camera::width = width;
    Camera::height = height;
    currentX = width / 2;
    currentY = height / 2;
    startPos = position;
    Position = position;
    Orientation = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - Position);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    if (isOrbiting) {
        float yawRad = glm::radians(yaw);
        float pitchRad = glm::radians(pitch);

        glm::vec3 camOffset;
        camOffset.x = radius * cos(pitchRad) * cos(yawRad);
        camOffset.y = radius * sin(pitchRad);
        camOffset.z = radius * cos(pitchRad) * sin(yawRad);

        Position = camOffset;
        view = glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 0.0f), Up);
    } else {
        view = glm::lookAt(Position, Position + Orientation, Up);
    }

    proj = glm::perspective(glm::radians(FOVdeg), (float) width / height, nearPlane, farPlane);

    camMatrix = proj * view;
}

void Camera::Matrix(Shader& shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(camMatrix));
}

void Camera::Inputs(double dt) {
    currentDt = dt;
}