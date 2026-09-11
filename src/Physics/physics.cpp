#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene),
    timeAccumulator(0),
    workgroupCount(0)
{ 
    totalStepsTaken = 0;
    queryIndex = 0;
    average = 0.0;
    queryHasResult[0] = queryHasResult[1] = 0;

    SMOOTHING_RADIUS = 0.0f;
    gridHashShader.load(GRID_CELL_CSHADER_PATH);
    prefixScanShader.load(LOCAL_PREFIX_SCAN_CSHADER_PATH);
    globalOffsetSumShader.load(GLOBAL_OFFSET_SUM_CSHADER_PATH);
    scatterShader.load(SCATTER_CSHADER_PATH);
    reorderBuffersShader.load(REORDER_BUFFER_CSHADER_PATH);
    computeCellBoundariesShader.load(COMPUTE_CELL_BOUNDARIES_CSHADER_PATH);
    densityShader.load(DENSITY_CSHADER_PATH);
    pressureShader.load(PRESSURE_CSHADER_PATH);
    forceShader.load(FORCE_CSHADER_PATH);

    glGenQueries(2, timeQueries);
}

void 
Physics::updateFrame() {
    int readIndex = 1 - queryIndex;
    double resultMs = -1.0;

    glBeginQuery(GL_TIME_ELAPSED, timeQueries[queryIndex]);

    refreshBoundarySSBOs();
    performSpatialHashAndSort();
    reorderParticleBuffers();
    computeCellBoundaries();
    computeSPHUpdates();
    
    glEndQuery(GL_TIME_ELAPSED);

    if (queryHasResult[readIndex]) {
        GLint available = 0;
        glGetQueryObjectiv(timeQueries[readIndex], GL_QUERY_RESULT_AVAILABLE, &available);
        if (available) {
            GLuint64 elapsedTime;
            glGetQueryObjectui64v(timeQueries[readIndex], GL_QUERY_RESULT, &elapsedTime);
            resultMs = static_cast<double>(elapsedTime) / 1e6;
            average += resultMs;
            totalStepsTaken++;
        }
    }

    queryHasResult[queryIndex] = 1;
    queryIndex = 1 - queryIndex;
}

double
Physics::getTime() {
    return average / totalStepsTaken;
}

void 
Physics::cleanup() {
    // TODO => Do something with this negro


}

void
Physics::uploadUniforms() {

    setGridUniforms();
    setPrefixScanUniforms();
    setGlobalOffsetSumUniforms();
    setScatterUniforms();
    setReorderBuffersUniforms();
    setComputeCellBoundariesUniforms();

    setDensityUniforms();
    setPressureUniforms();
    setForceUniforms();
}

void 
Physics::setupSSBO(Buffer& b) {

    glCreateBuffers(1, &b.bufferID);

    glNamedBufferData(
        b.bufferID,
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
    physicsScene.position_massInSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.position_massInSSBO.bufferData = NULL;
    physicsScene.position_massInSSBO.bufferBindBase = 0;
    setupSSBO(physicsScene.position_massInSSBO);

    // -------- Velocity Density buffer --------
    physicsScene.velocity_densityInSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.velocity_densityInSSBO.bufferData = NULL;
    physicsScene.velocity_densityInSSBO.bufferBindBase = 1;
    setupSSBO(physicsScene.velocity_densityInSSBO);
    
    // -------- Force Pressure buffer --------
    physicsScene.force_pressureInSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.force_pressureInSSBO.bufferData = NULL;
    physicsScene.force_pressureInSSBO.bufferBindBase = 2;
    setupSSBO(physicsScene.force_pressureInSSBO);

    // -------- Color Paddig buffer --------
    physicsScene.color_paddingInSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.color_paddingInSSBO.bufferData = NULL;
    physicsScene.color_paddingInSSBO.bufferBindBase = 3;
    setupSSBO(physicsScene.color_paddingInSSBO);

    // -------- First Cell Index buffer --------
    physicsScene.cell_index_oneSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.cell_index_oneSSBO.bufferData = NULL;  // empty buffer
    physicsScene.cell_index_oneSSBO.bufferBindBase = 4;
    setupSSBO(physicsScene.cell_index_oneSSBO);   

    // -------- First Particle Index buffer --------
    physicsScene.particle_index_oneSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.particle_index_oneSSBO.bufferData = NULL;  // empty buffer
    physicsScene.particle_index_oneSSBO.bufferBindBase = 5;
    setupSSBO(physicsScene.particle_index_oneSSBO);   

    // -------- Global Offset buffer --------
    physicsScene.gloablOffset_buffSSBO.bufferDataSize = sizeof(unsigned int) * (workgroupCount * 4);
    physicsScene.gloablOffset_buffSSBO.bufferData = NULL;
    physicsScene.gloablOffset_buffSSBO.bufferBindBase = 7;
    setupSSBO(physicsScene.gloablOffset_buffSSBO);

    // -------- Block Sum buffer --------
    physicsScene.blockSum_buffSSBO.bufferDataSize = sizeof(unsigned int) * (workgroupCount * 4);
    physicsScene.blockSum_buffSSBO.bufferData = NULL;
    physicsScene.blockSum_buffSSBO.bufferBindBase = 8;
    setupSSBO(physicsScene.blockSum_buffSSBO);

    // -------- Second Cell Index buffer --------
    physicsScene.cell_index_twoSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.cell_index_twoSSBO.bufferData = NULL;  // empty buffer
    physicsScene.cell_index_twoSSBO.bufferBindBase = 9;
    setupSSBO(physicsScene.cell_index_twoSSBO);   

    // -------- Second Particle Index buffer --------
    physicsScene.particle_index_twoSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.particle_index_twoSSBO.bufferData = 0;  // empty buffer
    physicsScene.particle_index_twoSSBO.bufferBindBase = 10;
    setupSSBO(physicsScene.particle_index_twoSSBO);   

    // -------- Position Mass Out buffer --------
    physicsScene.position_massOutSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.position_massOutSSBO.bufferData = physicsScene.getPositionMassData();
    physicsScene.position_massOutSSBO.bufferBindBase = 11;
    setupSSBO(physicsScene.position_massOutSSBO);

    // -------- Velocity Density Out buffer --------
    physicsScene.velocity_densityOutSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.velocity_densityOutSSBO.bufferData = physicsScene.getVelocityDensityData();
    physicsScene.velocity_densityOutSSBO.bufferBindBase = 12;
    setupSSBO(physicsScene.velocity_densityOutSSBO);
    
    // -------- Force Pressure Out buffer --------
    physicsScene.force_pressureOutSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.force_pressureOutSSBO.bufferData = physicsScene.getForcePressureData();
    physicsScene.force_pressureOutSSBO.bufferBindBase = 13;
    setupSSBO(physicsScene.force_pressureOutSSBO);

    // -------- Color Paddig Out buffer --------
    physicsScene.color_paddingOutSSBO.bufferDataSize = physicsScene.getPropertyDataSize();
    physicsScene.color_paddingOutSSBO.bufferData = physicsScene.getColorPaddingData();
    physicsScene.color_paddingOutSSBO.bufferBindBase = 14;
    setupSSBO(physicsScene.color_paddingOutSSBO);

    // -------- Cell Index Start buffer --------
    physicsScene.cell_boundary_startSSBO.bufferDataSize = sizeof(int) * GRID_CELL_COUNT;
    physicsScene.cell_boundary_startSSBO.bufferData = NULL;
    physicsScene.cell_boundary_startSSBO.bufferBindBase = 15;
    setupSSBO(physicsScene.cell_boundary_startSSBO);    

    // -------- Cell Index End buffer --------
    physicsScene.cell_boundary_endSSBO.bufferDataSize = sizeof(int) * GRID_CELL_COUNT;
    physicsScene.cell_boundary_endSSBO.bufferData = NULL;
    physicsScene.cell_boundary_endSSBO.bufferBindBase = 16;
    setupSSBO(physicsScene.cell_boundary_endSSBO);    
}

void 
Physics::performSpatialHashAndSort() {

    /**
     ***************************************
     * * * * G R I D   H A S H I N G * * * *
     ***************************************
     */
    gridHashShader.use();

    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    /**
     ***********************************
     * * * * R A D I X   S O R T * * * *
     ***********************************
     */
    
    /**
     * 4 way radix sort processes 2 bits at once.
     * 
     * This halves the number of passes needed to sort the entire buffer,
     * thus we increment the shift key by 2 per pass. 
     * 32 bit number i.e 16 passes
     */

    bool swapBufferOneWithTwo = true;
    for (int shift_key = 0; shift_key < 32; shift_key += 2) {

        /**
         * PHASE 1
         * 
         * Performs a local prefiix scan on 512 particles
         * per workgroup. 
         * 
         * Also stores the global offset values in a buffer. 
         */
        prefixScanShader.use();
        prefixScanShader.setInt("shift_key", shift_key);

        glDispatchCompute(workgroupCount, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        /**
         * PHASE 2
         *
         * The global offsets prefix sum is calculated by a single workgroup 
         * i.e. 256 threads with a maximum of 1024 workgroup's worth of particles.
         * 
         * Each workgroup processes 512 particles so the maximum particle count is
         * capped at 1024 * 512 = 524,288 particles
         */
        globalOffsetSumShader.use();

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        /**
         * PHASE 3
         *  
         * Final scatter phase to globally sort all particles 
         * and swap buffers in ping-pong manner
         */
        scatterShader.use();
        scatterShader.setInt("shift_key", shift_key);

        glDispatchCompute(workgroupCount, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        Buffer& inCellIndex = physicsScene.cell_index_oneSSBO;
        Buffer& outCellIndex = physicsScene.cell_index_twoSSBO;
        swapInputAndOutputBuffers(inCellIndex, outCellIndex);

        Buffer& inParticleIndex = physicsScene.particle_index_oneSSBO;
        Buffer& outParticleIndex = physicsScene.particle_index_twoSSBO;
        swapInputAndOutputBuffers(inParticleIndex, outParticleIndex);
    }
}

void
Physics::reorderParticleBuffers() {

    // copy OUT -> IN at start of frame
    glCopyNamedBufferSubData(
        physicsScene.position_massOutSSBO.bufferID,
        physicsScene.position_massInSSBO.bufferID,
        0,
        0,
        physicsScene.getPropertyDataSize()
    );

    glCopyNamedBufferSubData(
        physicsScene.velocity_densityOutSSBO.bufferID,
        physicsScene.velocity_densityInSSBO.bufferID,   
        0, 
        0, 
        physicsScene.getPropertyDataSize()
    );

    glCopyNamedBufferSubData(
        physicsScene.force_pressureOutSSBO.bufferID,
        physicsScene.force_pressureInSSBO.bufferID,   
        0, 
        0, 
        physicsScene.getPropertyDataSize()
    );

    glCopyNamedBufferSubData(
        physicsScene.color_paddingOutSSBO.bufferID,
        physicsScene.color_paddingInSSBO.bufferID,   
        0, 
        0, 
        physicsScene.getPropertyDataSize()
    );
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    reorderBuffersShader.use();

    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void
Physics::computeCellBoundaries() {
    computeCellBoundariesShader.use();

    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void
Physics::computeSPHUpdates() {

    // Density calculations pass
    densityShader.use();
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

#ifdef DEBUG

    glFinish();
    void* data = glMapNamedBuffer(physicsScene.velocity_densityOutSSBO.bufferID, GL_READ_ONLY);
    float* floats = (float*)data;
    for (int i = 0; i < 5; i++) {
        std::cout << "density[" << i << "] = " << floats[i * 4 + 3] << std::endl;
    }
    glUnmapNamedBuffer(physicsScene.velocity_densityOutSSBO.bufferID);

#endif

    // Pressure calculations pass
    pressureShader.use();
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Pressure force calculations pass
    forceShader.use();
    forceShader.setFloat("dt", PHYSICS_DT);
    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void
Physics::setWorkGroupCount() {
    // Lacks proper error handling :/
    if (physicsScene.getParticleCount() < 1) { workgroupCount = 1; return; }
    
    workgroupCount = ceil((float)physicsScene.getParticleCount() / (float)PARTICLES_PER_WORKGROUP);
    
#ifdef DEBUG
    std::cout << "Workgroup count: " << workgroupCount << std::endl;
#endif
}

void 
Physics::setGridUniforms() {
    gridHashShader.use();

    gridHashShader.setInt("numParticles", physicsScene.getParticleCount());
    // slighly larger cell size to collect all particles
    gridHashShader.setFloat("cell_size", SMOOTHING_RADIUS); 
    gridHashShader.setInt("gridSize", std::ceil((MAX_BOUND - MIN_BOUND) / SMOOTHING_RADIUS));
    gridHashShader.setVec3("gridMin", glm::vec3(MIN_BOUND, MIN_BOUND, MIN_BOUND));
}

void
Physics::setPrefixScanUniforms() {
    prefixScanShader.use();

    prefixScanShader.setInt("totalParticleCount", physicsScene.getParticleCount());
    prefixScanShader.setInt("workGroupCount", workgroupCount);

    int PARTICLES_PROCESSED_PER_WORKGROUP = THREADS_PER_GROUP * 2;
    prefixScanShader.setInt("passCount", getScanPassCount(PARTICLES_PROCESSED_PER_WORKGROUP));
}

void
Physics::setGlobalOffsetSumUniforms() {
    globalOffsetSumShader.use();

    int elementsProcessedPerThread = ceil((float)workgroupCount / (float)THREADS_PER_GROUP);
    globalOffsetSumShader.setInt("elementsProcessedPerThread", elementsProcessedPerThread);
    globalOffsetSumShader.setInt("effectiveThreadCount", getEffectiveThreadCount(workgroupCount * 4));
}

void
Physics::setScatterUniforms() {
    scatterShader.use();
    
    scatterShader.setInt("totalParticleCount", physicsScene.getParticleCount());
    scatterShader.setInt("workGroupCount", workgroupCount);
}

void
Physics::setReorderBuffersUniforms() {
    reorderBuffersShader.use();

    reorderBuffersShader.setInt("totalParticleCount", physicsScene.getParticleCount());
}

void
Physics::setComputeCellBoundariesUniforms() {
    computeCellBoundariesShader.use();

    computeCellBoundariesShader.setInt("totalParticleCount", physicsScene.getParticleCount());
}

void 
Physics::setDensityUniforms() {
    densityShader.use();

    /* 
     Pass smoothing radius square to 
     the compute densityShader to prevent repetition
     */
    densityShader.setFloat("h2", SMOOTHING_RADIUS*SMOOTHING_RADIUS);

    // Poly6 Kernel fn
#ifdef DEBUG
    std::cout << "Smoothing Radius: " << SMOOTHING_RADIUS << std::endl;
#endif 
    float polySix = 315 / (64 * M_PI * pow(SMOOTHING_RADIUS, 9));
    densityShader.setFloat("poly6", polySix);

    float w_at_zero = polySix * pow(SMOOTHING_RADIUS, 6);
#ifdef DEBUG
    std::cout << "W(0): " << w_at_zero << std::endl;
    std::cout << "approx density 30 neighbors: " << 30 * 0.000125f * w_at_zero << std::endl;
#endif

    int gridCountOnSide = std::ceil((MAX_BOUND - MIN_BOUND) / SMOOTHING_RADIUS);
    densityShader.setInt("totalParticleCount", physicsScene.getParticleCount());
    densityShader.setFloat("cellSize", SMOOTHING_RADIUS); 
    densityShader.setInt("gridSize", gridCountOnSide);
    densityShader.setFloat("minBound", MIN_BOUND);
    densityShader.setFloat("maxBound", MAX_BOUND);
}

void 
Physics::setPressureUniforms() {
    pressureShader.use();

    pressureShader.setInt("numParticles", physicsScene.getParticleCount());
    // Stiffness coefficient
    pressureShader.setFloat("k", K);
    // Speed of sound
    pressureShader.setFloat("speedOfSound", SPEED_OF_SOUND);
    // Inverse of Gamma for faster calculations
    pressureShader.setFloat("gammaInv", 1 / (float)GAMMA);
    // Resting density of the fluid
    pressureShader.setFloat("restingRho", RESTING_DENSITY);
    // Inverse of the resting density of the fluid
    pressureShader.setFloat("restingRhoInv", 1 / RESTING_DENSITY);
    // Gamma for Tait's equation
    pressureShader.setFloat("gamma", GAMMA);
}

void 
Physics::setForceUniforms() {
    forceShader.use();

    forceShader.setInt("numParticles", physicsScene.getParticleCount());

    int gridCountOnSide = std::ceil((MAX_BOUND - MIN_BOUND) / SMOOTHING_RADIUS);

    forceShader.setFloat("h", SMOOTHING_RADIUS);
    forceShader.setVec3("GRAVITY_C", GRAV_CONSTANT);         // m/s²
    forceShader.setFloat("mu", VISCOSITY);                   // Pa·s (water)

    // Spike kernel fn
    float spike = -45 / (M_PI * pow(SMOOTHING_RADIUS, 6));
    forceShader.setFloat("spikeGrad", spike);

    // Viscosity kernel fn
    // which is the -ve of spike
    forceShader.setFloat("viscLaplacian", -spike);

    // Container boundary
    forceShader.setFloat("floorBound", FLOOR_BOUNDARY); 
    forceShader.setFloat("restingDensity", RESTING_DENSITY);
    forceShader.setFloat("minBound", MIN_BOUND);
    forceShader.setFloat("maxBound", MAX_BOUND);
    forceShader.setFloat("xMinBound", X_CONTAINER_MIN);
    forceShader.setFloat("xMaxBound", X_CONTAINER_MAX);
    forceShader.setFloat("zMaxBound", Z_CONTAINER_MAX);
    forceShader.setFloat("zMinBound", Z_CONTAINER_MIN);
    forceShader.setFloat("damping", DAMPING_COEFF);
    forceShader.setInt("gridSize", gridCountOnSide);
}

int 
Physics::getScanPassCount(int n) {
    if (n <= 1) return 0;

    return std::bit_width((GLuint)n - 1); 
}

// returns the next biggest power of 2
int
Physics::getEffectiveThreadCount(int n) {
    if (n <= 1) return 1;
    return 1 << getScanPassCount(n);
}

unsigned int
Physics::getCountBufferDataSize() {
    return workgroupCount * 4 * sizeof(GLuint); // 4 integers (4 way radix) for each workgroup
}

// swaps the binding base of two buffers taken as parameters
void
Physics::swapBuffers(Buffer& b, GLuint base) {
    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        base,
        b.bufferID
    );
}

void
Physics::swapInputAndOutputBuffers(Buffer& in, Buffer& out) {
    GLuint inBase = in.bufferBindBase;
    GLuint outBase = out.bufferBindBase;

    swapBuffers(in, outBase);
    swapBuffers(out, inBase);

    std::swap(in.bufferBindBase, out.bufferBindBase);
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

void
Physics::setGridCellCount(float side) {
    int gridCountOnSide = std::ceil(side / SMOOTHING_RADIUS);
    
    GRID_CELL_COUNT = gridCountOnSide*gridCountOnSide*gridCountOnSide;
}

void
Physics::refreshBoundarySSBOs() {
    GLuint refreshValue = 0xFFFFFFFF;

    glClearNamedBufferData(
        physicsScene.cell_boundary_startSSBO.bufferID,
        GL_R32UI,
        GL_RED_INTEGER,
        GL_UNSIGNED_INT,
        &refreshValue
    );

    glClearNamedBufferData(
        physicsScene.cell_boundary_endSSBO.bufferID,
        GL_R32UI,
        GL_RED_INTEGER,
        GL_UNSIGNED_INT,
        &refreshValue
    );

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}
