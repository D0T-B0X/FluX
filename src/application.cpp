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
    SphereInstanceData testSphere;

    testSphere.fv3Position   =     glm::vec3(-0.5f, 0.0f, 0.0f);
    testSphere.fv3Color      =     glm::vec3(1.0f, 0.5f, 0.5f);
    testSphere.setEmitter(false);
    testSphere.setRadius(0.5f);

    activeScene.addSphere(testSphere);

    SphereInstanceData blueSphere;
    
    blueSphere.fv3Position   =     glm::vec3(0.5f, 0.0f, 0.0f);
    blueSphere.fv3Color      =     glm::vec3(0.0f, 0.0f, 1.0f);
    blueSphere.setEmitter(false);
    blueSphere.setRadius(0.5f);

    activeScene.addSphere(blueSphere);
}