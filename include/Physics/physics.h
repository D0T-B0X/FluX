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

// -------- Real-time simulation updates --------
    void            updateFrame();
    void            cleanup();
    void            performSpatialHashAndSort();
    void            computeSPHUpdates();

    // -------- Engine setup --------
    void            uploadUinforms();
    void            setupSSBO(Buffer& b);
    void            initSSBOs();
    void            setWorkGroupCount();

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
    Shader          prefixScanShader;
    Shader          globalOffsetSumShader;
    Shader          scatterShader;

    float           SMOOTHING_RADIUS;
    int             workgroupCount;

    GLuint          timeQuery;
    GLuint          iteration;
    double          timeSum;

    // ------------- Uniform setup -------------
    void            setDensityUniforms();
    void            setPressureUniforms();
    void            setForceUniforms();
    void            setGridUniforms();
    void            setPrefixScanUniforms();
    void            setGlobalOffsetSumUniforms();
    void            setScatterUniforms();

    int             getScanPassCount(int n);
    int             getEffectiveThreadCount(int n);
};

#endif