#include "application.h"
#include <random>

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
    activeScene.getGlobalSphere().setRadius(1.0f);

    const int maxParticles = 100000;
    const float minBound = -1000.0f;
    const float maxBound =  1000.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(minBound, maxBound);

    for (int i = 0; i < maxParticles; ++i) {
        SphereInstanceData particle;

        float particleMass = 1.0f;

        // Calculate position
        particle.position_mass = glm::vec4(
            dis(gen),
            dis(gen),
            dis(gen),
            particleMass
        );

        // Color white
        particle.color_padding = glm::vec4(
            1.0f,
            1.0f,
            1.0f,
            0.0f
        );

        particle.velocity_padding = glm::vec4(
            0.0f,
            0.0f,
            0.0f,
            0.0f 
        );

        activeScene.addSphere(particle);
    }

    pEngine.initSSBO();
    rEngine.uploadSphereMesh();

    /*
    SurfaceInstanceData testSurface;
    
    testSurface = activeScene.createSurface(sNormal::Y_NORMAL, 10, -3.0f);
    testSurface.setScale(10.0f);
    testSurface.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));

    activeScene.addSurface(testSurface);
    */
}

