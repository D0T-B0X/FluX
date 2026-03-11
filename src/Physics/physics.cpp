#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene) 
{ 
    densityShader.load(DENSITY_CSHADER_PATH);
    pressureShader.load(PRESSURE_CSHADER_PATH);
    densityShader.load(FORCE_CSHADER_PATH);
}

void Physics::setDensityUniforms() {
    densityShader.use();

    // TODO => Remove hard coded particle count
    densityShader.setInt("numParticles", 4096);
    /* 
     Pass smoothing radius square to 
     the compute densityShader to prevent repetition
     */
    densityShader.setFloat("h2", SMOOTHING_RADIUS*SMOOTHING_RADIUS);

    // Poly6 Kernel fn
    float polySix = 315 / (64 * M_PI * pow(SMOOTHING_RADIUS, 9));
    densityShader.setFloat("poly6", polySix);

    // TODO => Move to viscosity uniform fn
    // Viscosity kernel fn
    float viscosity = 45 / (M_PI * pow(SMOOTHING_RADIUS,6));
    densityShader.setFloat("viscLaplacian", viscosity);
}

void Physics::setPressureUniforms() {
    pressureShader.use();

    // TODO => Remove hard coded particle count
    pressureShader.setInt("numParticles", 4096);

    // Stiffness coefficient
    pressureShader.setFloat("k", K);

    // Inverse of the resting density of the fluid
    pressureShader.setFloat("restingRhoInv", 1 / RESTING_DENSITY);

    // Gamma for Tait's equation
    pressureShader.setFloat("gamma", GAMMA);
}

void Physics::setForceUniforms() {
    // TODO => Remove hard coded particle count
    forceShader.setInt("numParticles", 4096);

    forceShader.setFloat("h", SMOOTHING_RADIUS);

    // Spike kernel fn
    float spike = -45 / (M_PI * pow(SMOOTHING_RADIUS, 6));
    forceShader.setFloat("spikeGrad", spike);
}

void Physics::updateFrame() {
    densityShader.use();

    // TODO => Pass to final position calculation shader
    /*
    densityShader.setFloat("dt", physicsScene.dt);
    densityShader.setFloat("base", FLOOR);
    */

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        physicsScene.particleSSBO
    );

    int threadsPerGroup = 256;

    int numGroups = (physicsScene.getSpheresSize() + threadsPerGroup - 1) / threadsPerGroup;

    // Density calculations pass
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure calculations pass
    pressureShader.use();
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure force calculations pass
    forceShader.use();
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
