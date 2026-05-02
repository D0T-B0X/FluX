#ifndef SCENE_H
#define SCENE_H

#define FLOOR -20.0f

#include "Object/body.h"
#include "Object/buffer.h"

// OpenGL and GLFW helper libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Scene {
public:

    // ------ Program timing variables -------
    float          dt;
    float          currTime;
    float          lastTime;

    // ------ Simulation storage buffers ------
    Buffer         position_massInSSBO;
    Buffer         velocity_densityInSSBO;
    Buffer         force_pressureInSSBO;
    Buffer         color_paddingInSSBO;
    Buffer         cell_index_oneSSBO;
    Buffer         particle_index_oneSSBO;
    Buffer         abortFlag_buffSSBO;    
    Buffer         gloablOffset_buffSSBO;
    Buffer         blockSum_buffSSBO;
    Buffer         cell_index_twoSSBO;
    Buffer         particle_index_twoSSBO;
    Buffer         position_massOutSSBO;
    Buffer         velocity_densityOutSSBO;
    Buffer         force_pressureOutSSBO;
    Buffer         color_paddingOutSSBO;

    // Holds all sphere data in the simulation
    Particles                           particles;

    Scene();

    // -------- Global sphere access ---------
    Sphere3D&                           getGlobalSphere();

    // -------- Sphere control functions ---------
    void                                addSurface(SurfaceInstanceData surface);
    SurfaceInstanceData                 createSurface(sNormal normal, uint density, float distance);
    bool                                hasNoSpheres();

    // -------- Physical data flow --------
    int                                 getParticleCount();
    void                                incrementParticleCount();
    unsigned int                        getPropertyDataSize();
    unsigned int                        getParticleCountSize();
    const void*                         getPositionMassData();
    const void*                         getVelocityDensityData();
    const void*                         getForcePressureData();
    const void*                         getColorPaddingData();
    Particles&                          getSpheres();

private:
    // Global sphere mesh
    Sphere3D                            globalSphere;

    // Box mesh
    Surface3D                           surfaceMesh;   

    // Holds all surface instances in the sim
    std::vector<SurfaceInstanceData>    Surfaces; 

    int                                 particleCount;
};

#endif
