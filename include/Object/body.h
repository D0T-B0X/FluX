#ifndef BODY_H
#define BODY_H

#include "Mesh/Sphere3D.h"

struct SphereInstanceData {
    glm::vec3   fv3Position;
    glm::vec3   fv3Color;
    float       fRadius = 1.0f;
    float       fEmitter;

    void setRadius(float radius) {
        if (radius < EPSILON) { radius = EPSILON; }
        fRadius = radius;
    }

    void setEmitter(bool status) { fEmitter = status ? 1.0f : 0.0f; }
};

#endif