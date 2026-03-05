#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene) 
{ 
    shader.load(PHYSICS_CSHADER_PATH);
}

void Physics::setSPHUniforms() {
    shader.use();

    // TODO => Remove hard coded particle count
    shader.setInt("numParticles", 4096);

    // Stiffmess coefficient
    shader.setFloat("k", K);

    // Resting density of the fluid
    shader.setFloat("restingRho", RESTING_DENSITY);

    // Gamma for Tait's equation
    shader.setFloat("gamma", GAMMA);

    /* 
     Pass smoothing radius and its square to 
     the compute shader to prevent repetition
     */
    shader.setFloat("h", SMOOTHING_RADIUS);
    shader.setFloat("h2", SMOOTHING_RADIUS*SMOOTHING_RADIUS);

    // Poly6 Kernel fn
    float polySix = 315 / (64 * M_PI * pow(SMOOTHING_RADIUS, 9));
    shader.setFloat("poly6", polySix);

    // Spike kernel fn
    float spike = -45 / (M_PI * pow(SMOOTHING_RADIUS, 6));
    shader.setFloat("spikeGrad", spike);

    // Viscosity kernel fn
    float viscosity = 45 / (M_PI * pow(SMOOTHING_RADIUS,6));
    shader.setFloat("viscLaplacian", viscosity);
}

void Physics::updateFrame() {
    shader.use();

    shader.setFloat("dt", physicsScene.dt);
    shader.setFloat("base", FLOOR);

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        physicsScene.particleSSBO
    );

    int threadsPerGroup = 256;

    int numGroups = (physicsScene.getSpheresSize() + threadsPerGroup - 1) / threadsPerGroup;

    glDispatchCompute(numGroups, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
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
