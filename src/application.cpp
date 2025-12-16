#include "application.h"

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void App::run() {  
    setup();
    
    while (!rEngine.shouldEnd()) {
        rEngine.renderFrame();
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void App::setup() {
    // Set the global sphere radius (smaller for 1M particles)
    activeScene.getGlobalSphere().setRadius(0.015f);

    // Generate 1,000,000 equally spaced particles in a 3D grid
    // 100 x 100 x 100 = 1,000,000
    const int gridX = 46;
    const int gridY = 46;
    const int gridZ = 46;
    const int maxParticles = 1000000;

    // Grid bounds
    const float minBound = -0.8f;
    const float maxBound =  0.8f;
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
                particle.fv3Position = glm::vec3(
                    minBound + x * spacingX,
                    minBound + y * spacingY,
                    minBound + z * spacingZ
                );

                // Color based on position (gradient effect)
                particle.fv3Color = glm::vec3(
                    (float)x / (float)gridX,
                    (float)y / (float)gridY,
                    (float)z / (float)gridZ
                );

                activeScene.addSphere(particle);
                ++particleCount;
            }
        }
    }
}