#include "application.h"

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void
App::run() {  
    glfwSwapInterval(1); 
    while (!rEngine.shouldEnd()) {
        
        // get real time difference between frames
        activeScene.currTime = glfwGetTime();
        if (activeScene.lastTime == 0.0f) activeScene.lastTime = activeScene.currTime;
        activeScene.dt = activeScene.currTime - activeScene.lastTime;
        activeScene.lastTime = activeScene.currTime;

        if (activeScene.dt > 0.25) {
            activeScene.dt = 0.25; 
        }

        pEngine.timeAccumulator += activeScene.dt;

        // Advance physics engine in a pre-defined fixed timestep
        while (pEngine.timeAccumulator >= PHYSICS_DT) {
            pEngine.updateFrame();
            pEngine.timeAccumulator -= PHYSICS_DT;
        }

        rEngine.renderFrame();
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void 
App::init() {
    // Set the global sphere radius
    Sphere3D& sphere = activeScene.getGlobalSphere(); 
    sphere.setRadius(SPHERE_RADIUS);

    const int maxParticles = GRID_SIDE*GRID_SIDE*GRID_SIDE; // defined in settings.h

    // Grid bounds
    const float range = (MAX_BOUND - MIN_BOUND); // defined in settings.h

    // Spacing between particles
    const float spacing = range / (float)(GRID_SIDE - 1);
    pEngine.setSmoothingRadius(spacing * 2.5f); // smoothing radius should be 2.5x the spacing
    pEngine.setGridCellCount(range); 

    float totalParticleCount = maxParticles;
    float totalVolume = range * range * range;
    float massPerParticle = (RESTING_DENSITY * totalVolume) / totalParticleCount;

    Particles& sceneParticles = activeScene.particles;
    sceneParticles.position_mass.resize(maxParticles);
    sceneParticles.velocity_density.resize(maxParticles);
    sceneParticles.force_pressure.resize(maxParticles);
    sceneParticles.color_padding.resize(maxParticles);

    for (int x = 0; x < GRID_SIDE; ++x) {
    for (int y = 0; y < GRID_SIDE; ++y) {
    for (int z = 0; z < GRID_SIDE; ++z) {

        int particleIndex = (x * GRID_SIDE + y) * GRID_SIDE + z;

        // Calculate position
        activeScene.particles.position_mass[particleIndex] = glm::vec4(
            MIN_BOUND + x * spacing,
            MIN_BOUND + y * spacing,
            MIN_BOUND + z * spacing,
            massPerParticle
        );
        
        sceneParticles.velocity_density[particleIndex] = glm::vec4(
            0.0f,
            0.0f,
            0.0f,
            0.0f     // Density is calculated in the compute shader
        );

        sceneParticles.force_pressure[particleIndex] = glm::vec4(
            0.0f,
            0.0f,
            0.0f,
            0.0f     // Pressure is also calculated in the compute shader
        );

        // Color based on position (gradient effect)
        sceneParticles.color_padding[particleIndex] = glm::vec4(
            0.3f,
            0.5f,
            1.0f,
            69.0f    // ^_^
        );

        activeScene.incrementParticleCount();
    
    } // z
    } // y
    } // x

    // Physics setup
    pEngine.setWorkGroupCount();    
    pEngine.initSSBOs();
    pEngine.uploadUinforms();


#ifdef DEBUG
    std::cout << "Particle count at init: " << activeScene.getParticleCount() << std::endl;
    std::cout << "Property data size: " << activeScene.getPropertyDataSize() << std::endl;
    std::cout << "position_massOutSSBO ID: " << activeScene.position_massOutSSBO.bufferID << std::endl;

    // verify the buffer was actually created
    GLint size = 0;
    glGetNamedBufferParameteriv(activeScene.position_massOutSSBO.bufferID, GL_BUFFER_SIZE, &size);
    std::cout << "Actual GPU buffer size: " << size << std::endl;
#endif

    // Render setup
    rEngine.uploadSphereMesh();

    std::cout << "Currently rendering " << activeScene.getParticleCount() << " particles" << std::endl;
}

