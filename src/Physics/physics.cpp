#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene),
    timeAccumulator(0)
{ 
    SMOOTHING_RADIUS = 0.0f;
    densityShader.load(DENSITY_CSHADER_PATH);
    pressureShader.load(PRESSURE_CSHADER_PATH);
    forceShader.load(FORCE_CSHADER_PATH);
}

void 
Physics::setDensityUniforms() {
    densityShader.use();

    densityShader.setInt("numParticles", physicsScene.particleCount);
    /* 
     Pass smoothing radius square to 
     the compute densityShader to prevent repetition
     */
    densityShader.setFloat("h2", SMOOTHING_RADIUS*SMOOTHING_RADIUS);

    // Poly6 Kernel fn
    float polySix = 315 / (64 * M_PI * pow(SMOOTHING_RADIUS, 9));
    densityShader.setFloat("poly6", polySix);
}

void 
Physics::setPressureUniforms() {
    pressureShader.use();

    // TODO => Remove hard coded particle count
    pressureShader.setInt("numParticles", physicsScene.particleCount);

    // Stiffness coefficient
    pressureShader.setFloat("k", K);

    // Inverse of the resting density of the fluid
    pressureShader.setFloat("restingRhoInv", 1 / RESTING_DENSITY);

    // Gamma for Tait's equation
    pressureShader.setFloat("gamma", GAMMA);
}

void 
Physics::setForceUniforms() {
    forceShader.use();

    forceShader.setInt("numParticles", physicsScene.particleCount);

    forceShader.setFloat("h", SMOOTHING_RADIUS);
    // TODO => Remove hardcoded grav acc and viscosity
    forceShader.setVec3("GRAVITY_C", glm::vec3(0.0f, -9.81f, 0.0f));   // m/s²
    forceShader.setFloat("mu", 0.001f);                                // Pa·s (water)

    // Spike kernel fn
    float spike = -45 / (M_PI * pow(SMOOTHING_RADIUS, 6));
    forceShader.setFloat("spikeGrad", spike);

    // Viscosity kernel fn
    // which is the -ve of spike
    forceShader.setFloat("viscLaplacian", -spike);

    // Floor boundary
    forceShader.setFloat("floorY", -0.1f);
    forceShader.setFloat("damping", 0.3f);
}

void 
Physics::updateFrame() {

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        0,
        physicsScene.position_massSSBO
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        1,
        physicsScene.velocity_densitySSBO
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        2,
        physicsScene.force_pressureSSBO
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        3,
        physicsScene.color_paddingSSBO
    );

    int threadsPerGroup = 256;
    int numGroups = (physicsScene.particleCount + threadsPerGroup - 1) / threadsPerGroup;

    // Density calculations pass
    densityShader.use();
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure calculations pass
    pressureShader.use();
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure force calculations pass
    forceShader.use();
    forceShader.setFloat("dt", PHYSICS_DT);
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void 
Physics::cleanup() {
    
}

void 
Physics::initSSBO() {
    // Initialize all SSBOs
    glGenBuffers(1, &physicsScene.position_massSSBO);
    glGenBuffers(1, &physicsScene.velocity_densitySSBO);
    glGenBuffers(1, &physicsScene.force_pressureSSBO);
    glGenBuffers(1, &physicsScene.color_paddingSSBO);

    // -------- Position Mass buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.position_massSSBO
    );
    
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.getPropertyDataSize(),
        physicsScene.getPositionMassData(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        0, 
        physicsScene.position_massSSBO
    );

    // -------- Velocity Density buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.velocity_densitySSBO
    );
    
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.getPropertyDataSize(),
        physicsScene.getVelocityDensityData(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        1, 
        physicsScene.velocity_densitySSBO
    );

    // -------- Force Pressure buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.force_pressureSSBO
    );
    
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.getPropertyDataSize(),
        physicsScene.getForcePressureData(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        2, 
        physicsScene.force_pressureSSBO
    );

    // -------- Color Paddig buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.color_paddingSSBO
    );
    
    glBufferData(
        GL_SHADER_STORAGE_BUFFER, 
        physicsScene.getPropertyDataSize(),
        physicsScene.getColorPaddingData(),
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER, 
        3, 
        physicsScene.color_paddingSSBO
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void 
Physics::setSmoothingRadius(float s) {
    if (s < 0.0f) { SMOOTHING_RADIUS = 0.0f; return; }
    SMOOTHING_RADIUS = s;
}

float 
Physics::getSmoothingRadius() {
    return SMOOTHING_RADIUS;
}