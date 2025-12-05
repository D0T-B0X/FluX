#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(glm::vec3 position);

    void        setCameraPosition(glm::vec3 position);
    glm::mat4   generateProjectionMatrix();
    void        processMouseInput();

private:
    bool        first = true;
    double      lastX;
    double      lastY;
    
    glm::vec3   cameraPosition;

    void        updateCameraVectors();
};

#endif