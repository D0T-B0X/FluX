#include "Physics/physics.h"

Physics::Physics(Scene& activeScene) 
    : 
    physicsScene(activeScene),
    timeAccumulator(0),
    workgroupCount(0),
    frame(1)
{ 
    SMOOTHING_RADIUS = 0.0f;
    gridHashShader.load(GRID_CELL_CSHADER_PATH);
    orderCheckShader.load(ORDER_CHECK_CSHADER_PATH);
    prefixScanShader.load(LOCAL_PREFIX_SCAN_CSHADER_PATH);
    globalOffsetSumShader.load(GLOBAL_OFFSET_SUM_CSHADER_PATH);
    scatterShader.load(SCATTER_CSHADER_PATH);
    densityShader.load(DENSITY_CSHADER_PATH);
    pressureShader.load(PRESSURE_CSHADER_PATH);
    forceShader.load(FORCE_CSHADER_PATH);
}

void 
Physics::updateFrame() {

    performSpatialHashAndSort();
    computeSPHUpdates();
}

void 
Physics::cleanup() {
    
}

void
Physics::uploadUinforms() {
    setGridUniforms();
    setDensityUniforms();
    setPressureUniforms();
    setForceUniforms();
    setOrderCheckUniforms();
    setPrefixScanUniforms();
    setGlobalOffsetSumUniforms();
    setScatterUniforms();
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

    // -------- First Cell Index buffer --------
    physicsScene.cell_index_oneSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.cell_index_oneSSBO.bufferData = 0;  // empty buffer
    physicsScene.cell_index_oneSSBO.bufferBindBase = 4;
    setupSSBO(physicsScene.cell_index_oneSSBO);   

    // -------- First Particle Index buffer --------
    physicsScene.particle_index_oneSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.particle_index_oneSSBO.bufferData = 0;  // empty buffer
    physicsScene.particle_index_oneSSBO.bufferBindBase = 5;
    setupSSBO(physicsScene.particle_index_oneSSBO);   

    // -------- Abort Flag buffer --------
    physicsScene.abortFlag_buffSSBO.bufferDataSize = sizeof(unsigned int); // a single unsigned integer to hold the flag value
    physicsScene.abortFlag_buffSSBO.bufferData = 0;
    physicsScene.abortFlag_buffSSBO.bufferBindBase = 6;
    setupSSBO(physicsScene.abortFlag_buffSSBO);

    // -------- Global Offset buffer --------
    physicsScene.gloablOffset_buffSSBO.bufferDataSize = sizeof(unsigned int) * (workgroupCount * 4);
    physicsScene.gloablOffset_buffSSBO.bufferData = 0;
    physicsScene.gloablOffset_buffSSBO.bufferBindBase = 7;
    setupSSBO(physicsScene.gloablOffset_buffSSBO);

    // -------- Global Offset buffer --------
    physicsScene.blockSum_buffSSBO.bufferDataSize = sizeof(unsigned int) * (workgroupCount * 4);
    physicsScene.blockSum_buffSSBO.bufferData = 0;
    physicsScene.blockSum_buffSSBO.bufferBindBase = 8;
    setupSSBO(physicsScene.blockSum_buffSSBO);

    // -------- Second Cell Index buffer --------
    physicsScene.cell_index_twoSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.cell_index_twoSSBO.bufferData = 0;  // empty buffer
    physicsScene.cell_index_twoSSBO.bufferBindBase = 9;
    setupSSBO(physicsScene.cell_index_twoSSBO);   

    // -------- Second Particle Index buffer --------
    physicsScene.particle_index_twoSSBO.bufferDataSize = physicsScene.getParticleCountSize();
    physicsScene.particle_index_twoSSBO.bufferData = 0;  // empty buffer
    physicsScene.particle_index_twoSSBO.bufferBindBase = 10;
    setupSSBO(physicsScene.particle_index_twoSSBO);   
}

void 
Physics::performSpatialHashAndSort() {

    double totalIterationTime = 0.0;

    GLuint timeQuery;
    glGenQueries(1, &timeQuery);

    glBeginQuery(GL_TIME_ELAPSED, timeQuery);

    /**
     ***************************************
     * * * * G R I D   H A S H I N G * * * *
     ***************************************
     */
    gridHashShader.use();

    glDispatchCompute(workgroupCount, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glEndQuery(GL_TIME_ELAPSED);

    GLuint64 timeElapsedNs;
    glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
    double time_ms = timeElapsedNs / 1000000.0;
    std::cout << "Grid hash takes " << time_ms << "ms to complete" << std::endl;

    totalIterationTime += time_ms;

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
         * Order checking to ensure an early exit 
         * if all particles are already sorted.
         * 
         * The check exits early by utilizing a flag
         * which forces all threads to skip execution
         * at the presense of even a single particle 
         * thats out of place
         */

        // reset the flag to zero before computation
        GLuint zero = 0;
        glNamedBufferSubData(
            physicsScene.abortFlag_buffSSBO.bufferID, 
            0, 
            sizeof(GLuint), 
            &zero
        );
        
        glBeginQuery(GL_TIME_ELAPSED, timeQuery);

        // begin order check
        orderCheckShader.use();

        glDispatchCompute(workgroupCount, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glEndQuery(GL_TIME_ELAPSED);

        glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
        time_ms = timeElapsedNs / 1000000.0;
        std::cout << "Order check takes " << time_ms << "ms to complete" << std::endl;

        totalIterationTime += time_ms;

        // extract the value of abort flag
        GLuint abortFlagResult = 0;
        glGetNamedBufferSubData(
            physicsScene.abortFlag_buffSSBO.bufferID, 
            0, 
            sizeof(GLuint), 
            &abortFlagResult
        );

        // if the buffer is sorted no need to continue
        if (abortFlagResult == 0) break;

        glBeginQuery(GL_TIME_ELAPSED, timeQuery);

        /**
         * PHASE 2
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

        glEndQuery(GL_TIME_ELAPSED);    

        glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
        time_ms = timeElapsedNs / 1000000.0;
        std::cout << "Prefix scan takes " << time_ms << "ms to complete" << std::endl;

        totalIterationTime += time_ms;

        glBeginQuery(GL_TIME_ELAPSED, timeQuery);

        /**
         * PHASE 3
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

        glEndQuery(GL_TIME_ELAPSED);    

        glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
        time_ms = timeElapsedNs / 1000000.0;
        std::cout << "Global offset takes " << time_ms << "ms to complete" << std::endl;

        totalIterationTime += time_ms;

        glBeginQuery(GL_TIME_ELAPSED, timeQuery);

        /**
         * PHASE 4
         *  
         * Final scatter phase to globally sort all particles 
         * and swap buffers in ping-pong manner
         */
        scatterShader.use();
        scatterShader.setInt("shift_key", shift_key);

        glDispatchCompute(workgroupCount, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glEndQuery(GL_TIME_ELAPSED);    

        glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
        time_ms = timeElapsedNs / 1000000.0;
        std::cout << "Scatter takes " << time_ms << "ms to complete" << std::endl << std::endl;

        totalIterationTime += time_ms;

        if (swapBufferOneWithTwo) {
            Buffer& inCellIndex = physicsScene.cell_index_oneSSBO;
            Buffer& outCellIndex = physicsScene.cell_index_twoSSBO;
            swapInputAndOutputBuffers(inCellIndex, outCellIndex);

            Buffer& inParticleIndex = physicsScene.particle_index_oneSSBO;
            Buffer& outParticleIndex = physicsScene.particle_index_twoSSBO;
            swapInputAndOutputBuffers(inParticleIndex, outParticleIndex);

            swapBufferOneWithTwo = false;
        } else {
            Buffer& inCellIndex = physicsScene.cell_index_twoSSBO;
            Buffer& outCellIndex = physicsScene.cell_index_oneSSBO;
            swapInputAndOutputBuffers(inCellIndex, outCellIndex);

            Buffer& inParticleIndex = physicsScene.particle_index_twoSSBO;
            Buffer& outParticleIndex = physicsScene.particle_index_oneSSBO;
            swapInputAndOutputBuffers(inParticleIndex, outParticleIndex);

            swapBufferOneWithTwo = true;
        }
    }

    std::cout << "One Spatial Hash Iteration takes " << totalIterationTime << "ms to complete" << std::endl;
}

void
Physics::setWorkGroupCount() {
    // Lacks proper error handling :/
    if (physicsScene.getParticleCount() < 1) { workgroupCount = 1; return; }

    int PARTCILES_PROCESSED_PER_WORKGROUP = THREADS_PER_GROUP * 2;
    workgroupCount = ceil((float)physicsScene.getParticleCount() / (float)PARTCILES_PROCESSED_PER_WORKGROUP);

    std::cout << "Workgroup count: " << workgroupCount << std::endl;
}

void
Physics::computeSPHUpdates() {
    
    double totalIterationTime = 0.0;

    GLuint timeQuery;
    glGenQueries(1, &timeQuery);


    glBeginQuery(GL_TIME_ELAPSED, timeQuery);

    // Density calculations pass
    densityShader.use();
    glDispatchCompute(workgroupCount * 2, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glEndQuery(GL_TIME_ELAPSED);

    GLuint64 timeElapsedNs;
    glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
    double time_ms = timeElapsedNs / 1000000.0;
    std::cout << "Density takes " << time_ms << "ms to complete" << std::endl; 

    totalIterationTime += time_ms;

    glBeginQuery(GL_TIME_ELAPSED, timeQuery);

    // Pressure calculations pass
    pressureShader.use();
    glDispatchCompute(workgroupCount * 2, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glEndQuery(GL_TIME_ELAPSED);

    glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
    time_ms = timeElapsedNs / 1000000.0;
    std::cout << "Pressure takes " << time_ms << "ms to complete" << std::endl;

    totalIterationTime += time_ms;

    glBeginQuery(GL_TIME_ELAPSED, timeQuery);

    // Pressure force calculations pass
    forceShader.use();
    forceShader.setFloat("dt", PHYSICS_DT);
    glDispatchCompute(workgroupCount * 2, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glEndQuery(GL_TIME_ELAPSED);

    glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &timeElapsedNs);
    time_ms = timeElapsedNs / 1000000.0;
    std::cout << "Force takes " << time_ms << "ms to complete" << std::endl;

    totalIterationTime += time_ms;

    std::cout << "SPH Calculations take " << totalIterationTime << "ms to complete" << std::endl << std::endl << std::endl << std::endl;
}

void 
Physics::setGridUniforms() {
    gridHashShader.use();

    gridHashShader.setInt("numParticles", physicsScene.getParticleCount());
    // slighly larger cell size to collect all particles
    gridHashShader.setFloat("cell_size", 1.1 * SMOOTHING_RADIUS); 
    gridHashShader.setInt("gridSize", GRID_SIDE);
    gridHashShader.setVec3("gridMin", glm::vec3(MIN_BOUND, MIN_BOUND, MIN_BOUND));
}

void 
Physics::setDensityUniforms() {
    densityShader.use();

    densityShader.setInt("numParticles", physicsScene.getParticleCount());
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

    pressureShader.setInt("numParticles", physicsScene.getParticleCount());
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

    forceShader.setInt("numParticles", physicsScene.getParticleCount());

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
Physics::setOrderCheckUniforms() {
    orderCheckShader.use();

    orderCheckShader.setInt("totalParticleCount", physicsScene.getParticleCount());
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
