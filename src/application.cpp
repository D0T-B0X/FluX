#include "application.h"
#include <random>

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void App::run() {  
    setup();
    pEngine.initSSBO();
    rEngine.uploadSphereMesh();
    
    while (!rEngine.shouldEnd()) {
        pEngine.updateFrame();
        rEngine.renderFrame();
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void App::setup() {
    // Set the global sphere radius 
    activeScene.getGlobalSphere().setRadius(0.5f);

    const int gridX = 60;
    const int gridY = 60;
    const int gridZ = 60;
    const int maxParticles = 100000000;

    // Grid bounds
    const float minBound = -50.0f;
    const float maxBound =  50.0f;
    const float range = maxBound - minBound;

    // Spacing between particles
    const float spacingX = range / (float)(gridX - 1);
    const float spacingY = range / (float)(gridY - 1);
    const float spacingZ = range / (float)(gridZ - 1);

    int particleCount = 0;

    for (int x = 0; x < gridX && particleCount < maxParticles; ++x) {
        for (int y = 0; y < gridY && particleCount < maxParticles; ++y) {
            for (int z = 0; z < gridZ && particleCount < maxParticles; ++z) {
                SphereInstanceData particle;

                // Calculate position
                particle.position_mass = glm::vec4(
                    minBound + x * spacingX,
                    minBound + y * spacingY,
                    minBound + z * spacingZ,
                    0.0f
                );

                // Color based on position (gradient effect)
                particle.color_padding = glm::vec4(
                    (float)x / (float)gridX,
                    (float)y / (float)gridY,
                    (float)z / (float)gridZ,
                    0.0f
                );

                particle.velocity_padding = glm::vec4(
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0
                );

                activeScene.addSphere(particle);
                ++particleCount;
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
}

