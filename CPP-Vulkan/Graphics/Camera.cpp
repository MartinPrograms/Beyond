//
// Created by marti on 29/09/2024.
//

#include "Camera.h"

namespace Graphics {
    Camera::Camera() {
/*
        *    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
            camera.yaw = -90.0f;
            camera.pitch = 0.0f;
            camera.fov = 45.0f;
            camera.near = 0.1f;
            camera.far = 100.0f;

 */

        this->position = glm::vec3(0.0f, 0.0f, 3.0f);
        this->yaw = -90.0f;
        this->pitch = 0.0f;
        this->fov = 90.0f;
        this->near = 0.1f;
        this->far = 100.0f;
        this->aspectRatio = 1.0f;
        update();
    }

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
        auto localUp = up * -1.0f; // Inverting the up vector, because glm::lookAt expects the up vector to be pointing up.
        return glm::lookAt(position, position + front, localUp);
    }

    glm::mat4 Camera::getProjectionMatrix() {
        fov = glm::clamp(fov, 1.0f, 179.0f);
        auto persp = glm::perspective(glm::radians(fov), aspectRatio, near, far);
        return persp;
    }
} // Graphics