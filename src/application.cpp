#include "application.h"
#include <random>

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void
App::run() {  
    setup();

    while (!rEngine.shouldEnd()) {
        // Scene timing update
        activeScene.currTime = glfwGetTime();
        if (activeScene.lastTime == 0.0f) activeScene.lastTime = activeScene.currTime;
        activeScene.dt = activeScene.currTime - activeScene.lastTime;
        activeScene.lastTime = activeScene.currTime;
        
        pEngine.timeAccumulator += activeScene.dt;
        rEngine.timeAccumulator += activeScene.dt;

        // advance physics engine if time step is 2ms
        while (pEngine.timeAccumulator >= PHYSICS_DT) {
            std::cout << "Physics time: " << pEngine.timeAccumulator << std::endl;
            pEngine.updateFrame();
            pEngine.timeAccumulator -= PHYSICS_DT;
        }

        // advance render engine if time step is 16ms
        while (rEngine.timeAccumulator >= RENDER_DT) {
            std::cout << "Render time: " << rEngine.timeAccumulator << std::endl;
            rEngine.renderFrame();
            rEngine.timeAccumulator -= RENDER_DT;
        }
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void 
App::setup() {
    // Set the global sphere radius 
    activeScene.getGlobalSphere().setRadius(0.05f);

    const int gridD = 16;
    const int maxParticles = gridD*gridD*gridD;

    // Grid bounds
    const float minBound = -1.0f;
    const float maxBound =  1.0f;
    const float range = (maxBound - minBound);

    // Spacing between particles
    const float spacing = range / (float)(gridD - 1);
    pEngine.setSmoothingRadius(spacing * 2.5f); // smoothing radius should be 2.5x the spacing

    float totalParticleCount = maxParticles;
    float totalVolume = range * range * range;
    float massPerParticle = (RESTING_DENSITY * totalVolume) / totalParticleCount;

    for (int x = 0; x < gridD && activeScene.particleCount < maxParticles; ++x) {
        for (int y = 0; y < gridD && activeScene.particleCount < maxParticles; ++y) {
            for (int z = 0; z < gridD && activeScene.particleCount < maxParticles; ++z) {

                // Calculate position
                activeScene.particles.position_mass.push_back(glm::vec4(
                    minBound + x * spacing,
                    minBound + y * spacing,
                    minBound + z * spacing,
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
                    (float)x / (float)gridD,
                    (float)y / (float)gridD,
                    (float)z / (float)gridD,
                    69.0f    // I'm immature :P
                ));

                ++activeScene.particleCount;
            }
        }
    }
    
    // SSBO setup
    pEngine.initSSBO();

    rEngine.uploadSphereMesh();

    std::cout << "Currently rendering " << activeScene.particleCount << " particles" << std::endl;

    /*
    SurfaceInstanceData testSurface;
    
    testSurface = activeScene.createSurface(sNormal::Y_NORMAL, 10, -3.0f);
    testSurface.setScale(10.0f);
    testSurface.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));

    activeScene.addSurface(testSurface);
    */

    // Physics setup
    pEngine.setDensityUniforms();
    pEngine.setPressureUniforms();
    pEngine.setForceUniforms();
}

