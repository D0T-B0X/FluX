#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer/renderer.h"
#include "Physics/physics.h"

class App {
public:
    App();

    void init();
    void run();

private:
    Scene       activeScene;
    Renderer    rEngine;
    Physics     pEngine;

};

#endif