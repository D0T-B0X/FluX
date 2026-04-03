#ifndef SCENE_H
#define SCENE_H

#define FLOOR -20.0f

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
    unsigned int   particleCount;

    GLuint         position_massSSBO;
    GLuint         velocity_densitySSBO;
    GLuint         force_pressureSSBO;
    GLuint         color_paddingSSBO;

    // Holds all sphere data in the simulation
    Particles                           particles;

    Scene();

    // -------- Global sphere access ---------
    Sphere3D&                           getGlobalSphere();

    // -------- Sphere control functions ---------
    void                                addSurface(SurfaceInstanceData surface);
    SurfaceInstanceData                 createSurface(sNormal normal, uint density, float distance);
    bool                                hasNoSpheres();
    unsigned int                        getParticleCount();

    // -------- Physical data flow --------
    unsigned int                        getPropertyDataSize();
    const void*                         getPositionMassData();
    const void*                         getVelocityDensityData();
    const void*                         getForcePressureData();
    const void*                         getColorPaddingData();
    Particles&                          getSpheres();

private:
    // Global sphere mesh
    Sphere3D                            globalSphere;

    Surface3D                           surfaceMesh;   

    // Holds all surface instances in the sim
    std::vector<SurfaceInstanceData>    Surfaces; 
};

#endif
