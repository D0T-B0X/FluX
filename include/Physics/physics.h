#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include "scene.h"
#include "config.h"
#include "Renderer/shader.h"

class Physics {
public: 
    Physics(Scene& activeScene);

    void         updateFrame();
    void         cleanup();
    void         initSSBO();

private:
    Scene&       physicsScene;
    Shader       shader;
};

#endif