//
// Created by marti on 29/09/2024.
//

#include "Camera.h"

namespace Graphics {
    void Camera::update() {
        // Calculate front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Calculate right vector based on yaw
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

        // Calculate up vector based on right and front
        up = glm::normalize(glm::cross(right, front));
    }

    void Camera::updateAspectRatio(int width, int height) {
        aspectRatio = (float)width / (float)height;
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 Camera::getProjectionMatrix() {
        fov = glm::clamp(fov, 1.0f, 179.0f);
        return glm::perspective(glm::radians(fov), aspectRatio, near, far);
    }
} // Graphics