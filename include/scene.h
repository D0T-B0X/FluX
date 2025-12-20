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

    GLuint         particleSSBO;

    Scene();

    // -------- Global sphere access ---------
    Sphere3D&                           getGlobalSphere();

    // -------- Sphere control functions ---------
    void                                addSphere(SphereInstanceData sphere);
    void                                addSurface(SurfaceInstanceData surface);
    SurfaceInstanceData                 createSurface(sNormal normal, uint density, float distance);
    bool                                hasNoSpheres();
    unsigned int                        getSpheresDataSize();
    unsigned int                        getSpheresSize();
    const void*                         getSpheresData();
    std::vector<SphereInstanceData>&    getSpheres();

private:
    // Global sphere mesh
    Sphere3D                            globalSphere;

    Surface3D                           surfaceMesh;   

    // Holds all sphere data in the simulation
    std::vector<SphereInstanceData>     Spheres;

    // Holds all surface instances in the sim
    std::vector<SurfaceInstanceData>    Surfaces; 
};

#endif
