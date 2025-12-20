#include "Renderer/camera.h"

Camera::Camera() 
    : 
    cameraPosition(glm::vec3(0.0f, 0.0f, 0.0f)), 
    lastX(0.0),
    lastY(0.0),
    first(true),
    yaw(-90.0f),
    pitch(0.0f),
    front(glm::vec3(1.0f, 0.0f, 0.0f))
{
    updateCameraVectors();
}

Camera::Camera(glm::vec3 position) 
    : 
    cameraPosition(position), 
    lastX(0.0),
    lastY(0.0),
    first(true),
    yaw(-90.0f),
    pitch(0.0f),
    front(glm::vec3(1.0f, 0.0f, 0.0f))
{
    updateCameraVectors();
}

void Camera::setCameraPosition(glm::vec3 position) {
    cameraPosition = position;
}

glm::mat4 Camera::generateViewMatrix() {
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    viewMatrix[0][0] = right.x;
    viewMatrix[1][0] = right.y;
    viewMatrix[2][0] = right.z;

    viewMatrix[0][1] = up.x;
    viewMatrix[1][1] = up.y;
    viewMatrix[2][1] = up.z;
    
    viewMatrix[0][2] = -front.x;
    viewMatrix[1][2] = -front.y;
    viewMatrix[2][2] = -front.z;
    
    viewMatrix[3][0] = -glm::dot(right, cameraPosition);
    viewMatrix[3][1] = -glm::dot(up, cameraPosition);
    viewMatrix[3][2] =  glm::dot(front, cameraPosition);

    return viewMatrix;
}

glm::mat4 Camera::generateProjectionMatrix() {
    float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    return glm::perspective(
        glm::radians(FOV), 
        aspect, 
        0.01f, 
        1000.0f
    );
}

void Camera::processMouseInput() {
    if (first) {
        lastX = mousePosition.x;
        lastY = mousePosition.y;
        first = false;
    }

    double xoffset = mousePosition.x - lastX;
    double yoffset = lastY - mousePosition.y;
    lastX = mousePosition.x;
    lastY = mousePosition.y;

    yaw   += xoffset * MOUSE_SENSITIVITY;
    pitch += yoffset * MOUSE_SENSITIVITY;

    if (pitch >  89.0f) { pitch =  89.0f; }
    if (pitch < -89.0f) { pitch = -89.0f; }

    updateCameraVectors();
}

void Camera::processKeyboardInput(Direction dir, float dt) {
    if (dir == Direction::UP)
        cameraPosition += up * dt * MOVEMENT_SPEED;
    if (dir == Direction::DOWN)
        cameraPosition -= up * dt * MOVEMENT_SPEED;
    if (dir == Direction::FORWARD)
        cameraPosition += front * dt * MOVEMENT_SPEED;
    if (dir == Direction::BACKWARD)
        cameraPosition -= front * dt * MOVEMENT_SPEED;
    if (dir == Direction::RIGHT)
        cameraPosition += right * dt * MOVEMENT_SPEED;
    if (dir == Direction::LEFT)
        cameraPosition -= right * dt * MOVEMENT_SPEED;
}

void Camera::updateCameraVectors() {
    float radPitch = glm::radians(pitch);
    float radYaw = glm::radians(yaw);

    float xComp = cos(radPitch) * cos(radYaw);
    float yComp = sin(radPitch);
    float zComp = cos(radPitch) * sin(radYaw); 
    
    front   =   glm::normalize(glm::vec3(xComp, yComp, zComp));
    right   =   glm::normalize(glm::cross(front, WORLD_UP));
    up      =   glm::cross(right, front);
}