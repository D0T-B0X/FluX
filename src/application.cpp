#include "application.h"
#include <random>

App::App() : rEngine(activeScene), pEngine(activeScene) { }

void App::run() {  
    setup();

    while (!rEngine.shouldEnd()) {
        pEngine.updateFrame();
        rEngine.renderFrame();
    }

    rEngine.cleanup();
    pEngine.cleanup();
}

void App::setup() {
    // Set the global sphere radius 
    activeScene.getGlobalSphere().setRadius(0.005f);

    const int gridX = 16;
    const int gridY = 16;
    const int gridZ = 16;
    const int maxParticles = 4096;

    // Grid bounds
    const float minBound = -0.1f;
    const float maxBound =  0.1f;
    const float range = (maxBound - minBound);

    // Spacing between particles
    const float spacingX = range / (float)(gridX - 1);
    const float spacingY = range / (float)(gridY - 1);
    const float spacingZ = range / (float)(gridZ - 1);

    float totalParticleCount = gridX * gridY * gridZ;
    float totalVolume = range * range * range;
    float massPerParticle = (RESTING_DENSITY * totalVolume) / totalParticleCount;
    int particleCount = 0;

    for (int x = 0; x < gridX && particleCount < maxParticles; ++x) {
        for (int y = 0; y < gridY && particleCount < maxParticles; ++y) {
            for (int z = 0; z < gridZ && particleCount < maxParticles; ++z) {
                Particle particle;

                // Calculate position
                particle.position_mass = glm::vec4(
                    minBound + x * spacingX,
                    minBound + y * spacingY,
                    minBound + z * spacingZ,
                    massPerParticle
                );
                
                particle.velocity_density = glm::vec4(
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f     // Density is calculated in the compute shader
                );

                particle.force_pressure = glm::vec4(
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f     // Pressure is also calculated in the compute shader
                );

                // Color based on position (gradient effect)
                particle.color_padding = glm::vec4(
                    (float)x / (float)gridX,
                    (float)y / (float)gridY,
                    (float)z / (float)gridZ,
                    69.0f    // I'm immature :P
                );

                activeScene.addSphere(particle);
                ++particleCount;
            }
        }
    }
    // Physics setup — SSBO must exist before uploadSphereMesh binds it
    pEngine.initSSBO();

    rEngine.uploadSphereMesh();

    std::cout << "Currently rendering " << particleCount << " particles" << std::endl;

    /*
    SurfaceInstanceData testSurface;
    
    testSurface = activeScene.createSurface(sNormal::Y_NORMAL, 10, -3.0f);
    testSurface.setScale(10.0f);
    testSurface.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));

    activeScene.addSurface(testSurface);
    */

    pEngine.setDensityUniforms();
    pEngine.setPressureUniforms();
    pEngine.setForceUniforms();
}

