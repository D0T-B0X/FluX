#ifndef SPHERE_3D_H
#define SPHERE_3D_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

typedef unsigned int uint;
inline constexpr float EPSILON = 1e-3;

#define POS  1
#define NEG -1

enum Face {
    X = 1,
    Y = 2,
    Z = 3
};

class Sphere3D {
public:
    Sphere3D();
    Sphere3D(uint subdivs);

    void buildSphere();

    // Getter functions
    float*    getVertices();
    uint      getVerticesSize();
    uint*     getIndices();
    uint      getIndicesSize();
    uint      getIndexCount();
    uint      getSubdivisions();
    float     getRadius();
    bool      isMeshDrity();

    // Setter functions
    void setSubdivision(uint uSubdivs);
    void setRadius(float radius);

private:
    uint                 uSubdivisions; 
    uint                 uPointsPerRow; 
    uint                 uPointsPerFace;
    bool                 bDirtyMesh;
    float                fRadius;
    std::vector<float>   vfVertices;
    std::vector<uint>    vuIndices;

    std::vector<float>   buildFaceVertices(Face face, int iDirection);
    void                 buildIndices();
};

#endif
