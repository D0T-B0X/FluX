#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene),
    timeAccumulator(0),
    workgroupCount(0)
{ 
    SMOOTHING_RADIUS = 0.0f;
    densityShader.load(DENSITY_CSHADER_PATH);
    pressureShader.load(PRESSURE_CSHADER_PATH);
    forceShader.load(FORCE_CSHADER_PATH);
    gridHashShader.load(GRID_CELL_CSHADER_PATH);
    countBufferShader.load(COUNT_CSHADER_PATH);
    localScanShader.load(LOCAL_SCAN_CSHADER_PATH);
    blockSumScanShader.load(BLOCK_SUM_SCAN_CSHADER_PATH);
    combineShader.load(COMBINE_CSHADER_PATH);
}

void 
Physics::updateFrame() {

    buildGrid();
    computeSPHUpdates();
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
Physics::setLocalScanUniforms() {
    localScanShader.use();

    // 4 way radix sort 
    localScanShader.setInt("workGroupCount", workgroupCount);
}

void
Physics::setBlockSumScanUniforms() {
    blockSumScanShader.use();

    blockSumScanShader.setInt("blockSumSize", ceil((float)workgroupCount / (float)THREADS_PER_GROUP) * 4);
}

void
Physics::setCombineUniforms() {
    combineShader.use();

    combineShader.setInt("workGroupCount", workgroupCount);
}

void 
Physics::cleanup() {
    
}

void 
Physics::setupSSBO(Buffer& b) {

    glGenBuffers(1, &b.bufferID);

    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        b.bufferID
    );

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        b.bufferDataSize,
        b.bufferData,
        GL_DYNAMIC_DRAW
    );

    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        b.bufferBindBase,
        b.bufferID
    );
}

void
Physics::initSSBOs() {

    // -------- Position Mass buffer --------
    physicsScene.position_massSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.position_massSSBO.bufferData = physicsScene.getPositionMassData();
    physicsScene.position_massSSBO.bufferBindBase = 0;
    setupSSBO(physicsScene.position_massSSBO);

    // -------- Velocity Density buffer --------
    physicsScene.velocity_densitySSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.velocity_densitySSBO.bufferData = physicsScene.getVelocityDensityData();
    physicsScene.velocity_densitySSBO.bufferBindBase = 1;
    setupSSBO(physicsScene.velocity_densitySSBO);
    
    // -------- Force Pressure buffer --------
    physicsScene.force_pressureSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.force_pressureSSBO.bufferData = physicsScene.getForcePressureData();
    physicsScene.force_pressureSSBO.bufferBindBase = 2;
    setupSSBO(physicsScene.force_pressureSSBO);

    // -------- Color Paddig buffer --------
    physicsScene.color_paddingSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.color_paddingSSBO.bufferData = physicsScene.getColorPaddingData();
    physicsScene.color_paddingSSBO.bufferBindBase = 3;
    setupSSBO(physicsScene.color_paddingSSBO);

    // -------- Cell Index buffer --------
    physicsScene.cell_indexSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.cell_indexSSBO.bufferData = 0;  // empty buffer
    physicsScene.cell_indexSSBO.bufferBindBase = 4;
    setupSSBO(physicsScene.cell_indexSSBO);   

    // -------- Count sort buffer --------
    physicsScene.count_buffSSBO.bufferDataSize = getCountBufferDataSize();
    physicsScene.count_buffSSBO.bufferData = 0;  // empty buffer
    physicsScene.count_buffSSBO.bufferBindBase = 5;
    setupSSBO(physicsScene.count_buffSSBO);   

    // -------- Local Scan Block sum buffer --------
    physicsScene.blockSum_buffSSBO.bufferDataSize = sizeof(uint) * ceil(workgroupCount / THREADS_PER_GROUP) * 4;
    physicsScene.blockSum_buffSSBO.bufferData = 0;  // empty buffer
    physicsScene.blockSum_buffSSBO.bufferBindBase = 6;
    setupSSBO(physicsScene.blockSum_buffSSBO);   

    // -------- Local prefix sum offset buffer --------
    physicsScene.localSum_buffSSBO.bufferDataSize = getCountBufferDataSize();
    physicsScene.localSum_buffSSBO.bufferData = 0;  // empty buffer
    physicsScene.localSum_buffSSBO.bufferBindBase = 7;
    setupSSBO(physicsScene.localSum_buffSSBO);   

    // -------- Global Offset buffer --------
    physicsScene.offset_buffSSBO.bufferDataSize = getCountBufferDataSize();
    physicsScene.offset_buffSSBO.bufferData = 0;  // empty buffer
    physicsScene.offset_buffSSBO.bufferBindBase = 8;
    setupSSBO(physicsScene.offset_buffSSBO);   
}

void 
Physics::buildGrid() {

    // Calculate uniform grid hash cells
    gridHashShader.use();
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Radix sort of hashed cell indices
    // pass the new shift key after each pass
    for (int shift_key = 0; shift_key < 32; shift_key += 2) {
        countBufferShader.use();
        countBufferShader.setInt("shift_key", shift_key);

        glDispatchCompute(workgroupCount, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        // TODO => Remove magic numbers
        localScanShader.use();
        localScanShader.setInt("passCount", getScanPassCount(workgroupCount));

        unsigned int localScanDispatchSize = (unsigned int)ceil((float)workgroupCount / (float)THREADS_PER_GROUP);
        glDispatchCompute(localScanDispatchSize, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        blockSumScanShader.use();
        blockSumScanShader.setInt("passCount", getScanPassCount(localScanDispatchSize));
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        combineShader.use();
        glDispatchCompute(localScanDispatchSize, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }
}

void
Physics::setWorkGroupCount() {
    // Lacks proper error handling :/
    if (physicsScene.particleCount < 1) { workgroupCount = 1; return; }
    workgroupCount = ceil((float)physicsScene.particleCount / (float)THREADS_PER_GROUP);
}

void
Physics::computeSPHUpdates() {
    
    // Density calculations pass
    densityShader.use();
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure calculations pass
    pressureShader.use();
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Pressure force calculations pass
    forceShader.use();
    forceShader.setFloat("dt", PHYSICS_DT);
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

unsigned int 
Physics::getScanPassCount(unsigned int n) {
    if (n <= 1) return 0;
    return std::bit_width(n - 1);
}

unsigned int
Physics::getCountBufferDataSize() {
    return workgroupCount * 4 * sizeof(GLuint); // 4 integers (4 way radix) for each workgroup
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