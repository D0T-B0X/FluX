#include "Mesh/Surface3D.h"

Surface3D::Surface3D() { }

SurfaceInstanceData Surface3D::generateSurface(sNormal normal, uint density, float distance) {
    uint numLines = density + 1;

    SurfaceInstanceData surface;

    surface.fvVertices.reserve(4 * numLines);

    uint fixedAxis;
    uint horizontalAxis;
    uint verticalAxis;

    float step = 2.0f / (numLines);

    switch (normal) {
        case sNormal::X_NORMAL: fixedAxis = 0; horizontalAxis = 2; verticalAxis = 1;
        case sNormal::Y_NORMAL: fixedAxis = 1; horizontalAxis = 0; verticalAxis = 2;
        case sNormal::Z_NORMAL: fixedAxis = 2; horizontalAxis = 0; verticalAxis = 1;
    }

    float fv[3];
    fv[fixedAxis] = distance;

    for (int i = 0; i < numLines; ++i) {
        fv[horizontalAxis] = -1.0f + (step * i);
        fv[verticalAxis] = 1.0f;
        
        surface.fvVertices.push_back(fv[0]);
        surface.fvVertices.push_back(fv[1]);
        surface.fvVertices.push_back(fv[2]);

        fv[verticalAxis] = -1.0f;

        surface.fvVertices.push_back(fv[0]);
        surface.fvVertices.push_back(fv[1]);
        surface.fvVertices.push_back(fv[2]);
    }

    for (int i = 0; i < density; ++i) {
        fv[verticalAxis] = -1.0f + (step * i);
        fv[horizontalAxis] = 1.0f;
        
        surface.fvVertices.push_back(fv[0]);
        surface.fvVertices.push_back(fv[1]);
        surface.fvVertices.push_back(fv[2]);

        fv[horizontalAxis] = -1.0f;

        surface.fvVertices.push_back(fv[0]);
        surface.fvVertices.push_back(fv[1]);
        surface.fvVertices.push_back(fv[2]);
    }

    surface.uvIndices = calculateIndices();

    return surface;
}

std::vector<uint> Surface3D::calculateIndices() {
    std::vector<uint> indices;

    return indices;
}