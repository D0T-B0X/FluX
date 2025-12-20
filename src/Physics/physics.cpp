#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene) 
    { }

void Physics::cleanup() {
    
}

void Physics::initSSBO() {
    glGenBuffers(1, &physicsScene.particleSSBO);

    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.particleSSBO
    );
    
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.getSpheresDataSize(),
        physicsScene.getSpheresData(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        physicsScene.particleSSBO
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Physics::createPhysicsProgram() {
    physicsProgram = glCreateProgram();
}