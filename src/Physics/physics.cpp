#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene) 
{ 
    shader.load(PHYSICS_CSHADER_PATH);
}

void Physics::updateFrame() {
    shader.use();
    shader.setFloat("dt", physicsScene.dt);

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        physicsScene.particleSSBO
    );

    int numGroups = (physicsScene.getSpheresSize() + 127) / 128;

    glDispatchCompute(numGroups, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

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
