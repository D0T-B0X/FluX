#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include "scene.h"

class Physics {
public: 
    Physics(Scene& activeScene);

    void         cleanup();
    void         initSSBO();

private:
    Scene&       physicsScene;

    void         createPhysicsProgram();
    void         setPhysicsFloat(const char* name, float val);

    GLuint       physicsProgram;
};

#endif