#include "Renderer/camera.h"

Camera::Camera() : cameraPosition(glm::vec3(0.0f)) { }
Camera::Camera(glm::vec3 position) : cameraPosition(position) { }

void Camera::setCameraPosition(glm::vec3 position) {
    cameraPosition = position;

    // TODO => Implement camera update after new position set
}

glm::mat4 Camera::generateProjectionMatrix() {
    return glm::mat4(0);
}

void Camera::processMouseInput() {
    
}

void Camera::updateCameraVectors() {

}