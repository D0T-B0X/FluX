#ifndef SCENE_H
#define SCENE_H

#include "Object/body.h"

// OpenGL and GLFW helper libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Scene {
public:

    // ------ Program timing variables -------
    float          dt;
    float          currTime;
    float          lastTime;

    GLuint         particleSSBO = 0;

    Scene();

    // -------- Global sphere access ---------
    Sphere3D& getGlobalSphere();

    // -------- Sphere control functions ---------
    void addSphere(SphereInstanceData sphere);
    bool hasNoSpheres();
    unsigned int getSpheresDataSize();
    unsigned int getSpheresSize();
    const void* getSpheresData();
    std::vector<SphereInstanceData>& getSpheres();

private:
    // Global sphere mesh
    Sphere3D                         globalSphere;

    // Holds all sphere data in the simulation
    std::vector <SphereInstanceData> Spheres;
};

#endif
