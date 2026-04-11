#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <bit>

#include "settings.h"
#include "scene.h"
#include "config.h"
#include "Renderer/shader.h"

class Physics {
public: 
    Physics(Scene& activeScene);

// -------- Uniform uploads --------
    void            setCountSortUniforms();
    void            setDensityUniforms();
    void            setPressureUniforms();
    void            setForceUniforms();
    void            setGridUniforms();
    void            setLocalScanUniforms();
    void            setBlockSumScanUniforms();
    void            setCombineUniforms();

// -------- Engine setup --------
    void            setupSSBO(Buffer& b);
    void            initSSBOs();
    void            buildGrid();
    void            setWorkGroupCount();

// -------- Real-time simulation updates --------
    void            updateFrame();
    void            cleanup();
    void            computeSPHUpdates();

// -------- Miscllaneous --------
    unsigned int    getCountBufferDataSize();
    void            setSmoothingRadius(float s);
    float           getSmoothingRadius();

    float           timeAccumulator;

private:
    Scene&          physicsScene;
    Shader          densityShader;
    Shader          pressureShader;
    Shader          forceShader;
    Shader          gridHashShader;
    Shader          countBufferShader;
    Shader          localScanShader;
    Shader          blockSumScanShader;
    Shader          combineShader;

    float           SMOOTHING_RADIUS;
    unsigned int    workgroupCount;

    unsigned int    getScanPassCount(unsigned int n);
};

#endif