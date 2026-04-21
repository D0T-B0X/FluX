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
    void            setDensityUniforms();
    void            setPressureUniforms();
    void            setForceUniforms();
    void            setGridUniforms();
    void            setOrderCheckUniforms();
    void            setPrefixScanUniforms();
    void            setGlobalOffsetSumUniforms();
    void            setScatterUniforms();

// -------- Engine setup --------
    void            setupSSBO(Buffer& b);
    void            initSSBOs();
    void            performSpatialHashAndSort();
    void            setWorkGroupCount();

// -------- Real-time simulation updates --------
    void            updateFrame();
    void            cleanup();
    void            computeSPHUpdates();

// -------- Miscllaneous --------
    unsigned int    getCountBufferDataSize();
    void            swapBuffers(Buffer& b, GLuint base);
    void            swapInputAndOutputBuffers(Buffer& in, Buffer& out);
    void            setSmoothingRadius(float s);
    float           getSmoothingRadius();

    float           timeAccumulator;

private:
    Scene&          physicsScene;
    Shader          densityShader;
    Shader          pressureShader;
    Shader          forceShader;
    Shader          gridHashShader;
    Shader          orderCheckShader;
    Shader          prefixScanShader;
    Shader          globalOffsetSumShader;
    Shader          scatterShader;

    float           SMOOTHING_RADIUS;
    unsigned int    workgroupCount;

    unsigned int    getScanPassCount(unsigned int n);
    unsigned int    getEffectiveThreadCount(unsigned int n);
};

#endif