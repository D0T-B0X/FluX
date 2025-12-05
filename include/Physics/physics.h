#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include "scene.h"

class Physics {
public: 
    Physics(Scene& activeScene);

    void         cleanup();

private:
    Scene&       physicsScene;
};

#endif