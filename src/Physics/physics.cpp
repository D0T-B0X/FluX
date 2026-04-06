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
    gridHashShader.load(GRID_CELL_CSHADER_PATH);
    countBufferShader.load(COUNT_CSHADER_PATH);
}

void 
Physics::updateFrame() {

    int threadsPerGroup = 256;
    numGroups = (physicsScene.particleCount + threadsPerGroup - 1) / threadsPerGroup;

    buildGrid();
    updateSPH();
}

void 
Physics::setGridUniforms() {
    gridHashShader.use();

    gridHashShader.setInt("numParticles", physicsScene.getParticleCount());
    gridHashShader.setFloat("cell_size", 1.1 * SMOOTHING_RADIUS); // slighly larger cell size to collect all particles
    gridHashShader.setInt("gridSize", GRID_SIDE);
    gridHashShader.setVec3("gridMin", glm::vec3(MIN_BOUND, MIN_BOUND, MIN_BOUND));
}

void
Physics::setCountSortUniforms() {
    countBufferShader.use();

    countBufferShader.setInt("numParticles", physicsScene.particleCount);
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
    forceShader.setVec3("GRAVITY_C", GRAV_CONSTANT);         // m/s²
    forceShader.setFloat("mu", VISCOSITY);                      // Pa·s (water)

    // Spike kernel fn
    float spike = -45 / (M_PI * pow(SMOOTHING_RADIUS, 6));
    forceShader.setFloat("spikeGrad", spike);

    // Viscosity kernel fn
    // which is the -ve of spike
    forceShader.setFloat("viscLaplacian", -spike);

    // Floor boundary
    forceShader.setFloat("floorY", FLOOR_BOUNDARY);
    forceShader.setFloat("damping", DAMPING_COEFF);
}

void 
Physics::cleanup() {
    
}

void 
Physics::initSSBOs() {
    // Initialize all SSBOs
    glGenBuffers(1, &physicsScene.position_massSSBO);
    glGenBuffers(1, &physicsScene.velocity_densitySSBO);
    glGenBuffers(1, &physicsScene.force_pressureSSBO);
    glGenBuffers(1, &physicsScene.color_paddingSSBO);
    glGenBuffers(1, &physicsScene.cell_indexSSBO);
    glGenBuffers(1, &physicsScene.count_buffSSBO);

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

    // -------- Cell Index buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        physicsScene.cell_indexSSBO
    );

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        physicsScene.getParticleCountSize(),
        0,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        4,
        physicsScene.cell_indexSSBO
    );

    // -------- Count sort buffer --------
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        physicsScene.count_buffSSBO
    );

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        getCountBufferDataSize(),
        0,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        5,
        physicsScene.count_buffSSBO
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void 
Physics::buildGrid() {

    // Calculate uniform grid hash cells
    gridHashShader.use();
    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // count histogram pass
    // pass the new shift key after each pass
    for (int shift_key = 0; shift_key < 32; shift_key += 2) {
        countBufferShader.use();
        countBufferShader.setInt("shift_key", shift_key);

        glDispatchCompute(numGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }
}

void
Physics::updateSPH() {
    
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

unsigned int
Physics::getCountBufferDataSize() {
    return numGroups * 4 * sizeof(GLuint); // 4 integers (4 way radix) for each workgroup
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