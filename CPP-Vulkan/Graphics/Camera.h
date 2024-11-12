//
// Created by marti on 29/09/2024.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Graphics {

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;
    float pitch;
    float roll;

    float fov;

    float near;
    float far;

    float aspectRatio;

    void update(); // This updates the camera's front, right and up vectors, from the yaw, pitch and roll values.
    void updateAspectRatio(int width, int height); // This updates the camera's aspect ratio, from the width and height of the window.
    glm::mat4 getViewMatrix(); // This returns the view matrix of the camera.
    glm::mat4 getProjectionMatrix(); // This returns the projection matrix of the camera.
};

} // Graphics

#endif //CAMERA_H
