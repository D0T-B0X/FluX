#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

#include "settings.h"
#include "scene.h"
#include "config.h"
#include "Renderer/shader.h"

class Physics {
public: 
    Physics(Scene& activeScene);

    void            setCountSortUniforms();
    void            setDensityUniforms();
    void            setPressureUniforms();
    void            setForceUniforms();
    void            setGridUniforms();
    void            updateFrame();
    void            cleanup();
    void            setupSSBO(Buffer& b);
    void            initSSBOs();
    void            buildGrid();
    void            updateSPH();
    unsigned int    getCountBufferDataSize();
    void            setSmoothingRadius(float s);
    float           getSmoothingRadius();

    float           timeAccumulator;
    unsigned int    numGroups;

private:
    Scene&          physicsScene;
    Shader          densityShader;
    Shader          pressureShader;
    Shader          forceShader;
    Shader          gridHashShader;
    Shader          countBufferShader;
    Shader          prefixSumShader;

    float           SMOOTHING_RADIUS;
};

#endif