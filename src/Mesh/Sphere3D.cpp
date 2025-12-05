#include "Mesh/Sphere3D.h"

// ____ Constructors ____
Sphere3D::Sphere3D() : uSubdivisions(16) {
    // Two extra points per row for start and end
    uPointsPerRow = uSubdivisions + 2;

    // Points per face is simply the square of points per row
    uPointsPerFace = uPointsPerRow * uPointsPerRow; 

    buildSphere();
}

Sphere3D::Sphere3D(uint subdivs) : uSubdivisions(subdivs) {
    uPointsPerRow = uSubdivisions + 2;
    uPointsPerFace = uPointsPerRow * uPointsPerRow; 

    buildSphere();
}

// ____ Getter functions ____
float* Sphere3D::getVertices() {
    return vfVertices.data();
}

uint Sphere3D::getVerticesSize() {
    return (uint)vfVertices.size() * sizeof(float);
}

uint* Sphere3D::getIndices() {
    return vuIndices.data();
}

uint Sphere3D::getIndicesSize() {
    return (uint)vuIndices.size() * sizeof(uint);
}

uint Sphere3D::getIndexCount() {
    return (uint)vuIndices.size();
}

uint Sphere3D::getSubdivisions() {
    return uSubdivisions;
}

// ____ Setter functions ____
void Sphere3D::setSubdivision(uint uSubdivs) {
    uSubdivisions = uSubdivs;

    uPointsPerRow = uSubdivisions + 2;
    uPointsPerFace = uPointsPerRow * uPointsPerRow;

    buildSphere();
}

// ____ Sphere Generation ____
void Sphere3D::buildSphere() {
    std::vector<float> fvTmpVertices;
    for (int face = 0; face < 6; ++face) {
        switch (face) {
            // Build vertices for X face
            case 0: fvTmpVertices = buildFaceVertices(Face::X, POS); break;
            case 1: fvTmpVertices = buildFaceVertices(Face::X, NEG); break;

            // Build vertices for Y face
            case 2: fvTmpVertices = buildFaceVertices(Face::Y, POS); break;
            case 3: fvTmpVertices = buildFaceVertices(Face::Y, NEG); break;

            // Build vertices for Z face
            case 4: fvTmpVertices = buildFaceVertices(Face::Z, POS); break;
            case 5: fvTmpVertices = buildFaceVertices(Face::Z, NEG); break;
        }

        for (int i = 0; i < uPointsPerFace; ++i) {
            float fV[3];

            fV[0] = fvTmpVertices[3 * i];
            fV[1] = fvTmpVertices[3 * i + 1];
            fV[2] = fvTmpVertices[3 * i + 2];

            vfVertices.push_back(fV[0]); // x
            vfVertices.push_back(fV[1]); // y
            vfVertices.push_back(fV[2]); // z
        }
    }

    buildIndices();
}

void Sphere3D::buildIndices() {

    for (int face = 0; face < 6; ++face) {
        for(int i = 0; i < uPointsPerRow - 1; ++i) {
            uint uTopLeft;
            uint uTopRight;
            uint uBottomLeft;
            uint uBottomRight;

            for (int j = 0; j < uPointsPerRow - 1; ++j) {
                uint uBaseIndex = (face * uPointsPerFace) + uPointsPerRow * i + j;

                uTopRight     =   uBaseIndex + 1;
                uTopLeft      =   uBaseIndex;
                uBottomRight  =   uBaseIndex + uPointsPerRow + 1;
                uBottomLeft   =   uBaseIndex + uPointsPerRow;

                /**
                 * 
                 *  A quad is made up of two triangles 
                 *  ____       
                 *  |  /       /|
                 *  | /   +   / |
                 *  |/       /__|
                 * 
                 */

                // Triangle 1
                vuIndices.push_back(uTopLeft);
                vuIndices.push_back(uBottomLeft);
                vuIndices.push_back(uTopRight);

                // Triangle 2
                vuIndices.push_back(uTopRight);
                vuIndices.push_back(uBottomLeft);
                vuIndices.push_back(uBottomRight);
            }
        }
    }
}

std::vector<float> Sphere3D::buildFaceVertices(Face face, int iDirection) {
    std::vector<float> fvTmpVertices;
    uint uConstAxis;
    uint uHorizontalAxis;
    uint uVerticalAxis;

    switch (face) {
        case X: uConstAxis = 0; uHorizontalAxis = 2; uVerticalAxis = 1; break;
        case Y: uConstAxis = 1; uHorizontalAxis = 0; uVerticalAxis = 2; break;
        case Z: uConstAxis = 2; uHorizontalAxis = 0; uVerticalAxis = 1; break;
    }

    float fInc = 2.0 / (float)(uPointsPerRow - 1);
    for (uint i = 0; i < uPointsPerRow; ++i) {
        float fV[3];

        fV[uConstAxis] = (float)iDirection;
        fV[uVerticalAxis] = 1.0f - (fInc * i);
        for (uint j = 0; j < uPointsPerRow; ++j) {
            fV[uHorizontalAxis] = -1.0f + (fInc * j);

            fvTmpVertices.push_back(fV[0]);
            fvTmpVertices.push_back(fV[1]);
            fvTmpVertices.push_back(fV[2]);
        }
    }

    return fvTmpVertices;
}
