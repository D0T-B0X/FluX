#include "application.h"

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void
App::run() {  
    setup();

    glfwSwapInterval(1); 
    int check = 0;
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
            if(check++ < 2) pEngine.debugReadback();
            pEngine.timeAccumulator -= PHYSICS_DT;
        }

        rEngine.renderFrame();
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void 
App::setup() {
    // Set the global sphere radius 
    activeScene.getGlobalSphere().setRadius(SPHERE_RADIUS);

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

    for (int x = 0; x < GRID_SIDE && activeScene.getParticleCount() < maxParticles; ++x) {
        for (int y = 0; y < GRID_SIDE && activeScene.getParticleCount() < maxParticles; ++y) {
            for (int z = 0; z < GRID_SIDE && activeScene.getParticleCount() < maxParticles; ++z) {

                // Calculate position
                activeScene.particles.position_mass.push_back(glm::vec4(
                    MIN_BOUND + x * spacing,
                    MIN_BOUND + y * spacing,
                    MIN_BOUND + z * spacing,
                    massPerParticle
                ));
                
                activeScene.particles.velocity_density.push_back(glm::vec4(
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f     // Density is calculated in the compute shader
                ));

                activeScene.particles.force_pressure.push_back(glm::vec4(
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f     // Pressure is also calculated in the compute shader
                ));

                // Color based on position (gradient effect)
                activeScene.particles.color_padding.push_back(glm::vec4(
                    0.3f,
                    0.5f,
                    1.0f,
                    /*
                    (float)x / (float)GRID_SIDE,
                    (float)y / (float)GRID_SIDE,
                    (float)z / (float)GRID_SIDE,
                    */
                    69.0f    // I'm immature :P
                ));

                activeScene.incrementParticleCount();
            }
        }
    }

    /*
    SurfaceInstanceData testSurface;
    
    testSurface = activeScene.createSurface(sNormal::Y_NORMAL, 10, -3.0f);
    testSurface.setScale(10.0f);
    testSurface.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));

    activeScene.addSurface(testSurface);
    */

    // Physics setup
    pEngine.setWorkGroupCount();    
    pEngine.uploadUinforms();
    pEngine.initSSBOs();



    pEngine.debugReadback();
    std::cout << "Particle count at init: " << activeScene.getParticleCount() << std::endl;
    std::cout << "Property data size: " << activeScene.getPropertyDataSize() << std::endl;
    std::cout << "position_massOutSSBO ID: " << activeScene.position_massOutSSBO.bufferID << std::endl;

    // verify the buffer was actually created
    GLint size = 0;
    glGetNamedBufferParameteriv(activeScene.position_massOutSSBO.bufferID, GL_BUFFER_SIZE, &size);
    std::cout << "Actual GPU buffer size: " << size << std::endl;




    // Render setup
    rEngine.uploadSphereMesh();

    std::cout << "Currently rendering " << activeScene.getParticleCount() << " particles" << std::endl;
}

