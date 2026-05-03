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
    void                updateFrame();
    void                performSpatialHashAndSort();
    void                reorderParticleBuffers();
    void                computeCellBoundaries();
    void                computeSPHUpdates();

    // -------- Engine setup --------
    void                uploadUinforms();
    void                setupSSBO(Buffer& b);
    void                initSSBOs();
    void                setWorkGroupCount();

// -------- Miscllaneous --------
    unsigned int        getCountBufferDataSize();
    float               getSmoothingRadius();
    void                swapBuffers(Buffer& b, GLuint base);
    void                swapInputAndOutputBuffers(Buffer& in, Buffer& out);
    void                setSmoothingRadius(float s);
    void                setGridCellCount(float side);
    void                refreshBoundarySSBOs();

    void                cleanup();

    float               timeAccumulator;

private:
    Scene&              physicsScene;
    Shader              gridHashShader;
    Shader              prefixScanShader;
    Shader              globalOffsetSumShader;
    Shader              scatterShader;
    Shader              reorderBuffersShader;
    Shader              computeCellBoundariesShader;
    Shader              densityShader;
    Shader              pressureShader;
    Shader              forceShader;

    float               SMOOTHING_RADIUS;
    int                 GRID_CELL_COUNT;
    int                 workgroupCount;

    GLuint              timeQuery;
    GLuint              iteration;
    double              timeSum;

    // ------------- Uniform setup -------------
    void                setGridUniforms();
    void                setPrefixScanUniforms();
    void                setGlobalOffsetSumUniforms();
    void                setScatterUniforms();
    void                setReorderBuffersUniforms();
    void                setComputeCellBoundariesUniforms();
    void                setDensityUniforms();
    void                setPressureUniforms();
    void                setForceUniforms();

    int                 getScanPassCount(int n);
    int                 getEffectiveThreadCount(int n);
};

#endif