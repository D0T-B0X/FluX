#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "settings.h"

inline constexpr glm::vec3    WORLD_UP   =   glm::vec3(0.0f, 1.0f, 0.0f);

enum Direction {
    UP,
    DOWN,
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};

class Camera {
public:
    Camera();
    Camera(glm::vec3 position);

    glm::vec2   mousePosition;

    void        setCameraPosition(glm::vec3 position);
    glm::mat4   generateViewMatrix();
    glm::mat4   generateProjectionMatrix();
    void        processMouseInput();
    void        processKeyboardInput(Direction dir, float dt);

private:
    bool            first;
    double          lastX;
    double          lastY;   
    double          yaw;
    double          pitch; 
    
    glm::vec3       cameraPosition;
    glm::vec3       front;
    glm::vec3       right;
    glm::vec3       up;

    void        updateCameraVectors();
};

#endif